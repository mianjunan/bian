package top.bufeiniao.yuban.util.callback;

/**
 * @author lpyyxy
 * @since 0.0.0.1-alpha
 * @version 1.0
 * <p>有参数回调方法接口
 */
@FunctionalInterface
public interface Listener<T>{
    /**
     * 要执行的回调方法
     * @param datum 回调方法
     */
    void main(T datum);
}
