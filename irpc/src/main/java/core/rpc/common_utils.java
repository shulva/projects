package core.rpc;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;

// 获取目标对象的实现接口
public class common_utils {

    public static List<Class<?>> get_all_interfaces(Class target_class){
        if(target_class==null){
            throw new IllegalArgumentException("target_class is null");
        }

        Class[] clazz = target_class.getInterfaces();
        if(clazz==null){
            return Collections.emptyList();
        }

        List<Class<?>> classes = new ArrayList<>(clazz.length);
        for(Class c : clazz){
            classes.add(c);
        }

        return classes;
    }

    public static String get_ip_address(){
       try {
           Enumeration<NetworkInterface> all_net_interfaces = NetworkInterface.getNetworkInterfaces();
           InetAddress ip_address = null;
           while(all_net_interfaces.hasMoreElements()){
               NetworkInterface net_interface = (NetworkInterface) all_net_interfaces.nextElement();
               if(net_interface.isLoopback()||net_interface.isVirtual()||!net_interface.isUp()){
                   continue;
               }
               else {
                   Enumeration<InetAddress> addresses = net_interface.getInetAddresses();
                   while(addresses.hasMoreElements()){
                       ip_address = addresses.nextElement();
                       if(ip_address!=null && ip_address instanceof Inet4Address)
                           return ip_address.getHostAddress();//获取的是本地设备在局域网（LAN）中使用的 IP 地址，也可能是公网 IP 地址
                   }
               }
           }
       } catch (SocketException e) {
           System.err.println("ip获取失败"+e.toString());
       }
       return "";
    }

    public static boolean isEmpty(String str) {
        return str == null || str.length() == 0;
    }

    public static boolean isEmptyList(List list) {
        if (list == null || list.size() == 0) {
            return true;
        }
        return false;
    }

    public static boolean isNotEmptyList(List list) {
        return !isEmptyList(list);
    }
}