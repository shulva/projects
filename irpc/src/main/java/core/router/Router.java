package core.router;

import core.register.URL;
import core.rpc.ChannelFuture_wrapper;
import io.netty.channel.ChannelFuture;

public interface Router {
    //刷新路由数组
    void refresh_router_array(Selector selector);

    //获取到请求到连接通道
    ChannelFuture_wrapper select(Selector selector);

    //更新权重
    void update_weight(URL url);
}
