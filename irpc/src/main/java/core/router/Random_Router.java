package core.router;

import core.register.URL;
import core.rpc.ChannelFuture_wrapper;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import static core.cache.client_cache.CONNECT_CHANNEL_MAP;
import static core.cache.client_cache.CHANNEL_FUTURE_POLLING_REF;
import static core.cache.client_cache.SERVICE_ROUTER_MAP;

//预先产生一个随机数组，该数组定义好了需要调用的服务提供者顺序，接下来按照这个随机顺序去做轮询
public class Random_Router implements Router{

    @Override
    public void refresh_router_array(Selector selector) {

        //获取服务提供者的数目
        List<ChannelFuture_wrapper> wrappers = CONNECT_CHANNEL_MAP.get(selector.get_provider_service_name());
        ChannelFuture_wrapper[] arr = new ChannelFuture_wrapper[wrappers.size()];

        //提前生成调用先后顺序的随机数组
        int[] result = create_random_index(arr.length);

        for (int i = 0; i < result.length; i++) {
            arr[i] = wrappers.get(result[i]); //arr 是 result 随机下标带来的随机顺序
        }

        SERVICE_ROUTER_MAP.put(selector.get_provider_service_name(), arr);
    }

    //随机路由层内部对外暴露的核心方法是 select 函数，每次外界调用服务的时候都是通过这个函数去获取下一次调用的 provider 信息
    @Override
    public ChannelFuture_wrapper select(Selector selector) {
        return CHANNEL_FUTURE_POLLING_REF.get_ChannelFuture_wrapper(selector.get_provider_service_name());
    }

    @Override
    public void update_weight(URL url) {

        List<ChannelFuture_wrapper> list = CONNECT_CHANNEL_MAP.get(url.get_service_name());
        Integer[] weight = create_weight_array(list);//权重数组
        Integer[] final_arr = create_random_array(weight);//最终的随机下标数组
        ChannelFuture_wrapper[] final_arr_wrapper = new ChannelFuture_wrapper[final_arr.length];

        for (int i = 0; i < final_arr.length; i++) {
            final_arr_wrapper[i] = list.get(final_arr[i]);//最终的随机顺序
        }

        SERVICE_ROUTER_MAP.put(url.get_service_name(), final_arr_wrapper);
    }

    //权重值约定好配置是 100 的整倍数，权重高的提供者的下标会出现更多次
    private static Integer[] create_weight_array(List<ChannelFuture_wrapper> wrappers_list){
        List<Integer> weight_arr = new ArrayList<>();
        for (int i = 0; i < wrappers_list.size(); i++) {
            Integer weight = wrappers_list.get(i).get_weight();
            int c = weight/100;

            for (int j = 0; j < c; j++) {
                weight_arr.add(i);
            }
        }

        Integer arr[] = new Integer[weight_arr.size()];
        return weight_arr.toArray(arr);
    }

    //创建随机乱序数组
    private static Integer[] create_random_array(Integer arr[]){
        int total = arr.length;
        Random random  = new Random();

        for (int i=0;i<total;i++){
            int j = random.nextInt(total);

            if(i==j)
                continue;

            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }

        return arr;
    }

    //生成一大堆随机下标，互不相同，在 len 范围内
    private int[] create_random_index(int len){
        int [] arr_int = new int[len];
        Random random = new Random();
        for (int i = 0; i < arr_int.length; i++) {
            arr_int[i] = -1;
        }

        int index = 0;
        while (index<arr_int.length){
            int num = random.nextInt(len);
            //如果数组中不包含这个元素则赋值给数组
            if(!contains(arr_int,num)){
                arr_int[index++] = num;
            }
        }

        return arr_int;
    }

    public boolean contains(int[] arr, int key){
        for (int i = 0; i < arr.length; i++) {
            if (arr[i] == key) {
                return true;
            }
        }
        return false;
    }

    //测试
    public static void main(String[] args) {
        List<ChannelFuture_wrapper> channelFutureWrappers = new ArrayList<>();
        channelFutureWrappers.add(new ChannelFuture_wrapper(null, null, 100));
        channelFutureWrappers.add(new ChannelFuture_wrapper(null, null, 200));
        channelFutureWrappers.add(new ChannelFuture_wrapper(null, null, 9300));
        channelFutureWrappers.add(new ChannelFuture_wrapper(null, null, 400));
        Integer[] r = create_weight_array(channelFutureWrappers);
        System.out.println(r);
    }
}
