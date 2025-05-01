package irpc.client;

import core.proxy.proxy_factory;

public class RPC_reference {

    public proxy_factory proxy_factory;

    public RPC_reference(proxy_factory proxy_factory) {
        this.proxy_factory = proxy_factory;
    }

    public <T> T getProxy(RPC_reference_wrapper<T> reference_wrapper) throws Throwable {
        return proxy_factory.getProxy(reference_wrapper);
    }
}
