package core.register.zookeeper;

import com.alibaba.fastjson.JSON;
import core.event.rpc_node_change_event;
import core.event.rpc_update_event;
import core.register.URL;
import core.register.Register_Service;
import core.event.rpc_event;
import core.event.Rpc_Listener_Loader;
import rpc_interface.data_service;

import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

//主要负责的功能是对Zookeeper完成服务注册，服务订阅，服务下线等相关实际操作
public class Zookeeper_Register extends Abstract_Register implements Register_Service {

    private abstract_zookeeper_client client;

    private String root="/rpc";

    private String get_provider_path(URL url){ //通过url获取provider path
        return root+"/"+url.get_service_name()+"/provider/"+url.get_param().get("host")+":"+url.get_param().get("port");
    }

    private String get_consumer_path(URL url) {//通过url获取consumer path
        return root + "/" + url.get_service_name() + "/consumer/" + url.get_app_name() + ":" + url.get_param().get("host")+":";
    }

    public Zookeeper_Register(String address){
        this.client = new curator_client(address);
    }

    @Override
    public void register(URL url) { //server
        if(!this.client.existNode(root)){
            client.create_persistent_data(root,"");
        }

        String url_str = URL.build_provider_url(url);

        if(!client.existNode(get_provider_path(url))){
            client.create_temporary_data(get_provider_path(url),url_str);
        }
        else{
            client.deleteNode(get_provider_path(url));
            client.create_temporary_data(get_provider_path(url),url_str);
        }
        super.register(url);
    }

    @Override
    public void un_register(URL url) {
        client.deleteNode(get_provider_path(url));
        super.un_register(url);
    }

    @Override
    public void subscribe(URL url) {//client
        if(!this.client.existNode(root)){
            client.create_persistent_data(root,"");
        }

        String url_str = URL.build_consumer_url(url);

        if(!client.existNode(get_consumer_path(url))){
            client.create_temporary_data(get_consumer_path(url),url_str);
        }
        else{
            client.deleteNode(get_consumer_path(url));
            client.create_temporary_data(get_consumer_path(url),url_str);
        }
        super.subscribe(url);
    }

    @Override
    public void un_subscribe(URL url) {
        client.deleteNode(get_consumer_path(url));
        super.un_subscribe(url);
    }

    //节点数据更新触发,这里是权重
    public void watch_node_data_change(String new_server_node_path) {
        client.watch_nodedata(new_server_node_path,new Watcher() {

            @Override
            public void process(WatchedEvent watchedEvent) {
                String path = watchedEvent.getPath();
                String node_data = client.get_nodedata(path);
                node_data=node_data.replace(";","/");

                //System.out.println("zookeeper register 92,build有可能出错，要注意");
                provider_node_info provider_node_info = URL.build_url_from_urlstr(node_data);
                rpc_event event = new rpc_node_change_event(provider_node_info);
                Rpc_Listener_Loader.send_event(event);
                watch_node_data_change(new_server_node_path);
            }
        });
    }

    public void watch_child_nodedata(String new_server_node_path){
        client.watch_child_nodedata(new_server_node_path,new Watcher() {
            @Override
            public void process(WatchedEvent watchedEvent) { //有watchevent事件从zookeeper传来便触发
                System.out.println(watchedEvent);
                String path = watchedEvent.getPath();

                List<String> children_data_list = client.get_children_data(path);
                url_change_wrapper url_change_wrapper = new url_change_wrapper();
                url_change_wrapper.set_url_providers(children_data_list);
                url_change_wrapper.set_service_name(path.split("/")[2]);

                //自定义的一套事件监听组件
                rpc_event event = new rpc_update_event(url_change_wrapper);
                Rpc_Listener_Loader.send_event(event);
                //收到回调之后再注册一次监听，这样能保证一直都收到消息，本质上应该是因为zookeeper的watcher是一次性的
                watch_child_nodedata(new_server_node_path);
            }
        });
    }

    @Override
    //在客户端和服务提供端建立连接的时候，会触发这个函数，这个函数的内部需要订阅每个Provider目录下节点的变化信息，以及Provider目录下每个子节点自身的数据变动情况。
    public void after_subscribe(URL url) {
        //监听是否有新的服务注册
        System.out.println("after subscrbie:");
        String service_path = url.get_param().get("service_name");
        String newServerNodePath = root + "/" +  service_path;

        watch_child_nodedata(newServerNodePath);

        String provider_ip_json = url.get_param().get("provider_ips");
        List<String> provider_ips = JSON.parseObject(provider_ip_json,List.class);
        for (String provider_ip : provider_ips){
            this.watch_node_data_change(root+"/"+service_path+"/"+provider_ip);
        }
    }

    @Override
    public void before_subscribe(URL url) {

    }

    @Override
    public List<String> get_provider_ip(String service_name) {
        List<String> nodeDataList = this.client.get_children_data(root + "/" + service_name + "/provider");
        return nodeDataList;
    }

    //service name - ip - info的对应关系
    @Override
    public Map<String, String> get_service_nodeinfo_map(String service_name) {
        List<String> nodeDataList = this.client.get_children_data(root + "/" + service_name + "/provider");

        Map<String, String> res = new HashMap<>();
        for (String host_ip : nodeDataList){
            String child_data = client.get_nodedata(root + "/" + service_name + "/provider/" + host_ip);
            res.put(host_ip,child_data);
        }

        return res;
    }

    public static void main(String[] args) throws InterruptedException {
        Zookeeper_Register zookeeper_register = new Zookeeper_Register("localhost:2181");
        List<String> url = zookeeper_register.get_provider_ip(data_service.class.getName());
        System.out.println(url);
        Thread.sleep(200000);
    }
}
