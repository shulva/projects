package irpc.server;

import core.register.zookeeper.Zookeeper_Register;
import io.netty.bootstrap.ServerBootstrap;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;

import core.serailize.*;
import static core.cache.server_cache.*;
import core.config.property_bootstrap;
import core.register.URL;
import core.register.Register_Service;
import core.rpc.RPC_decoder;
import core.rpc.RPC_encoder;
import core.config.Server_Config;
import core.rpc.common_utils;


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

    public void init_server_config(){ //从本地property读取配置
        Server_Config server_config = property_bootstrap.load_server_config_from_local() ;//从配置文件中读取
        this.set_config(server_config);

        //serialize在server_handler中发挥作用
        String serialize = server_config.get_Serialize();
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
                SERVER_SERIALIZE_FACTORY = new Hessian_Serilaize_Factory();
                break;
            default:
                throw new RuntimeException("server no serialize for " + serialize);
        }
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

        bootstrap.childHandler(new ChannelInitializer<SocketChannel>() {
            @Override
            protected void initChannel(SocketChannel ch) throws Exception {
                System.out.println("初始化providers...");
                ch.pipeline().addLast(new RPC_encoder());
                ch.pipeline().addLast(new RPC_decoder());
                ch.pipeline().addLast(new server_handler());
            }
        });

        this.export_url();
        bootstrap.bind(server_config.getPort()).sync();

    }

    //暴露服务信息
    public void register_service(Object service){
        if(service.getClass().getInterfaces().length==0){
            throw new RuntimeException("service need interface");
        }

        Class [] interfaces = service.getClass().getInterfaces();
        if(interfaces.length>1){
            throw new RuntimeException("service need only one interface");
        }

        if(register_service==null)
            register_service = new Zookeeper_Register(server_config.get_register_address());

        Class interfaceClass = interfaces[0];
        PROVIDER_MAP.put(interfaceClass.getName(), service); // 在此注册服务

        //引入zookeeper之后
        URL url = new URL();
        url.set_service_name(interfaceClass.getName());
        url.set_app_name(server_config.get_application_name());
        url.add_param("host",common_utils.get_ip_address());
        url.add_param("port",String.valueOf(server_config.getPort()));
        System.out.println(common_utils.get_ip_address());

        PROVIDER_URL_SET.add(url);

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

    public static void main(String[] args) throws InterruptedException {
        netty_server server = new netty_server();
        /* 被init代替
        Server_Config config = new Server_Config();
        config.setPort(9090);
        server.set_config(config);
        */

        server.init_server_config();
        server.register_service(new data_service_impl()); //通过zookeeper注册
        server.register_service(new user_service_impl());
        server.start();
    }
}
