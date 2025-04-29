package core.register.zookeeper;

public class provider_node_info {

    private String service_name;

    private String address;

    public String get_service_name(){
        return service_name;
    }

    public String get_address(){
        return address;
    }

    public void set_service_name(String service_name){
        this.service_name = service_name;
    }

    public void set_address(String address){
        this.address = address;
    }

    @Override
    public String toString() { //?何用意
        return "ProviderNodeInfo{" +
                "serviceName='" + service_name + '\'' +
                ", address='" + address + '\'' +
                '}';
    }
}
