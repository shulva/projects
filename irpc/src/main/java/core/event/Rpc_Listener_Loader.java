package core.event;

import core.rpc.common_utils;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

//定义好了统一的事件规范，监听接口，下边就需要有专门的类去发送事件了
public class Rpc_Listener_Loader {

    private static List<rpc_listener> rpc_listeners_list = new ArrayList<>();

    private static ExecutorService thread_pool = Executors.newFixedThreadPool(2);

    public static void register_listener(rpc_listener rpc_listener) {
        rpc_listeners_list.add(rpc_listener);
    }

    public void init(){ //如果客户端与服务器是两个机器，那么就需要分别在server和client的main中调用
        register_listener(new service_update_listener());
        register_listener(new provider_node_data_change_listener());
    }

    //获取rpc_listener接口上的泛型T
    public static Class<?> get_interfaceT(Object o){//从一个对象所实现的泛型接口中提取泛型参数的实际类型
        Type[] types = o.getClass().getGenericInterfaces();
        ParameterizedType parameterizedType = (ParameterizedType) types[0];
        Type type = parameterizedType.getActualTypeArguments()[0];
        if(type instanceof ParameterizedType){
            return (Class<?>) type;
        }

        return null;
    }

    //发送事件
    public static void send_event(rpc_event event){
        if(common_utils.isEmptyList(rpc_listeners_list)){
            return;
        }

        for (rpc_listener<?> rpc_listener : rpc_listeners_list) {
            Class<?> type = get_interfaceT(event); //利用反射和泛型
            if(type.equals(event.getClass())){ //确保传来的事件和监听器可以对应上
                thread_pool.execute(new Runnable() {
                    @Override
                    public void run() {
                        try{
                            rpc_listener.callback(event.get_data());
                        }
                        catch (Exception e){
                            e.printStackTrace();
                        }
                    }
                });
            }
        }
    }
}
