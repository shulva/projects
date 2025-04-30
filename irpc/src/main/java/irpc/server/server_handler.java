package irpc.server;

import com.alibaba.fastjson.JSON;
import core.rpc.RPC_invocation;
import core.rpc.RPC_protocol;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Map;

import static core.cache.server_cache.PROVIDER_MAP;
import static core.cache.server_cache.SERVER_SERIALIZE_FACTORY;

public class server_handler extends ChannelInboundHandlerAdapter {

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws InvocationTargetException, IllegalAccessException { //服务器接收到客户端发送的数据时会触发该方法
        //服务端接收数据的时候统一以RpcProtocol协议的格式接收
        System.out.println("--------------------------");
        System.out.println("server接收到数据！");
        RPC_protocol rpc_protocol = (RPC_protocol) msg;
        //String json = new String(rpc_protocol.get_content(),0,rpc_protocol.get_content_len());
        //RPC_invocation rpc_invocation = JSON.parseObject(json,RPC_invocation.class);
        RPC_invocation rpc_invocation = SERVER_SERIALIZE_FACTORY.deserialize(rpc_protocol.get_content(), RPC_invocation.class);

        Object aim = PROVIDER_MAP.get(rpc_invocation.get_targetServiceName());

        Method methods[] = aim.getClass().getDeclaredMethods();

        Object result = null;
        for (Method method : methods) {
            if(method.getName().equals(rpc_invocation.get_targetMethod())) {

                if(method.getReturnType().equals(Void.TYPE)) {
                    method.invoke(aim,rpc_invocation.get_Args());
                }
                else
                    result = method.invoke(aim,rpc_invocation.get_Args());
            }
            break;
        }

        rpc_invocation.set_response(result);
        RPC_protocol res_rpc = new RPC_protocol(SERVER_SERIALIZE_FACTORY.serialize(rpc_invocation));
        ctx.writeAndFlush(res_rpc);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        cause.printStackTrace();
        Channel ch = ctx.channel();
        if(ch.isActive()) {
            ctx.close();
        }
    }
}
