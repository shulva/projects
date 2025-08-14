package cc.oo;

import java.util.ArrayList;
import java.util.Comparator;

public class txt_sorter {

    ArrayList<String> list = new ArrayList<String>();
    ArrayList<String> outputlist = new ArrayList<String>();
    ArrayList<String> list2 = new ArrayList<String>();

    String[] split1 = null;
    String temp = "";

    public void process(String data) {

        split1 = data.split(" ");// 分割句子
        for (int i = 0; i < split1.length; i++) {
            list.add(split1[i]);
        }

        for (int j = 0; j < split1.length; j++) {// 构造循环移位
            temp = "";

            for (String string : list) {
                temp += string + " ";
            }
            outputlist.add(temp);

            list.add(list.get(0));
            list.remove(0);
        }

        newsort();// 给outputlist排序
        for (String string : outputlist) {
            list2.add(string);
        }

        outputlist.clear();
        list.clear();
    }

    public void show() {// 打印outputlist中的内容
        for (String string : outputlist) {
            System.out.println(string);
        }
    }

    public void newsort() {// 覆写sort函数
        outputlist.sort(new Comparator<String>() {
            @Override
            public int compare(String o1, String o2) {
                o1 = o1.toLowerCase();
                o2 = o2.toLowerCase();
                return o1.charAt(0) > o2.charAt(0) ? 1 : -1;
            }
        });
    }

    public ArrayList<String> get() {// 对writer的接口

        return list2;
    }

}
