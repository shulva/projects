package core.fliter.client;

import core.fliter.client_fliter;
import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;
import static core.cache.client_cache.CLIENT_CONFIG;


//记录当前客户端程序调用了哪个具体的service方法
public class Client_Log_Fliter implements client_fliter {
    private static Logger logger = LoggerFactory.getLogger(Client_Log_Fliter.class);

    @Override
    public void do_fliter(List<ChannelFuture_wrapper> src, RPC_invocation invocation) {
        invocation.get_attachments().put("client_app_name",CLIENT_CONFIG.get_application_name());
        logger.info(invocation.get_attachments().get("client_app_name") + "invoke ->"+invocation.get_targetServiceName())  ;
    }
}
