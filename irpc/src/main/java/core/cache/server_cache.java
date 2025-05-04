package core.cache;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import core.register.URL;
import core.serialize.Serialize_Factory;
import core.fliter.server.server_fliter_chain;
import core.config.Server_Config;
import irpc.server.server_channel_dispatcher;
import irpc.server.Service_Wrapper;
import core.fliter.server.Server_Semaphore_Wrapper;

public class server_cache {
    public static final Map<String, Object> PROVIDER_MAP = new HashMap<>(); //服务名-服务的对应关系
    public static final Set<URL> PROVIDER_URL_SET = new HashSet<>();
    //------------------------------序列化
    public static Serialize_Factory SERVER_SERIALIZE_FACTORY;
    //------------------------------责任链模式
    public static server_fliter_chain SERVER_FLITER_CHAIN;
    public static Server_Config SERVER_CONFIG;
    public static Map<String, Service_Wrapper> PROVIDER_SERVICE_WRAPPER_MAP = new ConcurrentHashMap<>();
    public static server_channel_dispatcher SERVER_CHANNEL_DISPATCHER = new server_channel_dispatcher();
    //------------------------------单个服务请求限流
    public static final Map<String, Server_Semaphore_Wrapper> SERVER_SERVICE_SEMAPHORE_MAP = new ConcurrentHashMap<>(64);

}
