package starter;

import org.springframework.stereotype.Component;
import java.lang.annotation.*;

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Component
public @interface Anno_Rpc_Service {
    int limit() default 0;

    String group() default "default";

    String serviceToken() default "1";
}
