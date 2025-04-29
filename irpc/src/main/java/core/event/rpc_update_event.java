package core.event;

//继承自抽象事件，定义一个节点更新事件
//当zookeeper的某个节点发生数据变动的时候，就会发送一个update事件，然后由对应的监听器去捕获这些数据并做处理。
public class rpc_update_event implements rpc_event {
    private Object data;

    public rpc_update_event(Object data) {
        this.data = data;
    }

    @Override
    public Object get_data() {
        return data;
    }

    @Override
    public rpc_event set_data(Object data) {
        this.data = data;
        return this;
    }

}
