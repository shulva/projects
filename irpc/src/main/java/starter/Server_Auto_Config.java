package starter;

import irpc.server.*;
import core.event.Rpc_Listener_Loader;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.BeansException;
import org.springframework.beans.factory.InitializingBean;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ApplicationContextAware;

import java.util.Map;

public class Server_Auto_Config implements InitializingBean, ApplicationContextAware {

    private static final Logger LOGGER = LoggerFactory.getLogger(Server_Auto_Config.class);

    private ApplicationContext application_context;

    @Override
    public void afterPropertiesSet() throws Exception {
        System.out.println("Initializing Server_Auto_Config");
        netty_server server = null;

        Map<String, Object> beanMap = application_context.getBeansWithAnnotation(Anno_Rpc_Service.class);
        if (beanMap.size() == 0) {
            //说明当前应用内部不需要对外暴露服务，无需执行下边多余的逻辑
            return;
        }

        printBanner();
        long begin = System.currentTimeMillis();
        server = new netty_server();
        server.init_server_config();

        Rpc_Listener_Loader rpc_Listener_loader = new Rpc_Listener_Loader();
        rpc_Listener_loader.init(); //zk的监听

        for (String beanName : beanMap.keySet()) {
            Object bean = beanMap.get(beanName);
            Anno_Rpc_Service service = bean.getClass().getAnnotation(Anno_Rpc_Service.class);

            Service_Wrapper wrapper = new Service_Wrapper(bean, service.group());
            wrapper.set_service_token(service.serviceToken());
            wrapper.set_limit(service.limit());
            server.register_service(wrapper);
            LOGGER.info(">>>>>>>>>>>>>>> [rpc] {} export success! >>>>>>>>>>>>>>> ",beanName);
            System.out.format(">>>>>>>>>>>>>>> [rpc] {%s} export success! >>>>>>>>>>>>>>> ",beanName);
            System.out.println();
        }

        long end = System.currentTimeMillis();
        server.start();
        LOGGER.info(" ================== [{}] started success in {}s ================== ",server.get_config().get_application_name(),((double)end-(double)begin)/1000);
        System.out.format(" ================== [{%s}] started success in {%.3f}s ================== ",server.get_config().get_application_name(),((double)end-(double)begin)/1000);
        System.out.println();
    }

    @Override
    public void setApplicationContext(ApplicationContext applicationContext) throws BeansException {
        this.application_context = applicationContext;
    }

    private void printBanner(){
        System.out.println();
        System.out.println("==============================================");
        System.out.println("|||---------- Rpc Starting Now! ----------|||");
        System.out.println("==============================================");
        System.out.println();
    }
}
