package starter;

import com.esotericsoftware.minlog.Log;
import irpc.client.netty_client;
import irpc.client.*;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeansException;
import org.springframework.beans.factory.InitializingBean;
import org.springframework.beans.factory.config.BeanPostProcessor;
import org.springframework.boot.context.event.ApplicationReadyEvent;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ApplicationContextAware;
import org.springframework.context.ApplicationListener;
import java.io.IOException;
import java.lang.reflect.Field;

public class Client_Auto_Config implements BeanPostProcessor, ApplicationListener<ApplicationReadyEvent> {

    private static RPC_reference reference = null;
    private static netty_client client = null;
    private volatile boolean needInitClient = false;
    private volatile boolean hasInitClientConfig = false;

    private static final Logger LOGGER = LoggerFactory.getLogger(Client_Auto_Config.class);

    @Override
    public Object postProcessAfterInitialization(Object bean, String beanName) throws BeansException {

        Field[] fields = bean.getClass().getDeclaredFields();
        for (Field field : fields) {
            if (field.isAnnotationPresent(Anno_Rpc_Reference.class)) {
                if (!hasInitClientConfig) {
                    //初始化客户端的配置
                    client = new netty_client();
                    try {
                        client.init_client_config();
                        reference = client.init_application();
                    } catch (Exception e) {
                        LOGGER.error("[IRpcClientAutoConfiguration] postProcessAfterInitialization has error ",e);
                        throw new RuntimeException(e);
                    }
                    hasInitClientConfig = true;
                }
                needInitClient = true;

                Anno_Rpc_Reference anno_rpc_reference = field.getAnnotation(Anno_Rpc_Reference.class);
                try {
                    field.setAccessible(true);
                    Object refObj = field.get(bean);
                    RPC_reference_wrapper rpc_reference_wrapper = new RPC_reference_wrapper();

                    rpc_reference_wrapper.set_aim_class(field.getType());
                    rpc_reference_wrapper.set_group(anno_rpc_reference.group());
                    rpc_reference_wrapper.set_service_token(anno_rpc_reference.serviceToken());
                    rpc_reference_wrapper.set_url(anno_rpc_reference.url());
                    rpc_reference_wrapper.setTimeOut(anno_rpc_reference.timeOut());
                    rpc_reference_wrapper.setRetry(anno_rpc_reference.retry());//重传次数
                    rpc_reference_wrapper.set_async(anno_rpc_reference.async());

                    refObj = reference.getProxy(rpc_reference_wrapper);
                    field.set(bean, refObj);
                    client.subscribe_service(field.getType());

                    LOGGER.info(">>>>>>>>>>>>>>> [rpc] {} regist success! >>>>>>>>>>>>>>> ",rpc_reference_wrapper.get_url());
                    System.out.format(">>>>>>>>>>>>>>> [rpc] {%s} regist success! >>>>>>>>>>>>>>> ",rpc_reference_wrapper.get_url());
                    System.out.println();

                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (Throwable throwable) {
                    throwable.printStackTrace();
                }
            }
        }
        return bean;
    }

    @Override
    public void onApplicationEvent(ApplicationReadyEvent applicationReadyEvent) {
        if (needInitClient && client!=null) {
            LOGGER.info(" ================== [{}] started success ================== ",client.getClient_config().get_application_name());
            System.out.println();
            System.out.format(" ================== [{%s}] started success ================== ",client.getClient_config().get_application_name());
            Connection_Handler.set_bootstrap(client.get_bootstrap());
            client.connect_server();
            client.start_client();
        }
    }
}
