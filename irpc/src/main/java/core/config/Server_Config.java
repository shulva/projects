package core.config;

public class Server_Config {

    private Integer port;
    private String application_name;
    private String register_address;

    private String Serialize;

    private Integer serverBizThreadNums;

    private Integer serverQueueSize;//服务端接收队列的大小

    private Integer max_connections;

    public Integer getMax_connections() {
        return max_connections;
    }

    public void setMax_connections(Integer max_connections) {
        this.max_connections = max_connections;
    }

    public Integer getServerQueueSize() {
        return serverQueueSize;
    }

    public void setServerQueueSize(Integer serverQueueSize) {
        this.serverQueueSize = serverQueueSize;
    }

    public Integer getServerBizThreadNums() {
        return serverBizThreadNums;
    }

    public void setServerBizThreadNums(Integer serverBizThreadNums) {
        this.serverBizThreadNums = serverBizThreadNums;
    }


    public String get_Serialize() {
        return Serialize;
    }

    public void set_Serialize(String Serialize) {
        this.Serialize = Serialize;
    }

    public Integer getPort() {
        return port;
    }

    public void setPort(Integer port) {
        this.port = port;
    }


    public String get_application_name(){
        return application_name;
    }

    public void set_application_name(String application_name){
        this.application_name = application_name;
    }

    public String get_register_address(){
        return register_address;
    }

    public void set_register_address(String register_address){
        this.register_address = register_address;
    }

}
