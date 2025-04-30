package jmh_test;

import core.serailize.*;
import org.openjdk.jmh.annotations.Benchmark;
import org.openjdk.jmh.runner.Runner;
import org.openjdk.jmh.runner.RunnerException;
import org.openjdk.jmh.runner.options.Options;
import org.openjdk.jmh.runner.options.OptionsBuilder;

public class Serilaize_Compare {
    private static User buildUserDefault() {
        User user = new User();
        user.setAge(11);
        user.setAddress("深圳市南山区");
        user.setBankNo(12897873624813L);
        user.setSex(1);
        user.setId(10001);
        user.setIdCardNo("440308781129381222");
        user.setRemark("备注信息字段");
        user.setUsername("ddd-user-name");
        return user;
    }


    @Benchmark
    public void jdkSerializeTest(){
        Serialize_Factory serializeFactory = new JDK_Serialize_Factory();
        User user = buildUserDefault();
        byte[] result = serializeFactory.serialize(user);
        User deserializeUser = serializeFactory.deserialize(result, User.class);
    }

    @Benchmark
    public void hessianSerializeTest(){
        Serialize_Factory serializeFactory = new Hessian_Serilaize_Factory();
        User user = buildUserDefault();
        byte[] result = serializeFactory.serialize(user);
        User deserializeUser = serializeFactory.deserialize(result, User.class);
    }

    @Benchmark
    public void fastJsonSerializeTest(){
        Serialize_Factory serializeFactory = new FastJson_Serialize_Facotry();
        User user = buildUserDefault();
        byte[] result = serializeFactory.serialize(user);
        User deserializeUser = serializeFactory.deserialize(result, User.class);
    }

    @Benchmark
    public void kryoSerializeTest(){
        Serialize_Factory serializeFactory = new Kryo_Serialize_Factory();
        User user = buildUserDefault();
        byte[] result = serializeFactory.serialize(user);
        User deserializeUser = serializeFactory.deserialize(result, User.class);
    }


    public static void main(String[] args) throws RunnerException {
        Options options = new OptionsBuilder().warmupIterations(2).measurementBatchSize(2)
                .forks(1).build();
        new Runner(options).run();

        /* size:
        fastJson's size is 173
        jdk's size is 418
        kryo's size is 97
        hessian's size is 153
         */
    }
}
