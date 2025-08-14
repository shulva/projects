package cc.abfactory;

public class product {

    private abstract_industry ifactory;

    // 选择生产工厂
    public void set_factory(abstract_industry ifactory) {
        this.ifactory = ifactory;
    }

    public void produce() {
        ifactory.createpen().print();
        ifactory.createpenhat().print();
    }
}
