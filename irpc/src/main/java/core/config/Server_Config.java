package core.config;

public class Server_Config {

    private Integer port;
    private String application_name;
    private String register_address;

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
