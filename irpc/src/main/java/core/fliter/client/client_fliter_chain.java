package core.fliter.client;

import core.fliter.client_fliter;
import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;

import java.util.ArrayList;
import java.util.List;

//将这些过滤器串起来，并且依次执行
public class client_fliter_chain {
    List<client_fliter> client_fliters= new ArrayList<>();

    public void add_client_fliter(client_fliter client_fliter){
        client_fliters.add(client_fliter);
    }

    public void do_fliter(List<ChannelFuture_wrapper> src, RPC_invocation invocation){
        for(client_fliter client_fliter: client_fliters){
            client_fliter.do_fliter(src,invocation);
        }
    }
}
