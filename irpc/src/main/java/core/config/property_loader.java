package core.config;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

import core.rpc.common_utils;

//将properties的配置转换成本地的一个Map结构进行管理
public class property_loader {
    private static Properties properties;
    private static Map<String,String> properties_map = new HashMap<>();
    private static String path = "/Users/shulva/Documents/projects/irpc/src/main/resources/config.properties";

    public static void load_configuration() throws IOException {
        if(properties != null) {
            return;
        }

        properties = new Properties();
        FileInputStream fis = null;
        fis = new FileInputStream(path);
        properties.load(fis);
    }

    //根据键值获取配置属性
    public static String get_property_str(String key){
        if(properties == null) {
            return null;
        }

        if(common_utils.isEmpty(key))
            return null;

        if(!properties_map.containsKey(key)) {
            String value = properties.getProperty(key);
            properties_map.put(key, value);
        }

        return String.valueOf(properties_map.get(key));
    }

    public static Integer get_property_integer(String key){
        if(properties == null) {
            return null;
        }

        if(common_utils.isEmpty(key))
            return null;

        if(!properties_map.containsKey(key)) {
            String value = properties.getProperty(key);
            properties_map.put(key, value);
        }

        return Integer.valueOf(properties_map.get(key));
    }

}
