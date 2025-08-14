package cc.observer;

public class single implements iwatcher {

    @Override
    public void notice(icanbewatcher bewatcher) {
        System.out.println("对象1观察到变化:" + bewatcher.name());
    }
}
