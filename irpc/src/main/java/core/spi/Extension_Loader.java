package core.spi;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class Extension_Loader {

    public static String EXTENSION_LOADER_DIR_PREFIX = "META-INF/rpc/";
    public static Map<String, LinkedHashMap<String,Class>> extension_loader_class_cache = new ConcurrentHashMap<>();

    public void load_extension(Class clazz) throws IOException, ClassNotFoundException {
        if(clazz==null)
            throw new IllegalArgumentException("class is null!");

        String SPI_PATH = EXTENSION_LOADER_DIR_PREFIX + clazz.getName();

        ClassLoader class_loader = this.getClass().getClassLoader();//获取当前类的类加载器，用于查找 SPI 文件。

        Enumeration<URL> enumeration = null;
        enumeration = class_loader.getResources(SPI_PATH);

        while(enumeration.hasMoreElements()){//遍历所有匹配的 SPI 文件，逐行读取文件内容。
            URL url = enumeration.nextElement();
            InputStreamReader inputStreamReader = null;
            inputStreamReader = new InputStreamReader(url.openStream());
            BufferedReader bufferedReader = new BufferedReader(inputStreamReader);

            String line;
            LinkedHashMap<String, Class> classMap = new LinkedHashMap<>();

            while ((line = bufferedReader.readLine()) != null) {
                //如果配置中加入了#开头则表示忽略该类无需进行加载
                if (line.startsWith("#")) {
                    continue;
                }
                String[] lineArr = line.split("=");//按等号分割每行内容，获取实现类名和接口名，并将其映射关系存储在 classMap 中。
                String implClassName = lineArr[0];
                String interfaceName = lineArr[1];
                classMap.put(implClassName, Class.forName(interfaceName));
            }

            //如果 EXTENSION_LOADER_CLASS_CACHE 中已经包含了该类的配置信息，则将新的配置信息合并到已有的配置中；否则，将新的配置信息存储到 EXTENSION_LOADER_CLASS_CACHE 中
            if (extension_loader_class_cache.containsKey(clazz.getName())) {
                extension_loader_class_cache.get(clazz.getName()).putAll(classMap);
            } else {
                extension_loader_class_cache.put(clazz.getName(), classMap);
            }
        }
    }
}
