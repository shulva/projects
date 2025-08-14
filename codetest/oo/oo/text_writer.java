package cc.oo;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;

public class text_writer {

    txt_sorter ts = null;
    String name;
    BufferedWriter bw = null;

    public text_writer(txt_sorter tSorter, String name) {
        this.ts = tSorter;
        this.name = name;
    }

    public void write() throws IOException {
        bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(name)));

        for (int i = 0; i < ts.get().size(); i++) {
            bw.write(ts.get().get(i));
            bw.newLine();
            bw.flush();
        }

        bw.close();
    }

}
