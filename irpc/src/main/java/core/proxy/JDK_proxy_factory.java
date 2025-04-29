package core.proxy;

import core.proxy.proxy_factory;
import core.proxy.client_invocation_handler;

import java.lang.reflect.Proxy;


public class JDK_proxy_factory implements proxy_factory {

    @Override
    public <T> T getProxy(Class classs) throws Throwable {
        return (T) Proxy.newProxyInstance(classs.getClassLoader(),new Class[]{classs},new client_invocation_handler(classs));
    }
}
