package core.register.zookeeper;

import org.apache.zookeeper.Watcher;

import java.util.List;

public abstract class abstract_zookeeper_client {
    private String zk_address;
    private int basic_sleep_time;
    private int max_retry_times;

    public abstract_zookeeper_client(String zk_address){
        this.zk_address = zk_address;

        //默认3000ms
        this.basic_sleep_time = 1000;
        this.max_retry_times = 3;
    }

    public abstract_zookeeper_client(String zk_address, Integer basic_sleep_time, Integer max_retry_times) {
        this.zk_address = zk_address;

        if(basic_sleep_time == null){
            this.basic_sleep_time = 1000;
        }
        else
            this.basic_sleep_time = basic_sleep_time;

        if(max_retry_times == null){
            this.max_retry_times = 3;
        }
        else
            this.max_retry_times = max_retry_times;
    }

    public int get_basic_sleep_time() {
        return basic_sleep_time;
    }

    public void set_basic_sleep_time(int basic_sleep_time) {
        this.basic_sleep_time = basic_sleep_time;
    }

    public int get_max_retry_times() {
        return max_retry_times;
    }

    public void set_max_retry_times(int max_retry_times) {
        this.max_retry_times = max_retry_times;
    }

    public abstract Object get_client();
    public abstract void destory();//断开连接

    public abstract String update_nodedata(String path, String data);
    public abstract String get_nodedata(String path);//获取zk节点数据
    public abstract List<String> get_children_data(String path);//获取指定目录下zk节点数据

    public abstract void create_persistent_data(String address, String data);//创建持久化类型节点数据信息
    public abstract void create_persistent_with_seqdata(String address, String data);//创建持久且有序型节点数据信息

    public abstract void create_temporary_seqdata(String address, String data);//创建有序且临时类型节点数据信息
    public abstract void create_temporary_data(String address, String data);//创建临时节点数据类型信息
    public abstract void set_temporary_data(String address, String data);//设置某个节点的数值

    /**
     * @param address
     */
    public abstract List<String> listNode(String address);//展示节点下边的数据
    public abstract boolean deleteNode(String address);//删除节点下边的数据
    public abstract boolean existNode(String address);//判断是否存在其他节点
    public abstract void watch_nodedata(String path, Watcher watcher);//监听path路径下某个节点的数据变化
    public abstract void watch_child_nodedata(String path, Watcher watcher);//监听子节点下的数据变化
}
