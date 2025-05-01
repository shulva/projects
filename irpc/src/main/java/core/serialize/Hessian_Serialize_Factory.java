package core.serialize;

import com.caucho.hessian.io.Hessian2Input;
import com.caucho.hessian.io.Hessian2Output;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

public class Hessian_Serialize_Factory implements Serialize_Factory{
    @Override
    public <T> byte[] serialize(T t) {

        byte[] data = null;
        try{
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            Hessian2Output output = new Hessian2Output(baos);
            output.writeObject(t);
            output.getBytesOutputStream().flush();
            output.completeMessage();
            output.close();

            data = baos.toByteArray();

        } catch (Exception e) {
            throw new RuntimeException(e);
        }

        return data;
    }

    @Override
    public <T> T deserialize(byte[] data, Class<T> clazz) {
        if(data == null || data.length == 0){
            return null;
        }

        Object result = null;
        try{
            ByteArrayInputStream bais = new ByteArrayInputStream(data);
            Hessian2Input input = new Hessian2Input(bais);
            result = input.readObject();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return (T)result;
    }
}
