package top.bufeiniao.yuban.service;

import top.bufeiniao.yuban.model.work.NoneTask;

import java.util.HashSet;

public class Service {
    private final static HashSet<NoneTask> closeEnrollment=new HashSet<>();
    public static void registerClose(NoneTask task){
        closeEnrollment.add(task);
    }
    public static void close(){
        for(NoneTask task:closeEnrollment){
            task.main();
        }
    }
}
