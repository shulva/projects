package core.serialize;
import com.alibaba.fastjson.JSON;

public class FastJson_Serialize_Facotry implements Serialize_Factory{
    @Override
    public <T> byte[] serialize(T t) {
        String json = JSON.toJSONString(t);
        return json.getBytes();
    }

    @Override
    public <T> T deserialize(byte[] data, Class<T> clazz) {
        return JSON.parseObject(data, clazz);
    }
}
