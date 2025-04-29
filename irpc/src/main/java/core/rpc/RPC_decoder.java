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


        if(bytebuf.readableBytes() >= base_len) {
            if(bytebuf.readableBytes() >= 1000) { // 太大了，扔掉
                bytebuf.skipBytes(bytebuf.readableBytes());
            }

            int begin_reader;
            while(true) {
                begin_reader = bytebuf.readerIndex();
                bytebuf.markReaderIndex();
                int magic = bytebuf.readShort();
                if(magic==magic_num) {
                    break;
                }
                else{
                    System.err.println("RPC_decoder: invalid magic number");
                    ctx.close();
                    return;
                }
            }

            int length = bytebuf.readInt();
            if(bytebuf.readableBytes() < length) { //半包，不完整，重置索引
                bytebuf.readerIndex(begin_reader);
                return;
            }

            byte[] data = new byte[length];
            bytebuf.readBytes(data);
            RPC_protocol rpc_protocol = new RPC_protocol(data);
            out.add(rpc_protocol);
        }
    }
}
