package core.rpc;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToByteEncoder;

public class RPC_encoder extends MessageToByteEncoder<RPC_protocol> {

    @Override
    protected void encode(ChannelHandlerContext channelHandlerContext, RPC_protocol msg, ByteBuf byteBuf) throws Exception {
        byteBuf.writeShort(msg.get_magic_number());
        byteBuf.writeInt(msg.get_content_len());
        byteBuf.writeBytes(msg.get_content());
    }
}
