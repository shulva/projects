package core.fliter.server;

import java.util.concurrent.Semaphore;

public class Server_Semaphore_Wrapper {
    private Semaphore semaphore;

    private int maxNums;

    public Server_Semaphore_Wrapper(int maxNums) {

        this.maxNums = maxNums;
        this.semaphore = new Semaphore(maxNums);
    }

    public Semaphore getSemaphore() {
        return semaphore;
    }

    public void setSemaphore(Semaphore semaphore) {
        this.semaphore = semaphore;
    }

    public int getMaxNums() {
        return maxNums;
    }

    public void setMaxNums(int maxNums) {
        this.maxNums = maxNums;
    }
}
