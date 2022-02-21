package top.bufeiniao.yuban.service.timer;

import com.fasterxml.jackson.databind.JsonSerializer;
import jakarta.annotation.PostConstruct;
import org.springframework.stereotype.Service;
import top.bufeiniao.yuban.model.work.NoneTask;
import top.bufeiniao.yuban.service.config.ConfigInject;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

@Service
public class TimerService {
    private static ScheduledExecutorService scheduled;

    @ConfigInject
    private static int corePoolSize;

    @PostConstruct
    public void init(){
        scheduled= Executors.newScheduledThreadPool(corePoolSize);
    }
    public static void schedule(NoneTask task,long timeMillis){
        long delay=timeMillis-System.currentTimeMillis();
        if(delay>0){
            scheduled.schedule(task::main,delay,TimeUnit.MILLISECONDS);
        }else{
            scheduled.execute(task::main);
        }
    }
}
