package top.bufeiniao.yuban.util;

import top.bufeiniao.yuban.model.work.Task;
import java.util.HashMap;

/**
 *
 * @author lpyyxy
 * @since 0.0.0.1-alpha
 * 利用map来跳转
 */
public class Judge<K,T> {

    private final HashMap<K, Task<T>> branches=new HashMap<>();
    public Judge<K,T> addBranch(K branch,Task<T> task){
        branches.put(branch,task);
        return this;
    }
    public void run(K branch,T datum) {
        branches.get(branch).main(datum);
    }
}
