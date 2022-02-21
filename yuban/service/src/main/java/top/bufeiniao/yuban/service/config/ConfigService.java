package top.bufeiniao.yuban.service.config;

import jakarta.annotation.PostConstruct;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;
import top.bufeiniao.yuban.util.config.ConfigElement;
import top.bufeiniao.yuban.util.config.ConfigOperate;
import java.io.File;
import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

@Service
public class ConfigService {
    private static final Logger log = LoggerFactory.getLogger(ConfigService.class);
    private final ConfigOperate config=new ConfigOperate(ConfigService.class.getResourceAsStream("config./config.yml"));

    @PostConstruct
    @SuppressWarnings("unchecked")
    public void init(){
        try {
            ConfigElement<String> pathList= Objects.requireNonNull((ConfigElement<String>)config.getData(""));
            assert pathList.getAsArray()!= null;
            for(String path:pathList.getAsArray()){
                config.updateConfig(new File(path),(e)->log.warn("权限不足，无法创建配置文件 '{}'",e.getMessage()));
            }
        }catch (NullPointerException ignored){}
        configInject();
    }
    @SuppressWarnings("unchecked")
    public void configInject(){
        String path=config.getData("configInject.path").toString();
        Map<String,?> injectConfig= (Map<String, ?>) config.getData("configInject.args").datum();

    }
    @SuppressWarnings("unchecked")
    private void configInject(String path,Map<String,?> injectConfig){
        //TODO 内部类不确定能否使用
        HashMap<String, Field> annotations=new HashMap<>();
        injectConfig.forEach((key,value)->{
            if(value instanceof Map<?,?>){
                configInject(path+"."+key, (Map<String, ?>) value);
            }else{
                if(annotations.isEmpty()){
                    try {
                        Class<?> annotationClass = Class.forName(path);
                        for (Field field:annotationClass.getFields()){
                            if(field.isAnnotationPresent(ConfigInject.class)){
                                ConfigInject annotation = field.getAnnotation(ConfigInject.class);
                                annotations.put(annotation.id().equals("unknown")?field.getName():annotation.id(),field);
                            }
                        }
                    } catch (ClassNotFoundException e) {
                        log.warn("该类无法注入 '{}' '{}'",path,e.getMessage());
                    }
                }
                Field field;
                if((field=annotations.get(key))!=null){
                    field.setAccessible(true);
                    try {
                        field.set(null,value);
                    } catch (IllegalAccessException e) {
                        log.warn("注入配置失败 '{}' '{}'",path+"."+key,e.getMessage());
                    }
                }
            }
        });
    }
}
