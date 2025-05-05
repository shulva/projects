package core.fliter.server;

import core.fliter.server_fliter;
import core.rpc.RPC_invocation;
import static core.cache.server_cache.SERVER_SERVICE_SEMAPHORE_MAP;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.util.concurrent.Semaphore;


@SPI(value = "before")
public class Server_Before_Limit_Fliter implements server_fliter {

    private static final Logger LOGGER = LoggerFactory.getLogger(Server_Before_Limit_Fliter.class);

    @Override
    public void do_fliter(RPC_invocation invocation) {
        String serviceName = invocation.get_targetServiceName();
        Server_Semaphore_Wrapper semaphore_wrapper = SERVER_SERVICE_SEMAPHORE_MAP.get(serviceName);
        //从缓存中提取semaphore对象
        Semaphore semaphore = semaphore_wrapper.getSemaphore();
        boolean tryResult = semaphore.tryAcquire();
        if (!tryResult) {
            LOGGER.error("[ServerServiceBeforeLimitFilterImpl] {}'s max request is {},reject now", invocation.get_targetServiceName(), semaphore_wrapper.getMaxNums());

            RuntimeException RpcException = new RuntimeException("request rejected by the server,limited request");
            invocation.set_exception(RpcException);
        }
    }
}
