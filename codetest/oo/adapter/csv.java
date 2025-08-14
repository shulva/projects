package cc.adapter;

import java.util.Vector;

public class csv {

    // 一般来说都是文件读写需要适配
    Vector<String> backstring = new Vector<String>();

    public Vector<String> backVector() {

        backstring.add("我原来是string类型");
        return backstring;
    }
}
