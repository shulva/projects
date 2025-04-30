package core.event;

//监听器接口
public interface rpc_listener<T> { //<T>用来包含触发监听器的事件，从而在监听时可以让事件和监听器对应,具体见rpc_listener_loader中的senddata
    void callback(Object t);
}
