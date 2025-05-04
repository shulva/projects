package consumer;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import starter.Server_Auto_Config;

@SpringBootApplication(exclude = {Server_Auto_Config.class})
public class Consumer_Config {
    public static void main(String[] args) {
        SpringApplication.run(Consumer_Config.class);
    }
}
