package core.register;

import core.register.zookeeper.provider_node_info;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;

public class URL {

    private String app_name; //服务应用名
    private String service_name;

    private Map<String, String> params = new HashMap<>();

    public void add_param(String key, String value) {
        this.params.putIfAbsent(key, value);
    }

    public Map<String,String> get_param() {
        return this.params;
    }

    public void set_param(Map<String,String> params) {
        this.params = params;
    }

    public String get_app_name() {
        return app_name;
    }

    public void set_app_name(String app_name) {
        this.app_name = app_name;
    }

    public String get_service_name() {
        return service_name;
    }

    public void set_service_name(String service_name) {
        this.service_name = service_name;
    }

    public static String build_provider_url(URL url) {//将URL转换为写入zk的provider节点下的一段字符串
        String host = url.get_param().get("host");
        String port = url.get_param().get("port");
        return new String((url.get_app_name() + ";" + url.get_service_name() + ";" + host + ":"+port+";" + System.currentTimeMillis()+";100").getBytes(), StandardCharsets.UTF_8);//100是路由权重
    }

    public static String build_consumer_url(URL url) {//将URL转换为写入zk的consumer节点下的一段字符串
        String host = url.get_param().get("host");
        return new String((url.get_app_name() + ";" + url.get_service_name() + ";" + host + ";" + System.currentTimeMillis()).getBytes(), StandardCharsets.UTF_8);
    }

    //拆分逻辑是基于build_provider_url来的
    public static provider_node_info build_url_from_urlstr(String provider_node_str) {
        String[] parts = provider_node_str.split("/");
        provider_node_info info = new provider_node_info();
        info.set_service_name(parts[1]);
        info.set_address(parts[2]);
        info.set_register_time(parts[3]);
        info.set_weight(Integer.valueOf(parts[4]));
        return info;
    }

}
