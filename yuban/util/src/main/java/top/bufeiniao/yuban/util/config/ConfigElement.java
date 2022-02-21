package top.bufeiniao.yuban.util.config;

import java.util.ArrayList;
import java.util.List;

/**
 * @author lpyyxy
 * @version 1.0
 * <p>
 * @since 0.0.0.1-alpha
 */
public record ConfigElement<T>(T datum) {

    @SuppressWarnings("unchecked")
    public List<T> getAsArray() {
        if(datum instanceof List<?>){
            return ((List<T>) datum);
        }
        return null;
    }

    public boolean getAsBoolean() {
        return (boolean) datum;
    }

    public String getAsString() {
        return (String) datum;
    }

    public double getAsDouble() {
        return (double) datum;
    }

    public float getAsFloat() {
        return (float) datum;
    }

    public long getAsLong() {
        return (long) datum;
    }

    public int getAsInteger() {
        return (int) datum;
    }

    public byte getAsByte() {
        return (byte) datum;
    }



    public short getAsShort() {
        return (short) datum;
    }
}
