package core.event;

//监听器接口
public interface rpc_listener<T> {
    void callback(Object t);
}
