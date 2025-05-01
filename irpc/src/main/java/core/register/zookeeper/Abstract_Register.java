package core.register.zookeeper;

import core.register.Register_Service;
import core.register.URL;

import java.util.List;
import java.util.Map;

import static core.cache.server_cache.PROVIDER_URL_SET;
import static core.cache.client_cache.SUBSCRIBE_SERVICE_LIST;

public abstract class Abstract_Register implements Register_Service {

    @Override
    public void register(URL url){
       PROVIDER_URL_SET.add(url);
    }

    @Override
    public void un_register(URL url){
        PROVIDER_URL_SET.remove(url);
    }

    @Override
    public void subscribe(URL url){
        SUBSCRIBE_SERVICE_LIST.add(url);
    }

    @Override
    public void un_subscribe(URL url){
        SUBSCRIBE_SERVICE_LIST.remove(url);
    }

    //留给子类扩展
    public abstract void after_subscribe(URL url);
    public abstract void before_subscribe(URL url);
    public abstract List<String> get_provider_ip(String service_name);
    public abstract Map<String,String> get_service_nodeinfo_map(String service_name);

}
