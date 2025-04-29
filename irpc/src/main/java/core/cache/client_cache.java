package core.cache;
import core.register.URL;
import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class client_cache {
    public static BlockingQueue<RPC_invocation> SEND_QUEUE = new ArrayBlockingQueue<>(100); // send-invocation
    public static Map<String,Object> RESP_MAP = new ConcurrentHashMap<>(); //uuid - invocation

    public static List<String> subscribe_service_list = new ArrayList<>();
    public static Map<String,List<URL>> URL_MAP = new ConcurrentHashMap<>();
    public static Set<String> server_address = new HashSet<>(); //server地址

    //每次进行远程调用的时候都是从这里面去选择服务提供者,是服务名-ChannelFuture wrapper的对应关系
    public static Map<String,List<ChannelFuture_wrapper>> CONNECT_CHANNEL_MAP = new ConcurrentHashMap<>();
}
