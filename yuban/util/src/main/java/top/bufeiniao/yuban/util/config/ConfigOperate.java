package top.bufeiniao.yuban.util.config;

import org.yaml.snakeyaml.Yaml;
import top.bufeiniao.yuban.model.work.Task;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

/**
 * @author lpyyxy
 * @since 0.0.0.1-alpha
 * @version 1.0
 * <p>提供yml文件的操作
 */
public class ConfigOperate {
    private static final Yaml YAML_INSTANCE=new Yaml();
    private final Map<?,?> INITIAL_DATA;
    private final Map<Object,Object> UPDATE_DATA=new HashMap<>();

    /**
     * 初始化函数
     * @param yaml 用文本初始化
     */
    public ConfigOperate(String yaml){
        this.INITIAL_DATA =YAML_INSTANCE.load(yaml);
    }

    /**
     * 初始化函数
     * @param io 用输入流初始化
     */
    public ConfigOperate(InputStream io){
        this.INITIAL_DATA =YAML_INSTANCE.load(io);
    }

    /**
     * 添加更新配置
     * @param yaml 添加文本类型数据
     */
    public void updateConfig(String yaml){
        mergeMap(UPDATE_DATA,YAML_INSTANCE.load(yaml));
    }
    /**
     * 添加更新配置
     * @param io 添加输入流类型数据
     */
    public void updateConfig(InputStream io){
        mergeMap(UPDATE_DATA,YAML_INSTANCE.load(io));
    }

    /**
     * 添加更新配置
     * @param file 添加文件
     */
    public void updateConfig(File file, Task<IOException> exceptionTreat){
        try {
            if(!file.createNewFile()){
                mergeMap(UPDATE_DATA,YAML_INSTANCE.load(new FileInputStream(file)));
            }
        } catch (IOException ie) {
            exceptionTreat.main(ie);
        }
    }

    /**
     * 获取值
     * @param path 路径
     * @return 值
     */
    public ConfigElement<?> getData(String path){
        return getData(false,path);
    }

    /**
     * 获取值
     * @param initialData 是否使用初始化数据
     * @param path 路径
     * @return 值
     */
    public ConfigElement<?> getData(boolean initialData,String path) {
        String[] paths=path.split("\\.");
        //初始化TempData
        Object TempData= initialData?
                INITIAL_DATA:
                UPDATE_DATA;
        for(String p:paths) {
            if(TempData==null){
                //如果初始化TempData是updateData而且updateData找不到，就去initialData查找
                return initialData?
                        null:
                        getData(true, path);
            }
            TempData=((HashMap<?,?>)TempData).get(p);
        }
        return (!initialData)&&TempData==null?getData(true, path):new ConfigElement<>(TempData);
    }

    /**
     * 更新值，应配合store方法使用
     * @param path 路径
     * @param element 值
     */
    public void putData(String path,ConfigElement<?> element){
        putData(UPDATE_DATA,path,element);
    }

    /**
     * 保存更新
     * @param outputStream 目标文件输出流
     */
    public void store(OutputStream outputStream) {
        new Yaml().dump(UPDATE_DATA,new OutputStreamWriter(outputStream, StandardCharsets.UTF_8));
    }

    @Override
    public String toString(){
        return INITIAL_DATA.toString();
    }

    //修 @SuppressWarnings("unchecked")改map
    @SuppressWarnings("unchecked")
    private void putData(Map<Object,Object> data,String path,ConfigElement<?> element){
        //整体采用递归方法
        String[] paths=path.split("\\.",2);
        //判断是否到了结尾
        if(paths.length==2){
            //如果没有到结尾，判断原位置是否为map，如果不是替换成map
            data.merge(paths[0],new HashMap<>(),(oldValue,newValue)-> {
                if(oldValue instanceof Map<?,?>){
                    return oldValue;
                }else{
                    return newValue;
                }
            });
        }else{
            //如果到了结尾，添加或更改值
            data.put(paths[0],element.datum());
            return;
        }
        putData((Map<Object,Object>) data.get(paths[0])
                ,paths[1]
                ,element);
    }
    //合并两个map
    @SuppressWarnings("unchecked")
    private void mergeMap(Map<Object,Object> oldMap,Map<?,?> newMap){
        newMap.forEach((key,value)->{
            if(value instanceof Map<?,?>&&
                    oldMap.get(key) instanceof Map<?,?>&&
                    oldMap.containsKey(key)){
                mergeMap((Map<Object, Object>) oldMap.get(key), (Map<?, ?>) value);
                return;
            }
            UPDATE_DATA.merge(key,value,(oldValue,newValue)-> newValue);
        });
    }
}