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

import static core.cache.server_cache.*;

public class server_handler extends ChannelInboundHandlerAdapter {

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws InvocationTargetException, IllegalAccessException { //服务器接收到客户端发送的数据时会触发该方法

        //服务端接收数据的时候统一以 RpcProtocol 协议的格式接收
        //不直接处理业务逻辑。
        //将接收到的 RPC_protocol 数据和 ChannelHandlerContext 封装成一个对象。
        //将这个封装好的请求对象放入 RPC_READ_QUEUE 阻塞队列中。
        //尽快释放 Netty 的 I/O 线程，让其可以继续处理其他网络事件（如接受新连接、读写更多数据）。
        server_channel_read data=new server_channel_read();
        data.setRpcProtocol((RPC_protocol) msg);
        data.setChannelHandlerContext(ctx);
        SERVER_CHANNEL_DISPATCHER.add(data);
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
