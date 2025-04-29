package core.register.zookeeper;

import core.register.Register_Service;
import core.register.URL;

import java.util.List;

import static core.cache.server_cache.PROVIDER_URL_SET;
import static core.cache.client_cache.subscribe_service_list;

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
        subscribe_service_list.add(url.get_service_name());
    }

    @Override
    public void un_subscribe(URL url){
        subscribe_service_list.remove(url.get_service_name());
    }

    //留给子类扩展
    public abstract void after_subscribe(URL url);
    public abstract void before_subscribe(URL url);
    public abstract List<String> get_provider_ip(String service_name);

}
