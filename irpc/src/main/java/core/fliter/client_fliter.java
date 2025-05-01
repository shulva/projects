package core.fliter;

import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;

import java.util.List;

public interface client_fliter extends fliter {

    //执行过滤链
    void do_fliter(List<ChannelFuture_wrapper> src,RPC_invocation invocation);
}
