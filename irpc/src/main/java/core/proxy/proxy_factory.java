package core.proxy;

public interface proxy_factory {
    <T> T getProxy(final Class classs) throws Throwable;
}
