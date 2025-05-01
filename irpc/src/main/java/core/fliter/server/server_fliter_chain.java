package core.fliter.server;

import core.fliter.server_fliter;
import core.rpc.RPC_invocation;

import java.util.ArrayList;
import java.util.List;

//将这些过滤器串起来，并且依次执行
public class server_fliter_chain {
    List<server_fliter> server_fliters = new ArrayList<>();

    public void add_server_fliter(server_fliter server_fliter){
        server_fliters.add(server_fliter);
    }

    public void do_fliter(RPC_invocation invocation){
        for(server_fliter server_fliter : server_fliters){
            server_fliter.do_fliter(invocation);
        }
    }
}
