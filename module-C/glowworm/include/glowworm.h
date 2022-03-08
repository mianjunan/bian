/**
 * @File_name: glowworm.h
 * @Author: lpyyxy
 * @Version: 1.0
 * @Date: 2022-03-08
 * @Description: 通讯模块接口。
*/

#ifndef _GLOWWORM_H_
#define _GLOWWORM_H_

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/**
 * @Enum_name：基本类型
 * @Description: 定义8个基本类型，在Declaration和Condition中均有使用。
*/
typedef enum BaseType
{
    BYTE = 0, // char
    SHORT = 1, // short
    INT = 2, // int
    LONG = 3, // long long
    FLOAT = 4, // float
    DOUBLE = 5, // double
    BOOLEAN =6, // bool
    OBJ = 7 // struct
}BaseType;

/**
 * @Enum_name：逻辑运算符
 * @Description: 定义3个逻辑运算符，在Condition中使用。
*/
typedef enum LogicOperate
{
    NONE = 0,
    AND = 1, // &
    OR = 2 // |
}LogicOperate;

/**
 * @Enum_name：判断符号
 * @Description: 在Condition中使用。
*/
typedef enum WhereOperate
{
    EQUAL = 0, // ==
    NOT_EQUAL = 1, // !=
    GREATER_THAN = 2, // >
    LESS_THAN = 3 // <
}WhereOperate;

/**
 * @Enum_name：日志等级
 * @Description: 日志中使用，标明日志等级，严重程度递增。
*/
typedef enum LogLevel
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR =4
}LogLevel;

/**
 * @Enum_name：基本事件类型
 * @Description: 事件方法中使用，标明对哪种事件进行监听。
*/
typedef enum BaseEventID
{
    MESSAGE_SEND=0, // 发送消息
    MESSAGE_ACCEPT=1, // 接收消息
    STORE_DATA_ADD=2, // 添加储存数据
    STORE_DATA_REMOVE=3, // 删除储存数据
    STORE_DATA_UPDATE=4, // 更新储存数据
    STORE_DATA_GET=5, // 获取储存数据
}BaseEventID;

/**
 * @Struct_name：对象描述。
 * @Description: 描述一个结构体的结构。
*/
typedef struct Declaration
{
    const char *name; // 字段名称
    BaseType type; // 字段类型
    unsigned int Array_size; // 如果为0则为普通类型，否则为数组
    bool is_dynamic_array; // 是否为可变数组
    bool is_address; //
    struct Declaration *child_node; //
    int child_size; //
    unsigned char max_length; //
}Decl;

/**
 * @Struct_name：条件语句。
 * @Description: 描述一个条件语句。
*/
typedef struct Condition
{
    LogicOperate operate; // 和下一个条件语句的逻辑运算符，如果没有用NONE代替。
    char *target; // 要判断值的名称
    WhereOperate where_operate; // 判断符号
    BaseType type; // 要判断值的类型
    char *value;  // 预期值，如果为数字请用int_to_byte_array()等转化。
    struct Condition *successor; // 下一个条件语句，如果没有用NULL代替。
}Cond;

/**
 * @Struct_name：发送信息细节
 * @Description: 如果有信息发送，会返回发送信息中的id对应值到模块。
*/
typedef struct MessageSendDetails
{
    const char *id;
}MessageSendDetails;

/**
 * @Struct_name：接收信息细节
 * @Description: 如果有信息接收，会返回接收信息中的id对应值到模块。
*/
typedef struct MessageAcceptDetails
{
    const char *id;
}MessageAcceptDetails;

/**
 * @Struct_name：添加信息到数据库细节
 * @Description: 如果有信息添加到数据库，会返回添加到的行。
*/
typedef struct StoreDataAddDetails
{
    const unsigned long long row;
}StoreDataAddDetails;

/**
 * @Struct_name：从数据库移除信息细节
 * @Description: 如果有信息从数据库移除，会返回移除的行。
*/
typedef struct StoreDataRemoveDetails
{
    const unsigned long long row;
}StoreDataRemoveDetails;

/**
 * @Struct_name：更新信息到数据库细节
 * @Description: 如果有信息更新到数据库，会返回更新的行。
*/
typedef struct StoreDataUpdateDetails
{
    const unsigned long long row;
}StoreDataUpdateDetails;

/**
 * @Struct_name：从数据库获取信息细节
 * @Description: 如果有信息从数据库获取，会返回获取的行。
*/
typedef struct StoreDataGetDetails
{
    const unsigned long long row;
}StoreDataGetDetails;

/**
 * @Struct_name：储存数据
 * @Description: 从数据库获取的数据。
*/
typedef struct StoreData
{
    void** data;//数据
    unsigned int data_size;//数据的数目
}StoreData;

/**
 * @Union_name：细节
 * @Description: 当有事件发生时，不光会返回用户id，还会返回每个事件独有的信息。
*/
typedef union Details
{
    MessageSendDetails messageSendDetails;
    MessageAcceptDetails messageAcceptDetails;
    StoreDataAddDetails storeDataAddDetails;
    StoreDataRemoveDetails storeDataRemoveDetails;
    StoreDataUpdateDetails storeDataUpdateDetails;
    StoreDataGetDetails storeDataGetDetails;
}Details;

/**
 * @Function_name：初始化
 * @Description: 初始化模块，必须在所有函数之前调用。
 * @module_id: 由模块创建者生成，应具有唯一性，可用hash函数处理模块名称后获得。
 * @private_key: 验证模块合法性，可以从文件中读取。
*/
extern void initialize(long long module_id,unsigned char private_key[64]);

/**
 * @Function_name：获取消息
 * @Description: 获取客户端发给服务端的消息。
 * @condition: 用来筛选要获取的消息。
 * @declaration: 要获取消息的结构。
 * @size：要获取消息的长度。
 * @messageHandle：获取消息处理的方式。
 * @session_id：发送消息的用户的id。
 * @message：消息。
*/
extern void accept_message(Cond *condition,Decl declaration,unsigned int size,int (*messageHandle)(long long session_id,void* message));

/**
 * @Function_name：发送消息
 * @Description: 发送消息到客户端。
 * @session_id：发送消息的用户的id。
 * @declaration: 要发送消息的结构。
 * @message：消息。
*/
extern void send_message(long long session_id,Decl declaration,void *message);

/**
 * @Function_name：初始化储存表。
 * @Description: 初始化储存表，如果已初始化，就不会有其他操作。
 * @name：要初始化储存表的名字。
 * @declaration: 要初始化储存表的结构。
*/
extern void init_store_data(const char *name,Decl declaration);

/**
 * @Function_name：添加数据到储存表。
 * @Description: 添加数据到储存表新的一行。
 * @name：要操作储存表的名字。
 * @data: 该结构体的结构为初始化储存表时用到的declaration所描述的结构。
*/
extern void add_store_data(const char *name,void *data);

/**
 * @Function_name：从储存表中移除数据。
 * @Description: 从储存表中移除满足条件的数据。
 * @name：要操作储存表的名字。
 * @condition: 移除条件。
*/
extern void remove_store_data(const char *name,Cond *condition);

/**
 * @Function_name：更新数据到储存表。
 * @Description: 添加满足条件的数据到储存表。
 * @name：要操作储存表的名字。
 * @declaration：初始化储存表时用到的declaration的子集。
 * @data: 该结构体的结构为初始化储存表时用到的declaration所描述的结构的子集。
*/
extern void update_store_data(const char *name,Decl declaration,void *data);

/**
 * @Function_name：从储存表中获取数据。
 * @Description: 从储存表中获取满足条件的数据。
 * @name：要操作储存表的名字。
 * @condition：获取条件
 * @declaration：初始化储存表时用到的declaration的子集。
 * @data_size: 数据的数量
 * @return：返回储存数据的结构体
*/
extern StoreData get_store_data(const char *name,Cond *condition,Decl declaration,unsigned int data_size);

/**
 * @Function_name：日志。
 * @Description: 写入日志到协调模块的日志系统，该方法与printf()类似，可以使用格式字符串。
 * @log_level：要写入日志的等级。
 * @log：格式控制字符串。
*/
extern void log(LogLevel log_level,const char *log,...);

/**
 * @Function_name：计划任务。
 * @Description: 指定时间后执行某一任务。
 * @timestamp：要执行的时间的时间戳。
 * @scheduled_tasks：要执行的任务。
*/
extern void scheduled_tasks(long long timestamp,int (*scheduled_tasks)());

/**
 * @Function_name：监听事件。
 * @Description: 非阻塞地监听某一事件，也就是说只是通知模块发生事件。
 * @base_event_id：要监听事件的id。
 * @message_handle：发生事件后执行的方法。
 * @session_id:触发该事件的用户的用户id。
 * @details：事件的独有信息
*/
extern void monitor(BaseEventID base_event_id,int (*message_handle)(long long session_id,Details details));

/**
 * @Function_name：监听事件。
 * @Description: 阻塞地监听某一事件，也就是说只是未经模块允许，不会继续执行。
 * @base_event_id：要监听事件的id。
 * @message_handle：发生事件后执行的方法。
 * @session_id:触发该事件的用户的用户id。
 * @details：事件的独有信息。
 * @is_execution：是否继续同意继续执行。
*/
extern void block_monitor(BaseEventID base_event_id,int (*message_handle)(long long session_id,Details details,bool *is_execution));

/**
 * @Function_name：初始化配置文件。
 * @Description: 初始化配置文件，如果已初始化，就不会有其他操作。
 * @name：要初始化配置文件的名字。
*/
extern void init_config(const char *name);

/**
 * @Function_name：添加配置。
 * @Description: 添加配置，如果已经有值会覆盖，如果没有有值会添加。
 * @name：要添加配置的配置文件的名字。
 * @decl：要添加的配置的结构
 * @config: 添加配置的数据。
*/
extern void put_config(const char *name,Decl decl,void *config);

/**
 * @Function_name：获取配置。
 * @Description: 获取配置。
 * @name：要获取配置的配置文件的名字。
 * @decl：要获取的配置的结构
 * @return: 返回储存配置的结构体。
*/
extern void* get_config(const char *name,Decl decl);

/**
 * @Function_name：接收模块消息。
 * @Description: 通过监听指定端口，接收模块消息。
 * @port：要监听的端口名称。
 * @accept_module_message：接收消息后的处理函数
 * @message: 指定的消息。
*/
extern void accept_module_message(char *port,int (*accept_module_message)(void *message));

/**
 * @Function_name：发送模块消息。
 * @Description: 对指定模块的指定端口发送模块消息。
 * @target_module_id：目标模块的模块id。
 * @port：目标模块的指定端口。
 * @data: 要发送的数据。
 * @data_size：要发送的数据的长度。
*/
extern void send_module_message(long long target_module_id,char *port,void *data,unsigned int data_size);

/**
 * @Function_name：获取模块数据。
 * @Description: 通过指定端口，获取模块数据。
 * @target_module_id：目标模块的模块id。
 * @port：目标模块的指定端口。
 * @source_data: 要获取的数据。
 * @source_data_size: 要获取的数据的长度。
 * @get_module_data：获取数据之后运行的处理函数。
 * @return_data：获取的数据。
*/
extern void get_module_data(long long target_module_id,char *port,void *source_data,unsigned int source_data_size,int (*get_module_data)(void *return_data));

/**
 * @Function_name：放置模块数据。
 * @Description: 通过指定端口，放置模块数据。
 * @port：要监听的端口名称。
 * @return_data_size: 返回数据的长度。
 * @put_module_data: 由源数据生成返回数据的函数。
 * @source_data：其他模块发送过来的数据。
 * @return_data：返回的数据。
*/
extern void put_module_data(char *port,unsigned int return_data_size,int (*put_module_data)(void *source_data,void *return_data));

/**
 * @Function_name：放置缓存数据。
 * @Description: 把数据保存到服务端。
 * @data_size：要储存数据的长度。
 * @data: 要储存的数据。
 * @return：数据的地址。
*/
extern unsigned long long put_cache_data(unsigned int data_size,void *data);

/**
 * @Function_name：获取缓存数据。
 * @Description: 获取服务端的数据。
 * @address：要获取数据的地址。
 * @data_size: 要获取数据的长度。
 * @data：数据的地址。
*/
extern void get_cache_data(unsigned long long address,int (*return_data)(unsigned int data_size,void *data));

/**
 * @Function_name：就绪
 * @Description: 最后调用的函数，让主线程进入就绪状态。
*/
extern void ready();

extern long long byte_array_to_long(const char *data){
    long long temp;
    temp=(((long long)data[0]&0xff)<<56)|
            (((long long)data[1]&0xff)<<48)|
            (((long long)data[2]&0xff)<<40)|
            (((long long)data[3]&0xff)<<32)|
            (((long long)data[4]&0xff)<<24)|
            (((long long)data[5]&0xff)<<16)|
            (((long long)data[6]&0xff)<<8)|
            ((long long)data[7]&0xff);
    return temp;
}

extern char* long_to_byte_array(long long data){
    char bytes[]={(char)(data>>56),(char)(data>>48),(char)(data>>40),(char)(data>>32),(char)(data>>24),(char)(data>>16),(char)(data>>8),(char)(data)};
    static char temp[8];
    strcpy(temp,bytes);
    return temp;
}

extern char* int_to_byte_array(int data){
    char bytes[]={(char)(data>>24),(char)(data>>16),(char)(data>>8),(char)(data)};
    static char temp[4];
    strcpy(temp,bytes);
    return temp;
}

extern Decl* normal_declaration(const char *name,BaseType type){
    Decl* temp=(malloc(sizeof(Decl)));
    char max_length;
    switch (type) {
        case BYTE:
            max_length=1;
            break;
        case SHORT:
            max_length=2;
            break;
        case INT:
            max_length=4;
            break;
        case LONG:
            max_length=8;
            break;
        case FLOAT:
            max_length=4;
            break;
        case DOUBLE:
            max_length=8;
            break;
        case BOOLEAN:
            max_length=1;
            break;
        case OBJ:
            max_length=0;
            break;
    }
    Decl decl={
            .name=name,
            .type=type,
            .Array_size=0,
            .is_dynamic_array=false,
            .is_address=false,
            .child_node=NULL,
            .child_size=0,

            .max_length=max_length
    };
    memcpy(temp,&decl, sizeof(Decl));
    return temp;
}

extern Decl* object_declaration(const char *name,int child_size,...){
    Decl* temp=malloc(sizeof(Decl));
    Decl* child_node= malloc(child_size*sizeof(Decl));
    char max_length;
    va_list arg_list;
    va_start(arg_list,child_size);
    for(int i=0;i<child_size;i++){
        child_node[i]= va_arg(arg_list,Decl);
        if(child_node[i].type==OBJ){
            max_length=max_length<sizeof(void *)?sizeof(void *):max_length;
        } else{
            max_length=max_length<child_node[i].max_length?child_node[i].max_length:max_length;
        }
    }
    va_end(arg_list);
    Decl decl={
            .name=name,
            .type=OBJ,
            .Array_size=0,
            .is_dynamic_array=false,
            .is_address=false,
            .child_node=child_node,
            .child_size=child_size,
            .max_length=max_length
    };
    memcpy(temp,&decl, sizeof(Decl));
    return temp;
}

int get_string_length(const char *str)
{
    int count = 0;
    while (*str++ != '\0')
    {
        count++;
    }
    return count;
}

char get_effective_length(const unsigned int data)
{
    for(char i=4;i>0;i--){
        if(data>>((i-1)<<3)&0xff){
            return i;
        }
    }
    return 0;
}

long long hash(char *data){
    long long h = 0;
    for (int i=get_string_length(data)-1;i>=0;i--) {
        h = 31 * h + (data[i] & 0xff);
    }
    return h;
}


#endif