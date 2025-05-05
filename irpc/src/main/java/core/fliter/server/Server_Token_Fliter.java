package core.fliter.server;

import core.fliter.server_fliter;

import core.rpc.RPC_invocation;
import core.rpc.common_utils;
import irpc.server.Service_Wrapper;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static core.cache.server_cache.PROVIDER_SERVICE_WRAPPER_MAP;

//请求抵达服务端调用具体方法之前，先对其调用凭证进行判断操作，如果凭证不一致则抛出异常 保障安全
@SPI(value = "before")
public class Server_Token_Fliter implements server_fliter{

    private static final Logger logger = LoggerFactory.getLogger(Server_Token_Fliter.class);
    @Override
    public void do_fliter(RPC_invocation invocation) {
        System.out.println(invocation.get_targetServiceName());
        System.out.println(invocation.get_targetMethod());
        String token = String.valueOf(invocation.get_attachments().get("service_token"));
        Service_Wrapper service_wrapper = PROVIDER_SERVICE_WRAPPER_MAP.get(invocation.get_targetServiceName());
        String Match_token = String.valueOf(service_wrapper.get_service_token());

        if(common_utils.isEmpty(Match_token)) {
            return;
        }

        if(!common_utils.isEmpty(token)|| token.equals(Match_token)) {
            return;
        }

        logger.error(invocation.get_attachments().get("service_token") + "verify false")  ;
        throw new RuntimeException("token is"+token+"verfiy is false");
    }
}
