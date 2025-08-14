package cc.abfactory;

public class penfactory_15cm implements abstract_industry {

    @Override
    public Ipen createpen() {

        return new pen15cm();
    }

    @Override
    public Ipenhat createpenhat() {

        return new penhat15cm();
    }

}