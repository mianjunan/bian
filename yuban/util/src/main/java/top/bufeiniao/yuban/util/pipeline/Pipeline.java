package top.bufeiniao.yuban.util.pipeline;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.ReentrantLock;

public class Pipeline<T> {
    private final PipelineThreadPool pipelineThreadPool;
    private final LinkedBlockingQueue<T> data=new LinkedBlockingQueue<>();
    public Pipeline(PipelineTask<T> pipelineTask){
        this(Runtime.getRuntime().availableProcessors()+1,pipelineTask);
    }
    public Pipeline(int threadPoolSize,PipelineTask<T> pipelineTask){
        PipelineThreadPool pipelineThreadPool=new PipelineThreadPool(threadPoolSize);
        pipelineThreadPool.addTask(new PipelineThread<>(new ReentrantLock(),data,pipelineThreadPool,pipelineTask));
        this.pipelineThreadPool=pipelineThreadPool;
    }
    public void addData(T datum){
        data.add(datum);
    }
    public void close(){
        pipelineThreadPool.close();
    }
}
