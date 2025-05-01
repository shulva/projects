package core.fliter.server;

import core.fliter.server_fliter;

import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;
import static core.cache.client_cache.CLIENT_CONFIG;

//服务器端日志记录
public class Server_Log_Fliter implements server_fliter{

    private static final Logger logger = LoggerFactory.getLogger(Server_Log_Fliter.class);

    @Override
    public void do_fliter(RPC_invocation invocation) {
        logger.info(invocation.get_attachments().get("client_app_name") + "invoke ->"+invocation.get_targetServiceName()+"#"+invocation.get_targetMethod())  ;
    }
}
