package core.register.zookeeper;

import core.router.Router;
import core.router.Selector;
import core.rpc.ChannelFuture_wrapper;
import io.netty.bootstrap.Bootstrap;
import io.netty.channel.ChannelFuture;
import core.rpc.common_utils;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import static core.cache.client_cache.*;

//将对于netty连接的管理操作统一封装在了ConnectionHandler类中
//当注册中心的节点新增或者移除或者权重变化的时候，这个类主要负责对内存中的url做变更
public class Connection_Handler {

    /**
     * 核心的连接处理器
     * 专门用于负责和服务端构建连接通信
     */
    private static Bootstrap bootstrap;

    public static void set_bootstrap(Bootstrap bootstrap) {
        Connection_Handler.bootstrap = bootstrap;
    }

    public static ChannelFuture create_ChannelFuture(String ip, Integer port) throws InterruptedException {
        ChannelFuture channelFuture = bootstrap.connect(ip, port).sync();
        return channelFuture;
    }

    //构建单个连接通道 元操作，既要处理连接，还要统一将连接进行内存存储管理
    public static void connect(String service_name, String ip) throws InterruptedException {
        if(bootstrap==null){
            throw new RuntimeException("bootstrap is null");
        }

        ////格式错误类型的信息
        if(!ip.contains(":")){
            System.out.println("ip:"+ip);
            return ;
        }

        String []address = ip.split(":");

        String host = address[0]; //开vpn时不行
        Integer port = Integer.parseInt(address[1]);
        System.out.println(host+":"+port);

        ChannelFuture channelFuture = bootstrap.connect(host,port).sync();
        ChannelFuture_wrapper channelFuture_wrapper = new ChannelFuture_wrapper();
        channelFuture_wrapper.set_ChannelFuture(channelFuture);
        channelFuture_wrapper.setPort(port);
        channelFuture_wrapper.setHost(host);

        SERVER_ADDRESS_SET.add(ip);
        List<ChannelFuture_wrapper> wrappers = CONNECT_CHANNEL_MAP.get(service_name);
        if(common_utils.isEmptyList(wrappers)){
            wrappers = new ArrayList<>();
        }
        wrappers.add(channelFuture_wrapper);
        CONNECT_CHANNEL_MAP.put(service_name,wrappers);
        //--------------------路由层

        Selector selector = new Selector();
        selector.set_provider_service_name(service_name);
        ROUTER.refresh_router_array(selector);
    }

    public static void disconnect(String service_name,String ip) throws InterruptedException {
        SERVER_ADDRESS_SET.remove(ip);
        List<ChannelFuture_wrapper> wrappers = CONNECT_CHANNEL_MAP.get(service_name);

        if (common_utils.isNotEmptyList(wrappers)) {
            Iterator<ChannelFuture_wrapper> iterator = wrappers.iterator();
            while (iterator.hasNext()) {
                ChannelFuture_wrapper channelFuture_wrapper = iterator.next();
                if (ip.equals(channelFuture_wrapper.getHost() + ":" + channelFuture_wrapper.getPort())) { //ip相等的channel移除
                    iterator.remove();
                }
            }
        }
    }

    //从注册中心获取服务的地址信息，并且缓存在一个MAP集合中。
    //从缓存的MAP集合中根据服务名称查询到对应的通道List集合。
    //从List集合中随机筛选一个Channel通道，发送数据包。
    public static ChannelFuture get_ChannelFuture(String service_name) throws InterruptedException {
        List<ChannelFuture_wrapper> wrappers = CONNECT_CHANNEL_MAP.get(service_name);
        if(common_utils.isEmptyList(wrappers)){
            throw new RuntimeException("no provider for:"+service_name);
        }

        //-----------------------------------路由层
        Selector selector = new Selector();
        selector.set_provider_service_name(service_name);
        ChannelFuture ch = ROUTER.select(selector).get_ChannelFuture();

        //ChannelFuture ch = wrappers.get(new Random().nextInt(wrappers.size())).get_ChannelFuture();
        return ch;
    }


}
