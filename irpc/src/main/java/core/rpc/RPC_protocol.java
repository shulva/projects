package core.rpc;

import java.io.Serializable;
import static core.rpc.constants.magic_num;

public class RPC_protocol implements Serializable { // 固定协议传输
    private static final long serialVersionUID = 1L;
    private short magic_number = magic_num;
    private int content_length;
    private byte[] content;

    public RPC_protocol(byte[] content) {
        this.magic_number = magic_num;
        this.content_length = content.length;
        this.content = content;
    }

    public byte[] get_content(){
        return content;
    }

    public int get_content_len(){
        return content_length;
    }

    public short get_magic_number(){
        return magic_number;
    }

    public void set_content(byte[] content){
        this.content = content;
    }

    public void set_content_len(int content_len){
        this.content_length = content_len;
    }

}
