package cc.adapter;

import java.util.Vector;

public class csv_adapter implements csv_reader {// 适配器类
    public Vector<person> adapt() {

        // 我要person，但只有string，故要转化
        csv string_need = new csv();
        // 需要被适配的类
        Vector<String> stringVector = string_need.backVector();
        Vector<person> personVector = new Vector<person>()

        for (String testperson : stringVector) {
            personVector.add(new person(testperson));// person的构造函数，stirng全变为name
        }

        return personVector;
    }
}
