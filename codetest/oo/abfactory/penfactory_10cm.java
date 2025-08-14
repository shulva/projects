package cc.abfactory;

public class penfactory_10cm implements abstract_industry {

    // 在子类中生成实例
    @Override
    public Ipen createpen() {
        return new pen10cm();
    }

    @Override
    public Ipenhat createpenhat() {

        return new penhat10cm();
    }

}
