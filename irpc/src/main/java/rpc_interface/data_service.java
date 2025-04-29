package rpc_interface;
import java.util.List;

public interface data_service {
    String send_data(String data);
    List<String> get_list();
}
