package irpc.client;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

//rpc远程调用包装类
public class RPC_reference_wrapper<T> {

    private Class<T> aim_class;

    //信息中心
    private Map<String,Object> attatchments = new ConcurrentHashMap<>();

    public Class<T> get_aim_class() {
        return aim_class;
    }

    public void set_aim_class(Class<T> aim_class) {
        this.aim_class= aim_class;
    }

    public boolean is_async(){
        return Boolean.valueOf(String.valueOf(attatchments.get("async")));
    }

    public void set_async(boolean async){
        this.attatchments.put("async",async);
    }

    public String get_url(){
        return String.valueOf(attatchments.get("url"));
    }

    public void set_url(String url){
        attatchments.put("url",url);
    }

    public String get_service_token(){
        return String.valueOf(attatchments.get("service_token"));
    }

    public void set_service_token(String service_token){
        attatchments.put("service_token",service_token);
    }

    public String get_group(){
        return String.valueOf(attatchments.get("group"));
    }

    public void set_group(String group){
        attatchments.put("group",group);
    }

    public Map<String, Object> get_attatchments() {
        return attatchments;
    }

    public void set_attatchments(Map<String, Object> attatchments) {
        this.attatchments = attatchments;
    }
}
