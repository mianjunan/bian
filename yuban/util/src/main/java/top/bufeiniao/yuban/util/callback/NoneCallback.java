package top.bufeiniao.yuban.util.callback;

import java.util.ArrayList;

/**
 * @author lpyyxy
 * @since 0.0.0.1-alpha
 * @version 1.0
 * <p>提供无参数的回调方法
 */
public final class NoneCallback {
    private final ArrayList<NoneListener> listeners=new ArrayList<>();

    /**
     * 添加一个回调方法
     * @param listener 回调方法
     * @return 回调方法索引
     */
    public int addListener(NoneListener listener){
        synchronized (listeners){
            listeners.add(listener);
            return listeners.size()-1;
        }
    }

    /**
     * 删除一个回调方法
     * @param listener 回调方法
     */
    public void removeListener(NoneListener listener){
        synchronized (listeners) {
            listeners.remove(listener);
        }
    }

    /**
     * 添加一个回调方法
     * @param index 回调方法索引
     */
    public void removeListener(int index){
        synchronized (listeners) {
            listeners.remove(index);
        }
    }

    /**
     * 执行所有回调方法
     */
    public void run() {
        for(NoneListener listener:listeners){
            listener.main();
        }
    }
}
