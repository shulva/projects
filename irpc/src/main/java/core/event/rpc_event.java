package core.event;

//定义一个抽象的事件，该事件会用于装载需要传递的数据信息
public interface rpc_event {
    Object get_data();
    rpc_event set_data(Object data);
}
