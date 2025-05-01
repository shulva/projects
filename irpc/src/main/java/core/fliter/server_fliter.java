package core.fliter;

import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;

public interface server_fliter extends fliter{

    void do_fliter(RPC_invocation invocation);
}
