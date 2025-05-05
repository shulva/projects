package irpc.server;

import core.fliter.client.client_fliter_chain;
import core.fliter.client_fliter;
import core.fliter.server_fliter;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;

import core.serialize.*;
import core.config.property_bootstrap;
import core.register.URL;
import core.register.Register_Service;
import core.rpc.RPC_decoder;
import core.rpc.RPC_encoder;
import core.config.Server_Config;
import core.rpc.common_utils;
import core.fliter.server.*;
import core.register.zookeeper.Zookeeper_Register;
import core.fliter.server.Server_Before_Limit_Fliter;
import core.fliter.server.Server_After_Limit_Fliter;

import java.io.IOException;
import java.util.LinkedHashMap;

import static core.cache.client_cache.EXTENSION_LOADER;
import static core.cache.server_cache.*;
import static core.spi.Extension_Loader.extension_loader_class_cache;

public class netty_server {

    private static EventLoopGroup boss_group =null;
    private static EventLoopGroup worker_group =null;
    private Server_Config server_config;
    private Register_Service register_service;

    public Server_Config get_config(){
        return server_config;
    }

    public void set_config(Server_Config server_config){
        this.server_config = server_config;
    }

    public void init_server_config() throws IOException, ClassNotFoundException, InstantiationException, IllegalAccessException {
        Server_Config server_config = property_bootstrap.load_server_config_from_local() ;//从配置文件中读取
        this.set_config(server_config);
        SERVER_CONFIG = server_config;

        SERVER_CHANNEL_DISPATCHER.init(SERVER_CONFIG.getServerQueueSize(),SERVER_CONFIG.getServerBizThreadNums());

        //serialize在server_handler中发挥作用
        String serialize_config = server_config.get_Serialize();
        EXTENSION_LOADER.load_extension(Serialize_Factory.class);//使用spi加载
        LinkedHashMap<String, Class> serialize_map = extension_loader_class_cache.get(Serialize_Factory.class.getName());
        Class serialize_factory_class = serialize_map.get(serialize_config);
        if(serialize_factory_class==null){
            throw new RuntimeException("no match serialize type for" + serialize_config);
        }

        SERVER_SERIALIZE_FACTORY = (Serialize_Factory)serialize_factory_class.newInstance();

        /* 无需硬编码
        switch (serialize){
            case "fastJson":
                SERVER_SERIALIZE_FACTORY = new FastJson_Serialize_Facotry();
                break;
            case "jdk":
                SERVER_SERIALIZE_FACTORY = new JDK_Serialize_Factory();
                break;
            case "kryo":
                SERVER_SERIALIZE_FACTORY = new Kryo_Serialize_Factory();
                break;
            case "hessian":
                SERVER_SERIALIZE_FACTORY = new Hessian_Serialize_Factory();
                break;
            default:
                throw new RuntimeException("server no serialize for " + serialize);
        }

         */

        //-------------------责任链
        server_fliter_chain after_chain = new server_fliter_chain();
        server_fliter_chain before_chain = new server_fliter_chain();

        EXTENSION_LOADER.load_extension(server_fliter.class);//使用spi加载
        LinkedHashMap<String, Class> server_fliter_map = extension_loader_class_cache.get(server_fliter.class.getName());

        for (String class_name : server_fliter_map.keySet()) {
            Class server_fliter_class = server_fliter_map.get(class_name);
            if(server_fliter_class==null){
                throw new RuntimeException("no match fliter type for" + class_name);
            }
            SPI spi = (SPI)server_fliter_class.getDeclaredAnnotation(SPI.class);

            if (spi != null && "before".equals(spi.value())) {
                before_chain.add_server_fliter((server_fliter) server_fliter_class.newInstance());
            } else if (spi != null && "after".equals(spi.value())) {
                after_chain.add_server_fliter((server_fliter) server_fliter_class.newInstance());
            }
        }

        /*
        chain.add_server_fliter(new Server_Token_Fliter());
        chain.add_server_fliter(new Server_Log_Fliter());
         */

        SERVER_BEFORE_FLITER_CHAIN = before_chain;
        SERVER_AFTER_FLITER_CHAIN = after_chain;
    }

    public void start() throws InterruptedException {
        boss_group = new NioEventLoopGroup(); //boss
        worker_group = new NioEventLoopGroup();
        ServerBootstrap bootstrap = new ServerBootstrap();
        bootstrap.group(boss_group, worker_group);
        bootstrap.channel(NioServerSocketChannel.class);

        bootstrap.option(ChannelOption.TCP_NODELAY, true);
        bootstrap.option(ChannelOption.SO_BACKLOG, 1024);
        bootstrap.option(ChannelOption.SO_SNDBUF, 16 * 1024)
                .option(ChannelOption.SO_RCVBUF, 16 * 1024)
                .option(ChannelOption.SO_KEEPALIVE, true);

        bootstrap.handler(new Max_Connect_Handler(server_config.getMax_connections())); //限制连接数量

        bootstrap.childHandler(new ChannelInitializer<SocketChannel>() {
            @Override
            protected void initChannel(SocketChannel ch) throws Exception {
                ch.pipeline().addLast(new RPC_encoder());
                ch.pipeline().addLast(new RPC_decoder());
                ch.pipeline().addLast(new server_handler());
            }
        });

        //开启监听服务
        SERVER_CHANNEL_DISPATCHER.start_data_consume();
        this.export_url();
        bootstrap.bind(server_config.getPort()).sync();

    }

    //暴露服务信息
    public void register_service(Service_Wrapper service_wrapper){
        Object service = service_wrapper.get_service_obj();//还是原来的service类型，只是多了一层wrapper

        if(service.getClass().getInterfaces().length==0){
            throw new RuntimeException("service need interface");
        }

        Class [] interfaces = service.getClass().getInterfaces();
        if(interfaces.length>1){
            throw new RuntimeException("service need only one interface");
        }

        if(register_service==null)
            register_service = new Zookeeper_Register(server_config.get_register_address());

        Class interfaceClass = interfaces[0];//服务名,默认选择该对象的第一个实现的接口
        PROVIDER_MAP.put(interfaceClass.getName(), service); // 在此注册服务

        //引入zookeeper之后
        URL url = new URL();
        url.set_service_name(interfaceClass.getName());
        url.set_app_name(server_config.get_application_name());
        url.add_param("host",common_utils.get_ip_address());
        url.add_param("port",String.valueOf(server_config.getPort()));
        url.add_param("group",String.valueOf(service_wrapper.get_group()));
        url.add_param("limit",String.valueOf(service_wrapper.get_limit()));

        PROVIDER_URL_SET.add(url); // 与export_url中的循环联动
        //限制单个服务请求连接的数量
        SERVER_SERVICE_SEMAPHORE_MAP.put(interfaceClass.getName(),new Server_Semaphore_Wrapper(service_wrapper.get_limit()));

        if(!common_utils.isEmpty(service_wrapper.get_service_token()))
            PROVIDER_SERVICE_WRAPPER_MAP.put(interfaceClass.getName(), service_wrapper);

    }

    //将服务器这边的服务都暴露到注册中心，方便客户端调用
    public void export_url(){
        Thread thread = new Thread(new Runnable() {

            @Override
            public void run() {
                try{
                    Thread.sleep(2000);
                }
                catch (InterruptedException e){
                    e.printStackTrace();
                }

                for(URL url : PROVIDER_URL_SET){
                    register_service.register(url); //完成在zookeeper中的注册
                    System.out.println(url);
                }
            }
        });
        thread.start();
    }

    public static void main(String[] args) throws InterruptedException, IOException, ClassNotFoundException, InstantiationException, IllegalAccessException {
        netty_server server = new netty_server();
        server.init_server_config();
        /* 被init代替
        Server_Config config = new Server_Config();
        config.setPort(9090);
        server.set_config(config);
        */

        Service_Wrapper data = new Service_Wrapper(new data_service_impl(),"dev"); //通过zookeeper注册
        data.set_service_token("token-a");
        data.set_limit(2);

        Service_Wrapper user = new Service_Wrapper(new user_service_impl(),"test");
        user.set_service_token("token-b");
        user.set_limit(2);

        server.register_service(user);
        server.register_service(data);

        server.start();
    }
}
