package provider;
import rpc_interface.user_service;
import starter.Anno_Rpc_Service;

@Anno_Rpc_Service(group = "default", limit = 100)
public class user_service_impl implements user_service{

    @Override
    public void test() {
        System.out.println("user test");
    }
}
