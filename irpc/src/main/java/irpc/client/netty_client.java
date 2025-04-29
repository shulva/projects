package irpc.client;

import com.alibaba.fastjson.JSON;

import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import io.netty.bootstrap.Bootstrap;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import core.register.URL;
import core.register.zookeeper.Zookeeper_Register;
import core.config.Client_Config;
import rpc_interface.data_service;
import core.proxy.JDK_proxy_factory;
import core.register.zookeeper.Abstract_Register;
import core.rpc.RPC_decoder;
import core.rpc.RPC_encoder;
import core.rpc.RPC_invocation;
import core.rpc.RPC_protocol;
import core.event.Rpc_Listener_Loader;
import static core.cache.client_cache.SEND_QUEUE;
import static core.cache.client_cache.subscribe_service_list;
import core.register.zookeeper.Connection_Handler;
import core.config.property_bootstrap;
import core.rpc.common_utils;

import java.util.List;

public class netty_client {

    //public static EventLoopGroup group = new NioEventLoopGroup(); 会引发静态初始化失败

    private Logger logger = LoggerFactory.getLogger(netty_client.class);

    private Client_Config client_config;

    private Bootstrap bootstrap = new Bootstrap();

    private Abstract_Register abstract_register;

    private Rpc_Listener_Loader rpc_listener_loader;

    public Client_Config getClient_config() {
        return client_config;
    }

    public void setClient_config(Client_Config client_config) {
        this.client_config = client_config;
    }

    public Bootstrap get_bootstrap() {
        return bootstrap;
    }

    public RPC_reference init_application() throws InterruptedException {
        EventLoopGroup client_group = new NioEventLoopGroup();
        bootstrap.group(client_group);
        bootstrap.channel(NioSocketChannel.class);
        bootstrap.handler(new ChannelInitializer<SocketChannel>() {
           @Override
           protected void initChannel(SocketChannel ch) throws Exception {
               ch.pipeline().addLast(new RPC_encoder());
               ch.pipeline().addLast(new RPC_decoder());
               ch.pipeline().addLast(new client_handler());
           }
        });


        logger.info("============ 服务开始 ====================");
        /*
        ChannelFuture channelFuture = bootstrap.connect(client_config.get_server_addr(),client_config.get_port()).sync();
        this.start_client(channelFuture);

        if(channelFuture.isSuccess()){
            System.out.println("connect success");
        }
        else
            System.out.println("connect failed");

         */
        rpc_listener_loader = new Rpc_Listener_Loader();
        rpc_listener_loader.init();
        this.client_config = property_bootstrap.load_client_config_from_local();
        RPC_reference reference = null;
        if ("javassist".equals(client_config.get_proxy_type())) {
            //javaassist proxy , 未实现
        } else {
            reference = new RPC_reference(new JDK_proxy_factory()); //默认JDK
        }
        return reference;
    }

    public void subscribe_service(Class service){
        if(abstract_register == null){
            abstract_register = new Zookeeper_Register(client_config.get_register_address());
        }

        URL url = new URL();
        url.set_app_name(client_config.get_application_name());
        url.set_service_name(service.getName());
        url.add_param("host",common_utils.get_ip_address());
        abstract_register.subscribe(url); //订阅服务
    }

    //  与provider建立连接
    public void connect_server(){
        for(String provider_service_name:subscribe_service_list){
            List<String> provider_ips = abstract_register.get_provider_ip(provider_service_name);
            for(String provider_ip:provider_ips){
                try{
                    Connection_Handler.connect(provider_service_name,provider_ip);
                } catch (InterruptedException e) {
                    logger.error("[doConnectServer] connect fail ", e);
                }
            }

            URL url = new URL();
            url.set_service_name(provider_service_name);
            //客户端在此新增一个订阅的功能
            abstract_register.after_subscribe(url);
        }
    }

    //发送线程，专门将数据包发送给服务端
    private void start_client() {
        Thread async_send_thread = new Thread(new async_send_job());
        async_send_thread.start();
    }

    class async_send_job implements Runnable {

        // ChannelFuture交给connection handler处理
        //private ChannelFuture channelFuture;

        public async_send_job() {
        }

        @Override
        public void run() {
            while(true){
                try {
                    RPC_invocation data = SEND_QUEUE.take();
                    String json = JSON.toJSONString(data);
                    RPC_protocol rpc_protocol = new RPC_protocol(json.getBytes());
                    //发送给服务端
                    //System.out.println(JSON.toJSONString(rpc_protocol));
                    ChannelFuture channelFuture = Connection_Handler.get_ChannelFuture(data.get_targetServiceName());
                    channelFuture.channel().writeAndFlush(rpc_protocol);

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public static void main(String[] args) throws Throwable {
        try{
            /*
            Client_Config config = new Client_Config();
            config.set_port(9090);
            config.set_server_addr("localhost");
            client.setClient_config(config);
             */

            netty_client client = new netty_client();
            RPC_reference reference = client.init_application();

            data_service service = reference.getProxy(data_service.class);//获取代理对象，设置缓存信息,用订阅时调用
            client.subscribe_service(data_service.class);//订阅某个服务，添加到本地缓存subscribe_service_list
            Connection_Handler.set_bootstrap(client.get_bootstrap());

            //订阅服务，从subscribe_service_list中获取需要订阅的服务信息，添加注册中心的监听
            //根据服务者生产者信息，建立连接ChannelFuture，将其放入Connect_MAP
            client.connect_server();
            //开启异步线程发送请求，通过队列SEND_QUEUE进行通信
            client.start_client();

            for (int i = 0; i < 10; i++) {
                try{
                    //代理层invoke方法，将请求放入SEND_QUEUE队列
                    //异步线程接收到SEND_QUEUE数据，发起netty调用，在invoke方法循环中获取RESP_MAP缓存中的响应数据
                    //在Client_handler中将请求方法和响应数据放入RESP_MAP中
                    String res = service.send_data("hello");
                    System.out.println(res);
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        catch (Exception e){
            System.err.println("启动客户端时出现异常: " + e.getMessage());
        }
    }
}
