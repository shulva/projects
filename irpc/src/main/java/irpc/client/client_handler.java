package irpc.client;


import com.alibaba.fastjson.JSON;
import core.rpc.RPC_invocation;
import core.rpc.RPC_protocol;
import io.netty.buffer.ByteBuf;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.SimpleChannelInboundHandler;
import io.netty.util.ReferenceCountUtil;

import static core.cache.client_cache.RESP_MAP;
import static core.cache.client_cache.SEND_QUEUE;

public class client_handler extends ChannelInboundHandlerAdapter {
    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
        //客户端和服务端之间的数据都是以RpcProtocol对象作为基本协议进行的交互
        System.out.println("client received !");
        RPC_protocol protocol = (RPC_protocol) msg;

        //这里是传输参数更为详细的RpcInvocation对象字节数组。
        byte[] content = protocol.get_content();
        String json = new String(content,0,content.length);

        RPC_invocation invocation = JSON.parseObject(json, RPC_invocation.class);
        //通过之前发送的uuid来注入匹配的响应数值

        if(!RESP_MAP.containsKey(invocation.get_uuid())){ //uuid在client_invocation_handler中提前放入了，没有便是错误
            throw new IllegalArgumentException("server response wrong");
        }

        RESP_MAP.put(invocation.get_uuid(),invocation);
        ReferenceCountUtil.release(msg);
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) throws Exception {
        super.exceptionCaught(ctx, cause);
        Channel channel = ctx.channel();
        if(channel.isActive())
            channel.close();
    }
}
