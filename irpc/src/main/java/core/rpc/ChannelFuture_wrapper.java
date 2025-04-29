package core.rpc;

import io.netty.channel.ChannelFuture;

public class ChannelFuture_wrapper { //可能有多个ip,需要记录host和port
    private ChannelFuture channelFuture;
    private String host;
    private Integer port;

    public ChannelFuture get_ChannelFuture(){
        return channelFuture;
    }

    public String getHost() {
        return host;
    }

    public Integer getPort() {
        return port;
    }

    public void set_ChannelFuture(ChannelFuture channelFuture) {
        this.channelFuture = channelFuture;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public void setPort(Integer port) {
        this.port = port;
    }
}
