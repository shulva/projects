package irpc.server;

import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.ChannelOption;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Collections;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.LongAdder;

public class Max_Connect_Handler extends ChannelInboundHandlerAdapter {
    private Logger logger = LoggerFactory.getLogger(Max_Connect_Handler.class);

    private final int max_connect_times;

    private final AtomicInteger connect_num = new AtomicInteger(0);
    private final Set<Channel> child_channel = Collections.newSetFromMap(new ConcurrentHashMap<>());

    private final LongAdder num_dropped = new LongAdder();//高并发场景下的计数操作
    private final AtomicBoolean logged = new AtomicBoolean(false);

    public Max_Connect_Handler(int max_connect_times) {
        this.max_connect_times = max_connect_times;
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {

        System.out.println("connection limit handler");
        Channel channel = (Channel) msg;

        int conn = connect_num.incrementAndGet();
        if (conn > 0 && conn <= max_connect_times) {
            this.child_channel.add(channel);
            channel.closeFuture().addListener(future -> {
                // 当通道关闭时，从子通道集合中移除该通道
                child_channel.remove(channel);
                // 原子性地减少连接数量计数器
                connect_num.decrementAndGet();
            });
            super.channelRead(ctx, msg);
        } else { //太多了
            connect_num.decrementAndGet();
            //避免产生大量的time_wait连接
            channel.config().setOption(ChannelOption.SO_LINGER, 0);
            // 原子性地减少连接数量计数器
            channel.unsafe().closeForcibly();
            num_dropped.increment();

            //这里加入一道cas可以减少一些并发请求的压力,定期地执行一些日志打印
            if (logged.compareAndSet(false, true)) {
                ctx.executor().schedule(this::writeNumDroppedConnectionLog,1, TimeUnit.SECONDS);
            }
        }
    }

    /**
     * 记录连接失败的日志
     */
    private void writeNumDroppedConnectionLog() {
        logged.set(false); //重新设置成false，允许其他线程打印
        final long dropped = num_dropped.sumThenReset();
        if(dropped>0){
            logger.error("Dropped {} connection(s) to protect server,maxConnection is {}",dropped,max_connect_times);
        }
    }
}
