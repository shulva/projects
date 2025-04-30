package core.event;

public class rpc_node_change_event implements rpc_event {

    private Object data;

    public rpc_node_change_event(Object data) {
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
