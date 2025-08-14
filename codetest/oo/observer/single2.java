package cc.observer;

public class single2 implements iwatcher {

    @Override
    public void notice(icanbewatcher bewatcher) {
        System.out.println("对象2观察到变化:" + bewatcher.name());
    }

}
