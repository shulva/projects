package provider;
import rpc_interface.data_service;
import starter.Anno_Rpc_Service;

import java.util.ArrayList;
import java.util.List;

@Anno_Rpc_Service(group = "default", limit = 100)
public class data_service_impl implements data_service{
    @Override
    public String send_data(String data) {
        System.out.println("invoke success");
        return data;
    }

    @Override
    public List<String> get_list() {
        ArrayList<String> list = new ArrayList<>();
        list.add("hello1");
        list.add("hello2");
        System.out.println("getting list");

        return list;
    }
}
