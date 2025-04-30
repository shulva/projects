package core.router;

import core.register.URL;
import core.rpc.ChannelFuture_wrapper;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import static core.cache.client_cache.CONNECT_CHANNEL_MAP;
import static core.cache.client_cache.CHANNEL_FUTURE_POLLING_REF;
import static core.cache.client_cache.SERVICE_ROUTER_MAP;

public class Rotate_Router implements Router{
    @Override
    public void refresh_router_array(Selector selector) {
        List<ChannelFuture_wrapper> wrappers = CONNECT_CHANNEL_MAP.get(selector.get_provider_service_name());
        ChannelFuture_wrapper[] arr = new ChannelFuture_wrapper[wrappers.size()];

        for (int i = 0; i < wrappers.size(); i++) {
            arr[i] = wrappers.get(i);
        }

        SERVICE_ROUTER_MAP.put(selector.get_provider_service_name(), arr);
    }

    //随机路由层内部对外暴露的核心方法是select函数，每次外界调用服务的时候都是通过这个函数去获取下一次调用的provider信息
    @Override
    public ChannelFuture_wrapper select(Selector selector) {
        return CHANNEL_FUTURE_POLLING_REF.get_ChannelFuture_wrapper(selector.get_provider_service_name());
    }

    @Override
    public void update_weight(URL url) {

    }
}
