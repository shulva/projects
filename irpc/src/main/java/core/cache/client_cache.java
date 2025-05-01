package core.cache;
import core.register.URL;
import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;
import core.rpc.ChannelFuture_PollingRef;
import core.router.Router;
import core.serialize.Serialize_Factory;
import core.fliter.client.client_fliter_chain;
import core.config.Client_Config;
import core.spi.Extension_Loader;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class client_cache {
    public static BlockingQueue<RPC_invocation> SEND_QUEUE = new ArrayBlockingQueue<>(100); // send-invocation
    public static Map<String,Object> RESP_MAP = new ConcurrentHashMap<>(); //uuid - invocation

    //------------------------------注册层
    public static List<URL> SUBSCRIBE_SERVICE_LIST = new ArrayList<>();
    public static Map<String,Map<String,String>> URL_MAP = new ConcurrentHashMap<>();
    public static Set<String> SERVER_ADDRESS_SET = new HashSet<>(); //server地址
    //每次进行远程调用的时候都是从这里面去选择服务提供者,是服务名-ChannelFuture wrappers的一对多对应关系
    public static Map<String,List<ChannelFuture_wrapper>> CONNECT_CHANNEL_MAP = new ConcurrentHashMap<>();

    //------------------------------路由层
    public static Map<String, ChannelFuture_wrapper[]> SERVICE_ROUTER_MAP = new ConcurrentHashMap<>();
    public static ChannelFuture_PollingRef CHANNEL_FUTURE_POLLING_REF = new ChannelFuture_PollingRef();
    public static Router ROUTER;
    //------------------------------序列化
    public static Serialize_Factory CLIENT_SERIALIZE_FACTORY;
    //------------------------------责任链处理fliter
    public static client_fliter_chain CLIENT_FLITER_CHAIN;
    public static Client_Config CLIENT_CONFIG;
    //------------------------------SPI
    public static Extension_Loader EXTENSION_LOADER = new Extension_Loader();
}
