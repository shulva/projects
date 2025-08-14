package cc.observer;

interface icanbewatcher {
    public void regist(iwatcher watcher);

    public void delete(iwatcher watcher);

    public String name();
}
