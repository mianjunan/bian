package top.bufeiniao.yuban.util.pipeline;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

class PipelineThreadPool {
    private final ThreadPoolExecutor handleThreadPool;
    PipelineThreadPool(int threadNum) {
        handleThreadPool=new ThreadPoolExecutor(threadNum,threadNum,10, TimeUnit.SECONDS,new LinkedBlockingQueue<>(1));
    }
    void addTask(Thread task){
        handleThreadPool.execute(task);
    }
    void close() {
        handleThreadPool.shutdownNow();
    }
}
