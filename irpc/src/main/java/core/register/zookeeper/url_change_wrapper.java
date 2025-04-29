package core.register.zookeeper;

import java.util.List;

public class url_change_wrapper {
    private String service_name;
    private List<String> url_providers;

    public String get_service_name() {
        return service_name;
    }

    public void set_service_name(String service_name) {
        this.service_name = service_name;
    }

    public List<String> get_url_providers_list() {
        return url_providers;
    }

    public void set_url_providers(List<String> url_providers) {
        this.url_providers = url_providers;
    }

    @Override
    public String toString() {
        return "URLChangeWrapper{" +
                "service_name='" + service_name + '\'' +
                ", provider_url=" + url_providers+
                '}';
    }
}
