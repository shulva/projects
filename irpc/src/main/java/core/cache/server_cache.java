package core.cache;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import core.register.URL;

public class server_cache {
    public static final Map<String, Object> PROVIDER_MAP = new HashMap<>(); //服务名-服务的对应关系
    public static final Set<URL> PROVIDER_URL_SET = new HashSet<>();
}
