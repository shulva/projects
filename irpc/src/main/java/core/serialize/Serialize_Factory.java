package core.serialize;

public interface Serialize_Factory {
    <T> byte[] serialize(T t);
    <T> T deserialize(byte[] data, Class<T> clazz);
}
