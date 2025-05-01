package irpc.server;

public class Service_Wrapper {

    /**
     * 对外暴露的具体服务对象
     */
    private Object service_obj;

    /**
     * 具体暴露服务的分组
     */
    private String group = "default";

    /**
     * 整个应用的token校验
     */
    private String service_token = "";

    /**
     * 限流策略
     */
    private Integer limit = -1;

    public Service_Wrapper(Object service_obj) {
        this.service_obj = service_obj;
    }

    public Service_Wrapper(Object service_obj, String group) {
        this.service_obj = service_obj;
        this.group = group;
    }

    public Object get_service_obj(){
        return service_obj;
    }

    public void set_service_obj(Object service_obj){
        this.service_obj = service_obj;
    }

    public String get_group(){
        return group;
    }

    public void set_group(String group){
        this.group = group;
    }

    public String get_service_token(){
        return service_token;
    }

    public void set_service_token(String service_token){
        this.service_token = service_token;
    }

    public Integer get_limit(){
        return limit;
    }

    public void set_limit(Integer limit){
        this.limit = limit;
    }
}
