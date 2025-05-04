package starter;

import java.lang.annotation.*;

@Target(ElementType.FIELD)// 指定该注解只能用于类的字段（成员变量）
@Retention(RetentionPolicy.RUNTIME)// 指定该注解在运行时可见，可以通过反射获取
@Documented// 指定该注解会被 javadoc 工具记录
public @interface Anno_Rpc_Reference {
    String url() default "";

    String group() default "default";

    String serviceToken() default "1";

    int timeOut() default 3000;

    int retry() default 1;

    boolean async() default false;
}
