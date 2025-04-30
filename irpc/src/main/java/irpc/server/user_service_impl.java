package irpc.server;
import rpc_interface.user_service;

public class user_service_impl implements user_service{

    @Override
    public void test() {
        System.out.println("user test");
    }
}
