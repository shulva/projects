package provider;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import starter.Client_Auto_Config;

@SpringBootApplication(exclude = {Client_Auto_Config.class})
public class Provider_Config {
    public static void main(String[] args) {
        SpringApplication.run(Provider_Config.class);
    }
}
