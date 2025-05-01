package core.fliter.client;

import core.fliter.client_fliter;
import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;
import core.rpc.common_utils;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;

import static core.cache.client_cache.CLIENT_CONFIG;

/*
分组的服务管理在团队协作过程中使用的频率会比较高，
例如当A、B两个工程师在共同开发一个叫做UserService服务时候，
A的UserService开发还未完成，处于自测阶段，
B的UserService已经开发完成，进入了测试阶段，
而此时只有一个注册中心供团队使用，那么此时可能会出现：测试同学进行功能测试的时候，调用到了A写的UserService，从而影响测试结果的准确性。

而如果我们将服务按照组别进行管理，
A开发的UserService的group设置为dev，
B开发的UserService的group设置为test，
而远程调用的时候严格遵守group参数进行匹配调用，这样就能确保测试在调用服务的时候，不会将请求路由到A所写的还未完善的UserService上边了。
 */

//基于分组的过滤链路
public class Group_Fliter implements client_fliter {
    @Override
    public void do_fliter(List<ChannelFuture_wrapper> src, RPC_invocation invocation) {
        String group = String.valueOf(invocation.get_attachments().get("group"));

        for (ChannelFuture_wrapper channelFuture : src) {
            if(!channelFuture.getGroup().equals(group)){
                src.remove(channelFuture);
            }
        }

        if(common_utils.isEmptyList(src)){
            throw new RuntimeException("no provider match group for" + group);
        }
    }
}
