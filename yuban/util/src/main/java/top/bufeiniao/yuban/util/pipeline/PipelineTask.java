package top.bufeiniao.yuban.util.pipeline;

public interface PipelineTask<T> {
    void preTreat(T data);
    void treat(T data);
    void exception(T data,Exception e);
}
