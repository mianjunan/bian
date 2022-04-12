package top.bufeiniao.yuban.service.rpc;

import jdk.jshell.DeclarationSnippet;
import net.vrallev.java.ecc.Ecc25519Helper;
import net.vrallev.java.ecc.KeyHolder;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import top.bufeiniao.yuban.model.data.Declaration;
import top.bufeiniao.yuban.model.work.Obtain;
import top.bufeiniao.yuban.model.work.Task;
import top.bufeiniao.yuban.service.Service;
import top.bufeiniao.yuban.service.config.ConfigInject;
import top.bufeiniao.yuban.util.Util;
import top.bufeiniao.yuban.util.callback.Callback;
import top.bufeiniao.yuban.util.callback.Listener;
import top.bufeiniao.yuban.util.pipeline.Pipeline;
import top.bufeiniao.yuban.util.pipeline.PipelineTask;
import javax.annotation.PostConstruct;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.security.SecureRandom;
import java.util.*;
import java.util.concurrent.LinkedBlockingQueue;

public class RPCService {

    @ConfigInject
    private static int port;
    @ConfigInject
    private static byte[] publicKeySignature;
    @ConfigInject
    private static byte[] publicKeyDiffieHellman;

    private static ServerSocket server;

    private static final Logger log = LoggerFactory.getLogger(RPCService.class);

    private static final HashMap<Long,MessageThread> moduleThread=new HashMap<>();

    private static final Callback<MessageHandle, Listener<MessageHandle>> USE_MESSAGE =new Callback<>();

    private static final Pipeline<MessageHandle> HANDLE_MESSAGE =new Pipeline<>(new PipelineTask<>() {

        @Override
        public void preTreat(MessageHandle message) {
            message.handle();
        }

        @Override
        public void treat(MessageHandle message) {
            USE_MESSAGE.run(message);
        }

        @Override
        public void exception(MessageHandle message, Exception e) {
            if(message!=null){
                log.trace("消息处理出现异常 '{}'",e.getMessage());
            }
        }
    });

    private static Ecc25519Helper helper;

    @PostConstruct
    public void init(){
        helper=new Ecc25519Helper(new KeyHolder(publicKeyDiffieHellman,publicKeySignature));
        try {
            server =new ServerSocket(port);
        } catch (IOException e) {
            return;
        }
        initThread();
    }

    public void initThread(){
        new MessageThread().start();
    }

    public static class MessageThread extends Thread{
        private final LinkedBlockingQueue<byte[]> sendMessage=new LinkedBlockingQueue<>();
        @Override
        public void run() {
            Socket socket;
            DataInputStream in;
            DataOutputStream out;
            long moduleID;
            try {
                byte[] nonce = new byte[64];
                byte[] signature=new byte[64];
                new SecureRandom().nextBytes(nonce);
                socket = server.accept();
                byte[] moduleAddress=socket.getInetAddress().getAddress();
                in=new DataInputStream(socket.getInputStream());
                out=new DataOutputStream(socket.getOutputStream());
                out.write(Objects.requireNonNull(Util.byteArrayMerge(moduleAddress, nonce)));
                if(in.read(signature)==-1){
                    log.warn("模块连接失败，未读取到模块返回信息");
                    return;
                }
                if(!helper.isValidSignature(nonce,signature)){
                    log.warn("私钥验证错误");
                    return;
                }
                byte[] tempModuleID=new byte[8];
                if(in.read(tempModuleID)==-1){
                    log.warn("模块未发送合适信息");
                    return;
                }
                moduleID=new Util.ArrayNumber<Long>(tempModuleID).longValue();
            } catch (Exception e) {
                log.warn("模块连接失败",e);
                return;
            }finally {
                new MessageThread().start();
            }
            moduleThread.put(moduleID,this);
            new Thread(){
                @Override
                public void run(){
                    Service.registerClose(this::interrupt);
                    while(this.isInterrupted()){
                        try {
                            out.write(sendMessage.take());
                        } catch (Exception e) {
                            log.warn("模块连接出现问题",e);
                            return;
                        }
                    }
                }
            }.start();
            Service.registerClose(this::interrupt);
            while(this.isInterrupted()){
                try {
                    HANDLE_MESSAGE.addData(MessageHandleFactory.crop(in));
                } catch (Exception e) {
                    log.warn("模块连接出现问题",e);
                    return;
                }
            }
        }
        public void send(short sign,Context context,Declaration declaration,List<?> message){
            byte[] head=declaration==null?new byte[]{(byte) (message.size())}:new byte[]{(byte) (0b01110000|message.size()-1)},
                    fixed=Util.byteArrayMerge(new Util.ArrayNumber<>(context.tempMessageID()).byteArrayValue(),
                    new Util.ArrayNumber<>(context.sessionID()).byteArrayValue(),
                    new Util.ArrayNumber<>(sign).byteArrayValue()),
                    data = new byte[0],
                    obj=new byte[0];
            for(int i=0;i<(declaration==null?message.size():message.size()-1);i++){
                switch (message.get(i)){
                    case Number n:
                        byte[] ab=new Util.ArrayNumber<>(n).byteArrayValue();
                        head=Util.byteArrayMerge(head,new byte[]{(byte) ab.length});
                        data=Util.byteArrayMerge(data,ab);
                        break;
                    case Boolean b:
                        head=Util.byteArrayMerge(head,new byte[]{1});
                        data=Util.byteArrayMerge(data,new byte[]{(byte) (b?1:0)});
                        break;
                    case List l:
                        if(l.get(0) instanceof Number){
                            head=Util.byteArrayMerge(head,new byte[]{(byte) (new Util.ArrayNumber<>((Number)l.get(0)).byteArrayValue().length*l.size())});
                            for(Object oo:l){
                                data=Util.byteArrayMerge(data,new Util.ArrayNumber<>((Number) oo).byteArrayValue());
                            }
                        }else if(l.get(0) instanceof Boolean){
                            head=Util.byteArrayMerge(head,new byte[]{(byte) (l.size())});
                            for(Object oo:l){
                                data=Util.byteArrayMerge(data,new byte[]{(byte) ((Boolean)oo?1:0)});
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            if(declaration!=null){
                List<Map<?,?>> l= (List<Map<?,?>>) message.get(message.size()-1);
                obj=Util.byteArrayMerge(obj,new byte[]{(byte) (0b10000000)},new Util.ArrayNumber<>(l.size()).byteArrayValue());
                for(Map<?,?> m:l){
                    obj=Util.byteArrayMerge(obj,serialization(declaration,m));
                }
                head=Util.byteArrayMerge(head,new Util.ArrayNumber<>(obj.length).byteArrayValue());
            }
            sendMessage.offer(Objects.requireNonNull(Util.byteArrayMerge(head, fixed, data, obj)));
        }
    }

    public static Callback<MessageHandle, Listener<MessageHandle>> getHandleMessage(){
        return USE_MESSAGE;
    }

    private static Declaration deserialize(byte[] data){
        int index=7;
        long id=new Util.ArrayNumber<Long>(data).longValue();
        List<Declaration.SingleDeclaration> sds=new ArrayList<>();
        sds.add(null);
        while (index<data.length){
            String name=new String(data,index+1,data[index]);
            index+=(data[index]+1);
            Declaration.SingleDeclaration sd=new Declaration.SingleDeclaration(name,
                    switch (data[index]&0b00000111){
                        case 0-> Declaration.BaseType.BYTE;
                        case 1-> Declaration.BaseType.SHORT;
                        case 2-> Declaration.BaseType.INT;
                        case 3-> Declaration.BaseType.LONG;
                        case 4-> Declaration.BaseType.FLOAT;
                        case 5-> Declaration.BaseType.DOUBLE;
                        case 6-> Declaration.BaseType.BOOLEAN;
                        case 7-> Declaration.BaseType.OBJECT;
                        default -> throw new IllegalStateException("Unexpected value: " + (data[index] & 0b00000111));
                    },(data[index]&0b00100000)!=0?new Util.ArrayNumber<>(data,index+1,(data[index]&0b00011000)>>3).intValue():0
                    ,(data[index]&0b01000000)!=0
                    ,(data[index]&0b10000000)!=0
                    ,sds.get(data[index+=((data[index]&0b00100000)!=0?((data[index]&0b00011000)>>3)+1:1)]));
            sds.add(sd);
        }
        return new Declaration(id,sds);
    }

    private static Map<?,?> deserialize(Obtain<Declaration,Long> obtain, byte[] data){
        int index=8;
        long id=new Util.ArrayNumber<Long>(data,0,8).longValue();
        Map<String,Object> map=new HashMap<>();
        for(Declaration.SingleDeclaration sd:obtain.main(id).list()){
            Stack<String> stack=new Stack<>();
            Declaration.SingleDeclaration tempsd=sd.parentNode();
            while(tempsd!=null){
                stack.push(tempsd.name());
                tempsd=tempsd.parentNode();
            }
            Map<String,Object> tempMap=map;
            while (stack.empty()){
                tempMap= (Map<String, Object>) tempMap.get(stack.pop());
            }
            if(sd.isDynamicArray()||sd.arraySize()!=0){
                int length=0;
                if(sd.isDynamicArray()) {
                    length=new Util.ArrayNumber<Integer>(data, index, 4).intValue();
                    index += 4;
                }
                if(sd.arraySize()!=0){
                    length= sd.arraySize();
                }
                switch (sd.baseType()){
                    case BYTE -> {
                        List<Byte> bl=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            bl.add(new Util.ArrayNumber<Integer>(data,index,1).byteValue());
                            index+=1;
                        }
                        tempMap.put(sd.name(),bl);
                    }
                    case SHORT -> {
                        List<Short> sl=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            sl.add(new Util.ArrayNumber<Integer>(data,index,2).shortValue());
                            index+=2;
                        }
                        tempMap.put(sd.name(),sl);
                    }
                    case INT -> {
                        List<Integer> il=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            il.add(new Util.ArrayNumber<Integer>(data,index,4).intValue());
                            index+=4;
                        }
                        tempMap.put(sd.name(),il);
                    }
                    case LONG -> {
                        List<Long> ll=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            ll.add(new Util.ArrayNumber<Integer>(data,index,8).longValue());
                            index+=8;
                        }
                        tempMap.put(sd.name(),ll);
                    }
                    case FLOAT -> {
                        List<Float> fl=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            fl.add(new Util.ArrayNumber<Integer>(data,index,4).floatValue());
                            index+=8;
                        }
                        tempMap.put(sd.name(),fl);
                    }
                    case DOUBLE -> {
                        List<Double> dl=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            dl.add(new Util.ArrayNumber<Integer>(data,index,8).doubleValue());
                            index+=8;
                        }
                        tempMap.put(sd.name(),dl);
                    }
                    case BOOLEAN -> {
                        List<Boolean> bol=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            bol.add(data[index]==1);
                            index+=1;
                        }
                        tempMap.put(sd.name(),bol);
                    }
                    case OBJECT -> {
                        List<Map<String,Object>> ol=new ArrayList<>(length);
                        for (int i=0;i<length;i++){
                            ol.add(new HashMap<>());
                        }
                        tempMap.put(sd.name(),ol);
                    }
                }
                continue;
            }
            tempMap.put(sd.name(),
                    switch (sd.baseType()){
                        case BYTE -> new Util.ArrayNumber<Integer>(data,index,1).byteValue();
                        case SHORT ->new Util.ArrayNumber<Integer>(data,index,2).shortValue();
                        case INT ->new Util.ArrayNumber<Integer>(data,index,4).intValue();
                        case LONG ->new Util.ArrayNumber<Integer>(data,index,8).longValue();
                        case FLOAT ->new Util.ArrayNumber<Integer>(data,index,4).floatValue();
                        case DOUBLE ->new Util.ArrayNumber<Integer>(data,index,8).doubleValue();
                        case BOOLEAN ->data[index]==1;
                        case OBJECT ->new HashMap<String,Object>();
            });
            index+=switch (sd.baseType()) {
                case BYTE, BOOLEAN -> 1;
                case SHORT -> 2;
                case INT, FLOAT -> 4;
                case LONG, DOUBLE -> 8;
                case OBJECT -> 0;
            };
        }
        return map;
    }

    private static byte[] serialization(Declaration declaration, Map<?,?> data){
        byte[] temp=new byte[0];
        for(Declaration.SingleDeclaration sd:declaration.list()){
            Stack<String> stack=new Stack<>();
            Declaration.SingleDeclaration tempsd=sd.parentNode();
            while(tempsd!=null){
                stack.push(tempsd.name());
                tempsd=tempsd.parentNode();
            }
            Map<?,?> tempMap=data;
            while (stack.empty()){
                tempMap= (Map<?, ?>) tempMap.get(stack.pop());
            }
            if(sd.isDynamicArray()||sd.arraySize()!=0){
                if(sd.isDynamicArray()) {
                    temp = Util.byteArrayMerge(temp,
                            new Util.ArrayNumber<Integer>(((List<?>) tempMap.get(sd.name())).size()).byteArrayValue()
                    );
                }
                switch (sd.baseType()){
                    case BYTE -> {
                        for(Object o:(List<?>)tempMap.get(sd.name()))
                            temp=Util.byteArrayMerge(temp,new Util.ArrayNumber<>((Byte) o).byteArrayValue());
                    }
                    case SHORT -> {
                        for(Object o:(List<?>)tempMap.get(sd.name()))
                            temp=Util.byteArrayMerge(temp,new Util.ArrayNumber<>((Short) o).byteArrayValue());
                    }
                    case INT -> {
                        for(Object o:(List<?>)tempMap.get(sd.name()))
                        temp=Util.byteArrayMerge(temp,new Util.ArrayNumber<>((Integer) o).byteArrayValue());
                    }
                    case LONG -> {
                        for(Object o:(List<?>)tempMap.get(sd.name()))
                            temp=Util.byteArrayMerge(temp,new Util.ArrayNumber<>((Long) o).byteArrayValue());
                    }
                    case FLOAT -> {
                        for(Object o:(List<?>)tempMap.get(sd.name()))
                            temp=Util.byteArrayMerge(temp,new Util.ArrayNumber<>((Float) o).byteArrayValue());
                    }
                    case DOUBLE -> {
                        for(Object o:(List<?>)tempMap.get(sd.name()))
                            temp=Util.byteArrayMerge(temp,new Util.ArrayNumber<>((Double) o).byteArrayValue());
                    }
                    case BOOLEAN -> {
                        for(Object o:(List<?>)tempMap.get(sd.name()))
                            temp=Util.byteArrayMerge(temp,new byte[]{(byte) ((boolean)o?1:0)});
                    }
                    case OBJECT -> {

                    }
                }
            }
            if(sd.baseType().equals(Declaration.BaseType.OBJECT)){
                continue;
            }
            temp = Util.byteArrayMerge(temp,
                    switch (sd.baseType()){
                        case BYTE -> new Util.ArrayNumber<>((Byte)tempMap.get(sd.name())).byteArrayValue();
                        case SHORT -> new Util.ArrayNumber<>((Short)tempMap.get(sd.name())).byteArrayValue();
                        case INT -> new Util.ArrayNumber<>((Integer)tempMap.get(sd.name())).byteArrayValue();
                        case LONG -> new Util.ArrayNumber<>((Long)tempMap.get(sd.name())).byteArrayValue();
                        case FLOAT -> new Util.ArrayNumber<>((Float)tempMap.get(sd.name())).byteArrayValue();
                        case DOUBLE -> new Util.ArrayNumber<>((Double)tempMap.get(sd.name())).byteArrayValue();
                        case BOOLEAN -> new byte[]{(byte) ((boolean)tempMap.get(sd.name())?1:0)};
                        default -> throw new IllegalStateException("Unexpected value: " + sd.baseType());
                    }
            );
        }
        return temp;
    }

}
