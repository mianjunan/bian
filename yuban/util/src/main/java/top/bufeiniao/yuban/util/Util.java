package top.bufeiniao.yuban.util;

import java.util.Arrays;
import java.util.Base64;
import java.util.Random;

public final class Util {
    private static final Base64.Decoder DECODER = Base64.getDecoder();
    private static final Base64.Encoder ENCODER = Base64.getEncoder();

    public static byte[] base64Decoder(String text) {
        return DECODER.decode(text);
    }

    public static String base64Encoder(byte[] text) {
        return ENCODER.encodeToString(text);
    }

    private static final Random RANDOM = new Random();

    public static int random(int max) {
        return RANDOM.nextInt(max+1);
    }
    public static int random() {
        return RANDOM.nextInt();
    }

    public static class  ArrayNumber<T extends Number> extends Number{
        byte[] byteArray;
        public ArrayNumber(byte[] byteArray){
            this.byteArray=byteArray;
        }
        public ArrayNumber(byte[] byteArray,int offset, int length){
            this.byteArray= Arrays.copyOfRange(byteArray, offset, offset + length);
        }
        public ArrayNumber(T t){
            long temp=switch (t) {
                case Float f->Float.floatToIntBits(f);
                case Double d->Double.doubleToLongBits(d);
                default -> 0;
            };
            byteArray=switch (t){
                case Byte b->new byte[]{b};
                case Short s->new byte[]{(byte) (s>>8),s.byteValue()};
                case Integer i->new byte[]{(byte) (i>>24),(byte) (i>>16),(byte) (i>>8),i.byteValue()};
                case Long l->new byte[]{(byte) (l>>56),(byte) (l>>48),(byte) (l>>40),(byte) (l>>32),(byte) (l>>24),(byte) (l>>16),(byte) (l>>8),l.byteValue()};
                case Float f->new byte[]{(byte) (temp>>24),(byte) (temp>>16),(byte) (temp>>8),(byte) temp};
                case Double d->new byte[]{(byte) (temp>>56),(byte) (temp>>48),(byte) (temp>>40),(byte) (temp>>32),(byte) (temp>>24),(byte) (temp>>16),(byte) (temp>>8), (byte) temp};
                default -> new byte[0];
            };
        }
        @Override
        public byte byteValue() {
            return byteArray[0];
        }

        @Override
        public short shortValue() {
            return (short) (((byteArray[0]&0xff)<<8)|
                                (byteArray[1]&0xff));
        }
        @Override
        public int intValue() {
            return ((byteArray[0]&0xff)<<24)|
                    ((byteArray[1]&0xff)<<16)|
                    ((byteArray[2]&0xff)<<8)|
                    (byteArray[3]&0xff);
        }

        @Override
        public long longValue() {
            return ((byteArray[0]&0xffL)<<56)|
                    ((byteArray[1]&0xffL)<<48)|
                    ((byteArray[2]&0xffL)<<40)|
                    ((byteArray[3]&0xffL)<<32)|
                    ((byteArray[4]&0xffL)<<24)|
                    ((byteArray[5]&0xffL)<<16)|
                    ((byteArray[6]&0xffL)<<8)|
                    (byteArray[7]&0xffL);
        }

        @Override
        public float floatValue() {
            return Float.intBitsToFloat(intValue());
        }

        @Override
        public double doubleValue() {
            return Double.longBitsToDouble(longValue());
        }

        public byte[] byteArrayValue() {
            return byteArray;
        }
    }

    public static short intToShort(int data) {
        return (short)data;
    }
    public static int shortToInt(short data) {
        return ((int)data)&0x0000ffff;
    }

    public static byte[] byteArrayMerge(byte[]... Byte) {
        if(Byte.length!=0) {
            int length=0;
            for (byte[] bytes : Byte) {
                length = length + bytes.length;
            }
            byte[] Command=new byte[length];
            int j=0;
            for (byte[] bytes : Byte) {
                System.arraycopy(bytes, 0, Command, j, bytes.length);
                j = j + bytes.length;
            }
            return Command;
        }
        return null;
    }
}
