package provider;

import irpc.server.Service_Wrapper;
import irpc.server.data_service_impl;
import irpc.server.netty_server;
import irpc.server.user_service_impl;

import java.io.IOException;

public class Provider_Demo {
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
