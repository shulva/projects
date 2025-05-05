package core.fliter.server;

import core.fliter.server_fliter;
import core.rpc.RPC_invocation;
import static core.cache.server_cache.SERVER_SERVICE_SEMAPHORE_MAP;

@SPI(value = "after")
public class Server_After_Limit_Fliter implements server_fliter{


    @Override
    public void do_fliter(RPC_invocation invocation) {

        String serviceName = invocation.get_targetServiceName();
        Server_Semaphore_Wrapper wrapper = SERVER_SERVICE_SEMAPHORE_MAP.get(serviceName);
        wrapper.getSemaphore().release();
    }
}
