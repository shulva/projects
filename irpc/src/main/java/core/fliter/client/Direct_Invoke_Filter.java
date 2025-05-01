package core.fliter.client;


import core.fliter.client_fliter;
import core.rpc.ChannelFuture_wrapper;
import core.rpc.RPC_invocation;
import core.rpc.common_utils;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.List;
import java.util.Iterator;


import static core.cache.client_cache.CLIENT_CONFIG;

/*
按照指定ip访问的方式请求server端是我们在测试阶段会比较常见的方式
例如服务部署之后，发现2个名字相同的服务，面对相同的请求参数，在两个服务节点中返回的结果却不一样，此时就可以通过指定请求ip来进行debug诊断。
 */
public class Direct_Invoke_Filter implements  client_fliter {

    @Override
    public void do_fliter(List<ChannelFuture_wrapper> src, RPC_invocation invocation) {
        String url = (String) invocation.get_attachments().get("url");

        if(common_utils.isEmpty(url))
            return;

        Iterator<ChannelFuture_wrapper> iterator = src.iterator();
        while (iterator.hasNext()) {
            ChannelFuture_wrapper channelFuture = iterator.next();
            if(!(channelFuture.getHost()+":"+channelFuture.getPort()).equals(url))
                iterator.remove();
        }

        if(common_utils.isEmptyList(src))
            throw new RuntimeException("no match provider url for"+url);
    }
}
