package consumer;

import irpc.client.Connection_Handler;
import irpc.client.RPC_reference;
import irpc.client.RPC_reference_wrapper;
import irpc.client.netty_client;
import rpc_interface.data_service;
import rpc_interface.user_service;

public class Consumer_Demo {
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
