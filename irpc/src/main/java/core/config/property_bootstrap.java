package core.config;

import java.io.IOException;

//将properties的配置转换成本地的一个Map结构进行管理
public class property_bootstrap {
    private volatile boolean config_is_ready;

    public static final String SERVER_PORT = "rpc.serverPort";
    public static final String REGISTER_ADDRESS = "rpc.registerAddr";
    public static final String APPLICATION_NAME = "rpc.applicationName";
    public static final String PROXY_TYPE = "rpc.proxyType";
    public static final String ROUTER_TYPE = "rpc.routerStrategy";
    public static final String SERVER_SERIALIZE_TYPE = "rpc.serverSerialize";
    public static final String CLIENT_SERIALIZE_TYPE = "rpc.clientSerialize";
    public static final String SERVER_QUEUE_SIZE = "rpc.serverQueueSize";
    public static final String SERVER_THREAD_NUMS = "rpc.serverThreadNums";
    public static final String MAX_CONNECT = "rpc.maxConnect";

    public static Client_Config load_client_config_from_local() {
        try{
            property_loader.load_configuration();
        } catch (IOException e) {
            throw new RuntimeException("load_client_config_from_local", e);
        }

        Client_Config client_config = new Client_Config();
        client_config.set_application_name(property_loader.get_property_str(APPLICATION_NAME));
        client_config.set_proxy_type(property_loader.get_property_str(PROXY_TYPE));
        client_config.set_register_address(property_loader.get_property_str(REGISTER_ADDRESS));
        client_config.set_router_strategy(property_loader.get_property_str(ROUTER_TYPE));
        client_config.set_Serialize(property_loader.get_property_str(CLIENT_SERIALIZE_TYPE));

        return client_config;
    }

    public static Server_Config load_server_config_from_local() {
        try{
            property_loader.load_configuration();
        } catch (IOException e) {
            throw new RuntimeException("load_client_server_from_local", e);
        }

        Server_Config server_config = new Server_Config();
        server_config.set_application_name(property_loader.get_property_str(APPLICATION_NAME));
        server_config.set_register_address(property_loader.get_property_str(REGISTER_ADDRESS));
        server_config.setPort(property_loader.get_property_integer(SERVER_PORT));
        server_config.set_Serialize(property_loader.get_property_str(SERVER_SERIALIZE_TYPE));
        server_config.setServerQueueSize(Integer.parseInt(property_loader.get_property_str(SERVER_QUEUE_SIZE)));
        server_config.setServerBizThreadNums(Integer.parseInt(property_loader.get_property_str(SERVER_THREAD_NUMS)));
        server_config.setMax_connections(Integer.parseInt(property_loader.get_property_str(MAX_CONNECT)));


        return server_config;
    }
}
