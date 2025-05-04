package irpc.server;

import io.netty.channel.ChannelHandlerContext;

import core.rpc.RPC_protocol;

public class server_channel_read {

    private RPC_protocol rpcProtocol;

    private ChannelHandlerContext channelHandlerContext;

    public RPC_protocol getRpcProtocol() {
        return rpcProtocol;
    }

    public void setRpcProtocol(RPC_protocol rpcProtocol) {
        this.rpcProtocol = rpcProtocol;
    }

    public ChannelHandlerContext getChannelHandlerContext() {
        return channelHandlerContext;
    }

    public void setChannelHandlerContext(ChannelHandlerContext channelHandlerContext) {
        this.channelHandlerContext = channelHandlerContext;
    }
}
