package cc.oo;

import java.io.IOException;

public class kwic_oo {
    public static void main(String[] args) throws IOException {

        txt_sorter test_Sorter = new txt_sorter();

        txt_reader test = new txt_reader("D:\\software engineering\\patterns\\input.txt", test_Sorter);
        test.read();

        text_writer tw = new text_writer(test_Sorter, "D:\\software engineering\\patterns\\output.txt");
        tw.write();
    }
}
