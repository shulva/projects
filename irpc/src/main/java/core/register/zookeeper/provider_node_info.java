package core.register.zookeeper;

public class provider_node_info {

    private String service_name;

    private String address;

    private Integer weight;

    private String register_time;

    public String get_register_time() {
        return register_time;
    }

    public void set_register_time(String register_time) {
        this.register_time = register_time;
    }

    public Integer get_weight() {
        return weight;
    }

    public void set_weight(Integer weight) {
        this.weight = weight;
    }

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
    public String toString() {
        return "ProviderNodeInfo{" +
                "serviceName='" + service_name + '\'' +
                ", address='" + address + '\'' +
                ", weight=" + weight +
                ", registryTime='" + register_time + '\'' +
                '}';
    }


}
