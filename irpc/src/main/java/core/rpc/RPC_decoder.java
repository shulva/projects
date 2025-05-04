package core.rpc;

import io.netty.handler.codec.ByteToMessageDecoder;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.ByteToMessageDecoder;
import java.util.List;
import static core.rpc.constants.magic_num;

public class RPC_decoder extends ByteToMessageDecoder {
    public final int base_len= 2+4;

    @Override
    protected void decode(ChannelHandlerContext ctx, ByteBuf bytebuf, List<Object> out) throws Exception {
        if (bytebuf.readableBytes() >= base_len) {
            if (!(bytebuf.readShort() == magic_num)) {
                ctx.close();
                return;
            }
            int length = bytebuf.readInt();
            if (bytebuf.readableBytes() < length) {
                //数据包有异常
                ctx.close();
                return;
            }
            byte[] body = new byte[length];
            bytebuf.readBytes(body);
            RPC_protocol rpcProtocol = new RPC_protocol(body);
            out.add(rpcProtocol);
        }
    }

}

