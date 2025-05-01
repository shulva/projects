package core.fliter.server;

import core.fliter.server_fliter;

import core.rpc.RPC_invocation;
import core.rpc.common_utils;
import irpc.server.Service_Wrapper;

import static core.cache.server_cache.PROVIDER_SERVICE_WRAPPER_MAP;

//请求抵达服务端调用具体方法之前，先对其调用凭证进行判断操作，如果凭证不一致则抛出异常 保障安全
public class Server_Token_Fliter implements server_fliter{

    @Override
    public void do_fliter(RPC_invocation invocation) {
        String token = String.valueOf(invocation.get_attachments().get("service_token"));
        Service_Wrapper service_wrapper = PROVIDER_SERVICE_WRAPPER_MAP.get(invocation.get_targetServiceName());
        String Match_token = String.valueOf(service_wrapper.get_service_token());

        if(common_utils.isEmpty(Match_token)) {
            return;
        }

        if(!common_utils.isEmpty(token)|| token.equals(Match_token)) {
            return;
        }

        throw new RuntimeException("token is"+token+"verfiy is false");
    }
}
