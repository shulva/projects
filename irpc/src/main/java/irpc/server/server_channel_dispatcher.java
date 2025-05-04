package irpc.server;

import java.lang.reflect.Method;
import java.util.concurrent.*;

import core.rpc.*;

import static core.cache.server_cache.*;
import static core.cache.server_cache.SERVER_SERIALIZE_FACTORY;

/*
- server_handler 是生产者，负责将请求放入队列。
- server_channel_dispatcher 内部的 core_handle 线程是消费者，
它不断地从 RPC_READ_QUEUE 中取出请求（server_channel_read 对象）。
对于每个取出的请求，core_handle 会将其包装成一个任务，提交给 executor_service 线程池去执行。
线程池中的 业务线程 负责执行真正的 RPC 处理逻辑
 */
public class server_channel_dispatcher {

    //当队列为空时，消费者（ core_handle 线程）会被阻塞
    private BlockingQueue<server_channel_read> RPC_READ_QUEUE ;

    private ExecutorService executor_service;

    public server_channel_dispatcher() {

    }

    public void init(int queueSize, int bizThreadNums) {
        RPC_READ_QUEUE = new ArrayBlockingQueue<>(queueSize);
        executor_service = new ThreadPoolExecutor(bizThreadNums, bizThreadNums,
                0L, TimeUnit.MILLISECONDS,
                new ArrayBlockingQueue<>(512));
    }

    public void add(server_channel_read data) {
        //调用Put方法则会阻塞
        RPC_READ_QUEUE.add(data);
    }

    public void start_data_consume() {
        Thread thread = new Thread(new core_handle());
        thread.start();
    }

    class core_handle implements Runnable {

        @Override
        public void run() {
            while (true) {
                try{
                    server_channel_read data = RPC_READ_QUEUE.take();
                    executor_service.submit(new Runnable() { //线程池
                        @Override
                        public void run() { //这里是原来 server_handler 的核心处理逻辑
                            try{

                                System.out.println("--------------------------");
                                System.out.println("server 接收到数据！");
                                RPC_protocol rpc_protocol = data.getRpcProtocol();
                                //String json = new String(rpc_protocol.get_content(),0,rpc_protocol.get_content_len());
                                //RPC_invocation rpc_invocation = JSON.parseObject(json,RPC_invocation.class);
                                RPC_invocation rpc_invocation = SERVER_SERIALIZE_FACTORY.deserialize(rpc_protocol.get_content(), RPC_invocation.class);

                                //责任链启动
                                try{
                                    SERVER_FLITER_CHAIN.do_fliter(rpc_invocation);
                                }
                                catch (Exception e){
                                    rpc_invocation.set_exception(e);
                                    byte[] body = SERVER_SERIALIZE_FACTORY.serialize(rpc_invocation);
                                    RPC_protocol res_protocol = new RPC_protocol(body);
                                    data.getChannelHandlerContext().writeAndFlush(res_protocol);
                                    return;
                                }

                                Object aim = PROVIDER_MAP.get(rpc_invocation.get_targetServiceName());
                                Method methods[] = aim.getClass().getDeclaredMethods();
                                Object result = null;

                                for (Method method : methods) { //当前的逻辑无法处理重载

                                    if(method.getName().equals(rpc_invocation.get_targetMethod())) {
                                        if(method.getReturnType().equals(Void.TYPE)) {
                                            try{
                                                method.invoke(aim,rpc_invocation.get_Args());
                                            }
                                            catch (Exception e) {
                                                rpc_invocation.set_exception(e);
                                            }
                                        }
                                        else
                                            try{
                                                result = method.invoke(aim,rpc_invocation.get_Args());
                                            }
                                            catch (Exception e) {
                                                rpc_invocation.set_exception(e);
                                            }
                                        break;
                                    }
                                }

                                rpc_invocation.set_response(result);//获取到结果
                                RPC_protocol res_rpc = new RPC_protocol(SERVER_SERIALIZE_FACTORY.serialize(rpc_invocation));//序列化
                                data.getChannelHandlerContext().writeAndFlush(res_rpc);
                            } catch (Exception e) {
                                e.printStackTrace();
                                throw new RuntimeException(e);
                            }
                        }
                    });
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    throw new RuntimeException(e);
                }
            }
        }
    }
}
