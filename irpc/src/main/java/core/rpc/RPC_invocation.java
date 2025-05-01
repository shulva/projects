package core.rpc;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class RPC_invocation {
    //请求的目标方法，例如findUser
    private String targetMethod;
    //请求的目标服务名称，例如：com.sise.user.UserService
    private String targetServiceName;
    //请求参数信息
    private Object[] args;
    private String uuid;
    //接口响应的数据塞入这个字段中（如果是异步调用或者void类型，这里就为空）
    private Object response;

    private Map<String, Object> attachments = new ConcurrentHashMap<>();

    public Map<String, Object> get_attachments() {
        return attachments;
    }

    public void set_attachments(Map<String, Object> attachments) {
        this.attachments = attachments;
    }

    //getter setter toString 省略
    public String get_targetMethod() {
        return targetMethod;
    }

    public void set_TargetMethod(String targetMethod) {
        this.targetMethod = targetMethod;
    }

    public String get_targetServiceName() {
        return targetServiceName;
    }

    public void set_TargetServiceName(String targetServiceName) {
        this.targetServiceName = targetServiceName;
    }

    public Object[] get_Args() {
        return args;
    }

    public void set_Args(Object[] args) {
        this.args = args;
    }

    public String get_uuid() {
        return uuid;
    }

    public void set_uuid(String uuid) {
        this.uuid = uuid;
    }

    public Object get_response() {
        return response;
    }

    public void set_response(Object response) {
        this.response = response;
    }

}
