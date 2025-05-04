package core.proxy;

import core.rpc.RPC_invocation;
import irpc.client.RPC_reference_wrapper;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.util.UUID;
import java.util.concurrent.TimeoutException;

import static core.cache.client_cache.RESP_MAP;
import static core.cache.client_cache.SEND_QUEUE;

//核心任务是将需要调用的方法名称、服务名称，参数统统都封装好到RpcInvocation当中，然后塞入到一个队列里，并且等待服务端的数据返回。
public class JDK_Client_Invocation_Handler implements InvocationHandler {

    private final static Object ob = new Object();
    private RPC_reference_wrapper reference_wrapper; //主要的信息还是封装的类
    private long timeout = 3600;

    public JDK_Client_Invocation_Handler(RPC_reference_wrapper reference_wrapper) {
        this.reference_wrapper = reference_wrapper;
        timeout = Long.parseLong(reference_wrapper.getTimeOUt());
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable { //调用该代理对象的任何方法时，都会触发 InvocationHandler 接口中 invoke 方法的执行
        RPC_invocation invocation = new RPC_invocation();
        invocation.set_Args(args);
        invocation.set_TargetMethod(method.getName());
        invocation.set_TargetServiceName(reference_wrapper.get_aim_class().getName());
        invocation.set_uuid(UUID.randomUUID().toString()); //独特的uuid
        invocation.set_attachments(reference_wrapper.get_attatchments());
        invocation.set_retry(reference_wrapper.getRetry());

        RESP_MAP.put(invocation.get_uuid(),ob); //uuid先占一个ob位，后面client_handler中会真正放入invocation
        SEND_QUEUE.add(invocation);

        if(reference_wrapper.is_async()) //不关心调用方法的返回，只是想在服务端调用方法,从而优化这些请求的性能
            return null;

        long start = System.currentTimeMillis();
        int retry_times = 0; //尝试重传的次数
        while (System.currentTimeMillis() - start < timeout || invocation.get_retry()>0) { //判断是否出现了超时异常 或者 是否设置了重置次数
            Object obj = RESP_MAP.get(invocation.get_uuid());
            if(obj!=null && obj instanceof RPC_invocation) {

                RPC_invocation response = (RPC_invocation) obj;

                if(response.get_retry()==0 && response.get_exception()==null){
                    return response.get_response();
                } else if (response.get_exception() != null) {

                    if(response.get_retry()==0) //每次重试会将retry值减1
                        return response.get_response(); //retry次数没了，异常信息给客户端

                    if(System.currentTimeMillis() - start > timeout ){ //超时才会重传
                        retry_times++;
                        invocation.set_response(null);
                        invocation.set_retry(response.get_retry()-1);
                        RESP_MAP.put(invocation.get_uuid(),ob); //uuid先占一个ob位，后面client_handler中会真正放入invocation
                        SEND_QUEUE.add(invocation);
                    }
                }

                return ((RPC_invocation)obj).get_response();
            }
        }

        System.out.println("Wait for response from server on client " + timeout + "ms,retry times is " + retry_times + ",service's name is " + invocation.get_targetServiceName() + "#" + invocation.get_targetMethod());
        throw new TimeoutException("Wait for response from server on client " + timeout + "ms,retry times is " + retry_times + ",service's name is " + invocation.get_targetServiceName() + "#" + invocation.get_targetMethod());
    }
}
