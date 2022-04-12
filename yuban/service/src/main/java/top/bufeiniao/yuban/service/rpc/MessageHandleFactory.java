package top.bufeiniao.yuban.service.rpc;

import top.bufeiniao.yuban.util.Util;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class MessageHandleFactory {
    public static MessageHandle crop(InputStream in) throws IOException {
        byte[] head=new byte[1];
        byte[] temp = new byte[0];
        List<byte[]> preData=new ArrayList<>();
        byte[] object;
        if(in.read(head)!=-1){
            if((head[0]&0b01000000)==0){
                temp=new byte[head[0]&0b1111];
            }else {
                temp=new byte[head[0]&0b11+(head[0]&0b110000)>>4+1];
            }
            in.read(temp);
        }
        head=Util.byteArrayMerge(head,temp);
        byte[] data=new byte[18];
        in.read(data);
        for(int i=0;i<((head[0]&0b01000000)==0?head[0]&0b1111:head[0]&0b11);i++){
            byte[] b=new byte[i+1];
            in.read(b);
            preData.add(b);
        }
        object=new byte[new Util.ArrayNumber<>(head,head.length-5,4).intValue()];
        in.read(object);

        return new Command(data,preData,object);
    }
}
interface MessageHandle{
    void handle();
}

class Command implements MessageHandle{
    private final  byte[] data;
    private short trait;
    private Context context;
    private List<byte[]> preData;
    private byte[] object;
    public Command(byte[] data,List<byte[]> preData,byte[] object){
        this.data=data;
        this.preData=preData;
        this.object=object;
    }
    @Override
    public void handle() {
        Util.ArrayNumber<Long> tempMessageID=new Util.ArrayNumber<>(data,0,8);
        Util.ArrayNumber<Long> sessionID=new Util.ArrayNumber<>(data,8,8);
        context=new Context(tempMessageID.longValue(),sessionID.longValue());
        trait=new Util.ArrayNumber<Short>(data,16,2).shortValue();
    }
}