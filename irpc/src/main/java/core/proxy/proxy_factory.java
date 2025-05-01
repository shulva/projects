package core.proxy;

import irpc.client.RPC_reference_wrapper;

public interface proxy_factory {
    <T> T getProxy(RPC_reference_wrapper reference_wrapper) throws Throwable;
}
