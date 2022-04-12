package top.bufeiniao.yuban.model.data;

import java.util.List;
import java.util.Map;

public record Declaration(long id, List<SingleDeclaration> list) {

    public record SingleDeclaration(String name,
                                           BaseType baseType,
                                           int arraySize,
                                           boolean isDynamicArray,
                                           boolean isAddress,
                                           SingleDeclaration parentNode) {
    }
    public enum BaseType {
        BYTE,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
        BOOLEAN,
        OBJECT
    }
}

