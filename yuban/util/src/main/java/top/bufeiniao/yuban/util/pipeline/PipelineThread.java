package top.bufeiniao.yuban.util.pipeline;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

class PipelineThread<T> extends Thread{
    private final Lock lock;
    private final LinkedBlockingQueue<T> data;
    private final PipelineThreadPool pipelineThreadPool;
    private final PipelineTask<T> pipelineTask;
    PipelineThread(Lock lock, LinkedBlockingQueue<T> data, PipelineThreadPool pipelineThreadPool, PipelineTask<T> pipelineTask){
        this.lock=lock;
        this.data=data;
        this.pipelineTask=pipelineTask;
        this.pipelineThreadPool=pipelineThreadPool;
    }
    @Override
    public void run(){
        T datum = null;
        try{
            Lock newLock = new ReentrantLock();
            datum=data.take();
            newLock.lock();
            pipelineThreadPool.addTask(new PipelineThread<>(newLock,data,pipelineThreadPool,pipelineTask));
            pipelineTask.preTreat(datum);
            lock.lock();
            pipelineTask.treat(datum);
            newLock.unlock();
        }catch (Exception e){
            pipelineTask.exception(datum,e);
        }
    }
}
