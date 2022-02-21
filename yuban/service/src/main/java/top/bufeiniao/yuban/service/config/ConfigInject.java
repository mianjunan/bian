package top.bufeiniao.yuban.service.config;

import java.lang.annotation.*;

@Documented
@Target(ElementType.FIELD)
@Retention(RetentionPolicy.RUNTIME)
public @interface ConfigInject {
    String id() default "unknown";
}
