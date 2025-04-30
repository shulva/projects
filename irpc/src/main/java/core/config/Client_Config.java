package core.config;

public class Client_Config {
    private Integer port;
    private String server_addr;

    public Integer get_port(){
        return port;
    }

    public String get_server_addr(){
        return server_addr;
    }

    public void set_port(Integer port){
        this.port = port;
    }

    public void set_server_addr(String server_addr){
        this.server_addr = server_addr;
    }

    private String application_name;
    private String register_address;
    private String proxy_type;
    private String router_strategy;
    private String Serialize;

    public String get_Serialize() {
        return Serialize;
    }

    public void set_Serialize(String Serialize) {
        this.Serialize = Serialize;
    }

    public String get_router_strategy(){
        return router_strategy;
    }

    public void set_router_strategy(String router_strategy){
        this.router_strategy = router_strategy;
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

    public String get_proxy_type(){
        return proxy_type;
    }

    public void set_proxy_type(String proxy_type){
        this.proxy_type = proxy_type;
    }
}
