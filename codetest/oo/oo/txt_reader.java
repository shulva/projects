package cc.oo;

import java.io.*;

public class txt_reader {

    BufferedReader br = null;
    String data = null;
    txt_sorter sorter;

    public txt_reader(String name, txt_sorter sorter) throws IOException {
        br = new BufferedReader(new InputStreamReader(new FileInputStream(name)));
        this.sorter = sorter;
    }

    public void read() throws IOException {
        while ((data = br.readLine()) != null) {
            sorter.process(data);
        }
        br.close();
    }

}