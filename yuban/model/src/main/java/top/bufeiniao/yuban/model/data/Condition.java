package top.bufeiniao.yuban.model.data;

import java.util.List;

public record Condition(long id, List<SingleCondition<?>> list) {

    public record SingleCondition<T>(LogicOperate operate,
                                     String target,
                                     WhereOperate where_operate,
                                     T value) {
        public enum LogicOperate {
            NONE,
            AND,
            OR
        }

        public enum WhereOperate {
            EQUAL, // ==
            NOT_EQUAL, // !=
            GREATER_THAN, // >
            LESS_THAN // <
        }
    }
}
