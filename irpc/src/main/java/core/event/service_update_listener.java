package core.event;

import io.netty.channel.ChannelFuture;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import core.register.zookeeper.url_change_wrapper;
import core.rpc.ChannelFuture_wrapper;
import core.rpc.common_utils;
import core.register.zookeeper.Connection_Handler;
import static core.cache.client_cache.CONNECT_CHANNEL_MAP;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

//zk的服务提供者节点发生了变更，发送事件通知了变更
//这个时候客户端需要更新本地的一个目标服务列表，避免向无用的服务发送请求。
public class service_update_listener implements rpc_listener<rpc_update_event> {
    private static final Logger logger = LoggerFactory.getLogger(service_update_listener.class);

    @Override
    public void callback(Object t) {
        //获取到字节点的数据信息
        url_change_wrapper wrapper = (url_change_wrapper) t;
        List<ChannelFuture_wrapper> channel_future_wrapper_list = CONNECT_CHANNEL_MAP.get(wrapper.get_service_name());

        if(common_utils.isEmptyList(channel_future_wrapper_list)){
            logger.error("[ServiceUpdateListener] channelFutureWrappers is empty");
            return;
        }
        else{

            List<String> match_provider_url =  wrapper.get_url_providers_list();
            List<ChannelFuture_wrapper> final_channel_future_wrapper = new ArrayList<>();
            Set<String> final_url = new HashSet<>();

            for(ChannelFuture_wrapper channel_future : channel_future_wrapper_list){
                String old_server_address = channel_future.getHost()+":"+channel_future.getPort();

                if(!match_provider_url.contains(old_server_address)){//老的url已被移出
                    continue;
                }
                else{ //只添加老的
                   final_channel_future_wrapper.add(channel_future);
                   final_url.add(old_server_address);
                }
            }

            //此时老的url已经被移除了，开始检查是否有新的url
            //ChannelFutureWrapper其实是一个自定义的包装类，将netty建立好的ChannelFuture做了一些封装

            List<ChannelFuture_wrapper> new_wrapper = new ArrayList<>();
            for(String new_provider_url:match_provider_url){
                if(!final_url.contains(new_provider_url)){ // 添加新的
                    ChannelFuture_wrapper new_channel_future = new ChannelFuture_wrapper();

                    String host = new_provider_url.strip().split(":")[0];
                    Integer port = Integer.valueOf(new_provider_url.strip().split(":")[1]);
                    //System.out.println(host+port);

                    new_channel_future.setHost(host);
                    new_channel_future.setPort(port);
                    ChannelFuture ch = null;

                    try{
                        ch = Connection_Handler.create_ChannelFuture(host,port);
                        new_channel_future.set_ChannelFuture(ch);

                        new_wrapper.add(new_channel_future);
                        final_url.add(new_provider_url);

                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                }
            }
            final_channel_future_wrapper.addAll(new_wrapper);

            //最终更新服务在这里
            CONNECT_CHANNEL_MAP.put(wrapper.get_service_name(), final_channel_future_wrapper);
        }

    }
}
