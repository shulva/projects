package irpc.client;

import com.alibaba.fastjson.JSON;

import core.fliter.client_fliter;
import core.router.Random_Router;
import core.router.Rotate_Router;
import core.router.Router;
import core.serialize.*;
import core.spi.Extension_Loader;
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
import core.proxy.JDK_proxy_factory;
import core.register.zookeeper.Abstract_Register;
import core.rpc.RPC_decoder;
import core.rpc.RPC_encoder;
import core.rpc.RPC_invocation;
import core.rpc.RPC_protocol;
import core.event.Rpc_Listener_Loader;
import core.config.property_bootstrap;
import core.rpc.common_utils;
import core.fliter.client.*;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import rpc_interface.data_service;
import rpc_interface.user_service;
import static core.cache.client_cache.*;
import static core.cache.server_cache.SERVER_SERIALIZE_FACTORY;
import static core.spi.Extension_Loader.extension_loader_class_cache;

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

    public void init_client_config() throws IOException, ClassNotFoundException, InstantiationException, IllegalAccessException {
        Client_Config client_config =  property_bootstrap.load_client_config_from_local(); //配置初始化
        this.setClient_config(client_config);
        CLIENT_CONFIG = client_config;

        String router = client_config.get_router_strategy();
        EXTENSION_LOADER.load_extension(Router.class);
        LinkedHashMap<String, Class> router_map = extension_loader_class_cache.get(Router.class.getName());
        Class router_class =router_map.get(router);
        if(router_class==null){
            throw new RuntimeException("no match router strategy" + router);
        }
        ROUTER =(Router)router_class.newInstance();

        /*
        if("random".equals(router)){
            ROUTER = new Random_Router();
        }
        else if("rotate".equals(router)){
            ROUTER = new Rotate_Router();
        }
        */


        //serialize在client_handler和发送线程async_send_job中发挥作用

        String serialize_config = client_config.get_Serialize();
        EXTENSION_LOADER.load_extension(Serialize_Factory.class);//使用spi加载
        LinkedHashMap<String, Class> serialize_map = extension_loader_class_cache.get(Serialize_Factory.class.getName());
        Class serialize_factory_class = serialize_map.get(serialize_config);
        if(serialize_factory_class==null){
            throw new RuntimeException("no match serialize type for" + serialize_config);
        }

        CLIENT_SERIALIZE_FACTORY = (Serialize_Factory)serialize_factory_class.newInstance();
        /*
        switch (serialize){
            case "fastJson":
                CLIENT_SERIALIZE_FACTORY = new FastJson_Serialize_Facotry();
                break;
            case "jdk":
                CLIENT_SERIALIZE_FACTORY = new JDK_Serialize_Factory();
                break;
            case "kryo":
                CLIENT_SERIALIZE_FACTORY = new Kryo_Serialize_Factory();
                break;
            case "hessian":
                CLIENT_SERIALIZE_FACTORY = new Hessian_Serialize_Factory();
                break;
            default:
                throw new RuntimeException("client no serialize for " + serialize);

        }

         */

        EXTENSION_LOADER.load_extension(client_fliter.class);//使用spi加载
        client_fliter_chain chain = new client_fliter_chain();
        LinkedHashMap<String, Class> client_fliter_map = extension_loader_class_cache.get(client_fliter.class.getName());
        for (String class_name : client_fliter_map.keySet()) {
            Class client_fliter_class = client_fliter_map.get(class_name);
            if(client_fliter_class==null){
                throw new RuntimeException("no match fliter type for" + class_name);
            }
            chain.add_client_fliter((client_fliter) client_fliter_class.newInstance());
        }
        /*
        chain.add_client_fliter(new Direct_Invoke_Filter());
        chain.add_client_fliter(new Group_Fliter());
        chain.add_client_fliter(new Client_Log_Fliter());
         */
        CLIENT_FLITER_CHAIN = chain;

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

        RPC_reference reference = null;
        if ("javassist".equals(client_config.get_proxy_type())) {
            //javaassist proxy , 未实现
        } else {
            reference = new RPC_reference(new JDK_proxy_factory()); //默认JDK
        }
        return reference;
    }

    //启动服务前需要先订阅服务
    public void subscribe_service(Class service){
        if(abstract_register == null){
            abstract_register = new Zookeeper_Register(client_config.get_register_address());
        }

        URL url = new URL();
        url.set_app_name(client_config.get_application_name());
        url.set_service_name(service.getName());
        url.add_param("host",common_utils.get_ip_address());
        Map<String, String> result = abstract_register.get_service_nodeinfo_map(service.getName()); //通过name获取对应的host ip ->provider_node_info 关系
        URL_MAP.put(service.getName(),result);
        abstract_register.subscribe(url); //订阅服务
    }

    //  与provider建立连接
    public void connect_server(){
        for(URL provider_url: SUBSCRIBE_SERVICE_LIST){
            List<String> provider_ips = abstract_register.get_provider_ip(provider_url.get_service_name());
            for(String provider_ip:provider_ips){
                try{
                    Connection_Handler.connect(provider_url.get_service_name(),provider_ip);
                } catch (InterruptedException e) {
                    logger.error("[doConnectServer] connect fail ", e);
                }
            }

            URL url = new URL();
            url.add_param("service_name",provider_url.get_service_name()+"/provider"); //感觉其实不应该叫service_name,service_path比较好
            url.add_param("provider_ips",JSON.toJSONString(provider_ips));
            System.out.println(provider_url.get_service_name()+"/provider");
            //客户端在此新增一个订阅的功能
            abstract_register.after_subscribe(url);
        }
    }

    //发送线程，专门将数据包发送给服务端
    public void start_client() {
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
                    RPC_protocol rpc_protocol = new RPC_protocol(CLIENT_SERIALIZE_FACTORY.serialize(data));
                    //发送给服务端
                    //System.out.println(JSON.toJSONString(rpc_protocol));
                    //客户端启动之后并订阅所有服务之后，就会建立与对应服务器之间的连接（channel）(connect函数中），然后在远程调用的时候再选取连接进行调用
                    ChannelFuture channelFuture = Connection_Handler.get_ChannelFuture(data);
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
            client.init_client_config();
            RPC_reference reference = client.init_application();

            RPC_reference_wrapper<data_service> data_reference_wrapper = new RPC_reference_wrapper<>();
            data_reference_wrapper.set_aim_class(data_service.class);
            data_reference_wrapper.set_group("dev");
            data_reference_wrapper.set_service_token("token-a");
            data_reference_wrapper.set_async(false);//需要接受返回
            data_reference_wrapper.setTimeOut(3000);
            data_reference_wrapper.setRetry(0); //超时重传次数

            data_service service = reference.getProxy(data_reference_wrapper);//获取代理对象，设置缓存信息,用订阅时调用
            client.subscribe_service(data_service.class);//订阅某个服务，添加到本地缓存subscribe_service_list


            RPC_reference_wrapper<user_service> user_reference_wrapper = new RPC_reference_wrapper<>();
            user_reference_wrapper.set_aim_class(user_service.class);
            user_reference_wrapper.set_group("test");
            user_reference_wrapper.set_service_token("token-b");
            user_reference_wrapper.set_async(true);//不需要接受返回
            user_reference_wrapper.setTimeOut(3000);
            user_reference_wrapper.setRetry(0); //超时重传次数

            user_service service2 = reference.getProxy(user_reference_wrapper);
            client.subscribe_service(user_service.class);

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
                    service2.test();
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
