package cc.observer;

import java.util.ArrayList;
import java.util.List;

public class center implements icanbewatcher {

    private List<iwatcher> watcherlist = new ArrayList<iwatcher>();

    private String name1 = "西游记";

    public void change(String name1) {// 改变时通知所有观察者
        this.name1 = name1;
        inform();
    }

    @Override
    public void regist(iwatcher watcher) {// 登记操作
        watcherlist.add(watcher);
    }

    @Override
    public void delete(iwatcher watcher) {// 删除操作
        watcherlist.remove(watcher);
    }

    @Override
    public String name() {
        return name1;
    }

    public void inform() {// 通知，调用所有已注册的观察者的notice函数()
        for (iwatcher iwatcher : watcherlist) {
            iwatcher.notice(this);
        }
    }
}
