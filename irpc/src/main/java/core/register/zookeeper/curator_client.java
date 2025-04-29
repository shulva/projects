package core.register.zookeeper;

import core.register.URL;
import org.apache.curator.RetryPolicy;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.retry.ExponentialBackoffRetry;
import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.data.Stat;
import static org.apache.zookeeper.Watcher.Event.EventType.NodeDeleted;
import org.apache.curator.framework.state.ConnectionState;
import org.apache.curator.framework.state.ConnectionStateListener;


import java.util.Collection;
import java.util.Collections;
import java.util.List;

public class curator_client extends abstract_zookeeper_client{
    private CuratorFramework client;

    public curator_client(String zk_address) {
        this(zk_address,null,null);
    }

    public curator_client(String zk_address,Integer basic_sleep_time,Integer max_retry_times) {
        super(zk_address,basic_sleep_time,max_retry_times);
        RetryPolicy retryPolicy = new ExponentialBackoffRetry(super.get_basic_sleep_time(), super.get_max_retry_times());

        if(client==null){
            //这个session time可能需要设置的长一点
            client = CuratorFrameworkFactory.builder().connectString(zk_address).sessionTimeoutMs(5000).retryPolicy(retryPolicy).build();
            client = CuratorFrameworkFactory.builder()
                    .connectString(zk_address)
                    .sessionTimeoutMs(50000)
                    .retryPolicy(retryPolicy)
                    .build();

            ConnectionStateListener listener = (curatorFramework, newState) -> {
                System.out.println("****** Zookeeper Connection State Changed: " + newState + " ******");
                if (newState == ConnectionState.LOST) {
                    // 会话丢失，可能需要执行清理或重新注册逻辑
                    System.err.println("****** Zookeeper session lost! ******");
                } else if (newState == ConnectionState.SUSPENDED) {
                    // 连接暂时挂起，Curator 正在尝试重连
                    System.out.println("****** Zookeeper connection suspended, retrying... ******");
                } else if (newState == ConnectionState.RECONNECTED) {
                    // 重新连接成功
                    System.out.println("****** Zookeeper reconnected successfully! ******");
                    // 可能需要在这里重新创建临时节点或重新设置 Watcher
                }
            };
            client.getConnectionStateListenable().addListener(listener);

            client.start();
        }

    }

    @Override
    public Object get_client() {
        return client;
    }

    @Override
    public void destory() {
        client.close();
    }

    @Override
    public String update_nodedata(String path, String data) {
        try {
            client.setData().forPath(path,data.getBytes());
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    @Override
    public String get_nodedata(String path) {
        try {
            byte[] res  = client.getData().forPath(path);
            if(res!=null){
                return new String(res);
            }
        }
        catch (KeeperException.NoNodeException e){
            return null;
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    @Override
    public List<String> get_children_data(String path) {
        try {
            List<String> children_data = client.getChildren().forPath(path);
            return children_data;
        }
        catch (KeeperException.NoNodeException e){
            return null;
        }
        catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    @Override
    public void create_persistent_data(String address, String data) {
        try {
            client.create().creatingParentsIfNeeded().withMode(CreateMode.PERSISTENT).forPath(address,data.getBytes());
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void create_persistent_with_seqdata(String address, String data) {
        try {
            client.create().creatingParentContainersIfNeeded().withMode(CreateMode.PERSISTENT_SEQUENTIAL).forPath(address, data.getBytes());
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void create_temporary_seqdata(String address, String data) {
        try {
            client.create().creatingParentContainersIfNeeded().withMode(CreateMode.EPHEMERAL_SEQUENTIAL).forPath(address, data.getBytes());
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void create_temporary_data(String address, String data) {
        try{
            // 使用 creatingParentsIfNeeded() 代替 creatingParentContainersIfNeeded()
            client.create().creatingParentsIfNeeded().withMode(CreateMode.EPHEMERAL).forPath(address, data.getBytes());
        }
        catch (KeeperException.NodeExistsException e) {
            // 如果节点已存在（可能是因为之前的会话异常断开后未清理），尝试更新数据
            // 注意：这取决于你的业务逻辑是否允许覆盖已存在的临时节点
            try {
                client.setData().forPath(address, data.getBytes());
                System.out.println("Temporary node " + address + " already exists, updated data.");
            } catch (Exception ex) {
                // 如果更新也失败，记录并抛出异常
                System.err.println("Failed to update existing temporary node " + address);
                throw new RuntimeException("Failed to create or update temporary node: " + address, ex);
            }
        }
        catch (KeeperException.NoChildrenForEphemeralsException e) {
            // 这个异常理论上不应该在创建 EPHEMERAL 节点时直接发生，除非父节点是临时的
            System.err.println("Attempted to create an ephemeral node under an ephemeral parent? Path: " + address);
            throw new RuntimeException(e.getMessage(), e); // 直接抛出原始异常
        }
        catch (Exception ex) {
            // 捕获其他所有异常，包括 UnimplementedException
            System.err.println("Failed to create temporary node " + address);
            // 抛出原始异常，以便更清晰地看到问题，例如 UnimplementedException
            throw new RuntimeException(ex.getMessage(), ex);
        }
    }


    @Override
    public void set_temporary_data(String address, String data) {
        try {
            client.setData().forPath(address, data.getBytes());
        } catch (Exception ex) {
            throw new IllegalStateException(ex.getMessage(), ex);
        }
    }

    @Override
    public List<String> listNode(String address) {
        try{
            List<String> res = client.getChildren().forPath(address);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return Collections.emptyList();
    }

    @Override
    public boolean deleteNode(String address) {
        try{
            client.delete().forPath(address);
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    @Override
    public boolean existNode(String address) {
        try {
            Stat stat = client.checkExists().forPath(address);
            return stat!=null;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    @Override
    public void watch_nodedata(String path, Watcher watcher) {
        try {
            client.getData().usingWatcher(watcher).forPath(path);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void watch_child_nodedata(String path, Watcher watcher) {
        try {
            client.getChildren().usingWatcher(watcher).forPath(path);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        abstract_zookeeper_client client = new curator_client("localhost:2181");
        client.watch_nodedata("/rpc/rpc_interface.data_service/provider/10.1.21.11:9092", new Watcher() {
            @Override
            public void process(WatchedEvent watchedEvent) {
                System.out.println(watchedEvent.getType());
                if(NodeDeleted.equals(watchedEvent.getType())){
                    provider_node_info provider_node_info = URL.build_url_from_urlstr(watchedEvent.getPath());
                    System.out.println(provider_node_info);
                }
            }
        });

        while(true){

        }
    }

}
