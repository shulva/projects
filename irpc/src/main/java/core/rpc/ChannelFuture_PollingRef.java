package core.rpc;

import java.util.concurrent.atomic.AtomicLong;
import static core.cache.client_cache.SERVICE_ROUTER_MAP;

//实现轮询效果，它的本质就是通过取模计算
public class ChannelFuture_PollingRef {
    public AtomicLong refCount = new AtomicLong(0);

    public ChannelFuture_wrapper get_ChannelFuture_wrapper(String serviceName) {
        ChannelFuture_wrapper[] wrappers = SERVICE_ROUTER_MAP.get(serviceName);
        long i = refCount.getAndIncrement();
        int index = (int)(i % wrappers.length);
        return wrappers[index];
    }
}
