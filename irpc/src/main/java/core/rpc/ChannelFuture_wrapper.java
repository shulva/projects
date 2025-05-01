package core.rpc;

import io.netty.channel.ChannelFuture;

public class ChannelFuture_wrapper { //可能有多个ip,需要记录host和port
    private ChannelFuture channelFuture;
    private String host;
    private Integer port;
    private Integer weight; // 路由权重,权重值约定好配置是100的整倍数
    private String group;

    public ChannelFuture get_ChannelFuture(){
        return channelFuture;
    }

    public ChannelFuture_wrapper(){

    }

    public ChannelFuture_wrapper( String host, Integer port , Integer weight) {
        this.host = host;
        this.port = port;
        this.weight = weight;
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

    public Integer get_weight(){
        return weight;
    }

    public void set_weight(Integer weight) {
        this.weight = weight;
    }

    public String getGroup() {
        return group;
    }

    public void setGroup(String group) {
        this.group = group;
    }

    @Override
    public String toString() {
        return "ChannelFutureWrapper{" +
                "channelFuture=" + channelFuture +
                ", host='" + host + '\'' +
                ", port=" + port +
                ", weight=" + weight +
                ", group='" + group + '\'' +
                '}';
    }
}
