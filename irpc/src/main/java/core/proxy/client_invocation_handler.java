package core.proxy;

import core.rpc.RPC_invocation;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.util.UUID;
import java.util.concurrent.TimeoutException;

import static core.cache.client_cache.RESP_MAP;
import static core.cache.client_cache.SEND_QUEUE;

//核心任务是将需要调用的方法名称、服务名称，参数统统都封装好到RpcInvocation当中，然后塞入到一个队列里，并且等待服务端的数据返回。
public class client_invocation_handler implements InvocationHandler {

    private final static Object ob = new Object();
    private Class<?> any_class;

    public client_invocation_handler(Class<?> any_class) {
        this.any_class = any_class;
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        RPC_invocation invocation = new RPC_invocation();
        invocation.set_Args(args);
        invocation.set_TargetMethod(method.getName());
        invocation.set_TargetServiceName(any_class.getName());
        invocation.set_uuid(UUID.randomUUID().toString()); //独特的uuid

        RESP_MAP.put(invocation.get_uuid(),ob); //uuid先占一个ob位，后面client_handler中会真正放入invocation
        SEND_QUEUE.add(invocation);

        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < 3600) {
            Object obj = RESP_MAP.get(invocation.get_uuid());
            if(obj instanceof RPC_invocation) {
                return ((RPC_invocation)obj).get_response();
            }
        }

        System.out.println("invoke!");
        throw new TimeoutException("client wait server's response timeout!");
    }
}
