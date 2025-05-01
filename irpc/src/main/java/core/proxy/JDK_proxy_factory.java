package core.proxy;

import java.lang.reflect.Proxy;
import irpc.client.RPC_reference_wrapper;


public class JDK_proxy_factory implements proxy_factory {

    @Override
    public <T> T getProxy(RPC_reference_wrapper reference_wrapper) throws Throwable {
        return (T) Proxy.newProxyInstance(reference_wrapper.get_aim_class().getClassLoader(),new Class[]{reference_wrapper.get_aim_class()},new JDK_Client_Invocation_Handler(reference_wrapper));
    }
}
