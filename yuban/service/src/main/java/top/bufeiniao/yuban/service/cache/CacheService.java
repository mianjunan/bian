package top.bufeiniao.yuban.service.cache;

import top.bufeiniao.yuban.model.data.Condition;
import top.bufeiniao.yuban.model.data.Declaration;

import java.util.concurrent.ConcurrentHashMap;

public class CacheService {
    private final static ConcurrentHashMap<Long, Declaration> declarationCache=new ConcurrentHashMap<>();
    public static void putDeclarationCache(long id,Declaration declaration){
        declarationCache.put(id,declaration);
    }
    public static Declaration getDeclarationCache(long id){
        return declarationCache.get(id);
    }
    public static Declaration removeDeclarationCache(long id){
        return declarationCache.remove(id);
    }
}
