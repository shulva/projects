package core.serialize;

import java.io.*;

public class JDK_Serialize_Factory implements Serialize_Factory {

    @Override
    public <T> byte[] serialize(T t) {
        byte[] data = null;
        try{
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ObjectOutputStream output = new ObjectOutputStream(baos);

            output.writeObject(t);
            output.flush();
            output.close();
            data = baos.toByteArray();

        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return data;
    }

    @Override
    public <T> T deserialize(byte[] data, Class<T> clazz) {
        ByteArrayInputStream bais = new ByteArrayInputStream(data);
        try{
            ObjectInputStream input = new ObjectInputStream(bais);
            Object result = input.readObject();
            return (T) result;
        } catch (IOException e) {
            throw new RuntimeException(e);
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }
}
