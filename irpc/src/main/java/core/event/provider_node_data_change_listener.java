package core.event;
import core.register.URL;
import core.register.zookeeper.provider_node_info;
import core.router.Router;
import core.rpc.ChannelFuture_wrapper;
import static core.cache.client_cache.CONNECT_CHANNEL_MAP;
import static core.cache.client_cache.ROUTER;

import java.util.List;

public class provider_node_data_change_listener implements  rpc_listener<rpc_node_change_event>{
    @Override
    public void callback(Object t) {
        provider_node_info node_info = (provider_node_info) t;
        List<ChannelFuture_wrapper> wrappers =  CONNECT_CHANNEL_MAP.get(node_info.get_service_name());

        for(ChannelFuture_wrapper wrapper : wrappers){
            String address = wrapper.getHost()+":"+wrapper.getPort();
            if(address.equals(node_info.get_address())){
                //修改权重
                wrapper.set_weight(node_info.get_weight());

                URL url = new URL();
                url.set_service_name(node_info.get_service_name());
                ROUTER.update_weight(url); //
                break;
            }
        }
    }
}
