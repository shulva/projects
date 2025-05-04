package consumer.controller;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import rpc_interface.*;
import starter.Anno_Rpc_Reference;

import java.util.List;
import org.springframework.web.bind.annotation.RequestParam; // 1. 导入 @RequestParam

@RestController
@RequestMapping(value = "/user")
public class Controller {

    @Anno_Rpc_Reference
    private data_service data_service;

    @Anno_Rpc_Reference
    private user_service user_service;

    @GetMapping(value = "/test")
    public void test(){
        user_service.test();
    }

    @GetMapping(value = "/send-data")
    public String send_data(@RequestParam("i") String i){ // 2. 在参数前添加 @RequestParam("i")
        data_service.send_data(i);
        return "send data success";
    }

    @GetMapping(value = "/get-list")
    public List<String> get_list(){
        List<String> result =  data_service.get_list();
        System.out.println(result);
        return result;
    }
}
