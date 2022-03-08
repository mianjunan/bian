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

typedef enum BaseType
{
    BYTE = 0, //char
    SHORT = 1, //short
    INT = 2, //int
    LONG = 3, //long long
    FLOAT = 4, //float
    DOUBLE = 5, //double
    BOOLEAN =6, //bool
    OBJ = 7 //struct
}BaseType;

typedef enum LogicOperate
{
    NONE = 0,
    AND = 1,
    OR = 2
}LogicOperate;

typedef enum WhereOperate
{
    EQUAL = 0,
    NOT_EQUAL = 1,
    GREATER_THAN = 2,
    LESS_THAN = 3
}WhereOperate;

typedef enum LogLevel
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR =4
}LogLevel;

typedef enum BaseEventID
{
    MESSAGE_SEND=0,
    MESSAGE_ACCEPT=1,
    STORE_DATA_ADD=2,
    STORE_DATA_REMOVE=3,
    STORE_DATA_UPDATE=4,
    STORE_DATA_GET=5,
}BaseEventID;

typedef struct Declaration
{
    const char *name;
    BaseType type;
    unsigned int Array_size;
    bool is_dynamic_array;
    bool is_address;
    struct Declaration *child_node;
    int child_size;
    unsigned char max_length;
}Decl;

typedef struct Condition
{
    LogicOperate operate;
    char *target;
    WhereOperate where_operate;
    BaseType type;
    int value_size;
    char *value;
    struct Condition *successor;
}Cond;

typedef struct MessageSendDetails
{
    const char *id;
}MessageSendDetails;

typedef struct MessageAcceptDetails
{
    const char *id;
}MessageAcceptDetails;

typedef struct StoreDataAddDetails
{
    const unsigned long long row;
}StoreDataAddDetails;

typedef struct StoreDataRemoveDetails
{
    const unsigned long long row;
}StoreDataRemoveDetails;

typedef struct StoreDataUpdateDetails
{
    const unsigned long long row;
}StoreDataUpdateDetails;

typedef struct StoreDataGetDetails
{
    const unsigned long long row;
}StoreDataGetDetails;

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
 * @Description: 初始化模块，必须在所有函数之前调用。其中moduleId由模块创建者生成，应具有唯一性，private_key为
 *
 *
*/
extern void initialize(long long module_id,unsigned char private_key[64]);
//接收消息并调用回调函数
extern void accept_message(Cond *condition,Decl declaration,unsigned int size,int (*messageHandle)(long long,void*));

extern void send_message(long long sessionId,Decl declaration,void *message);

extern void init_store_data(const char *name,Decl declaration);

extern void add_store_data(const char *name,void *data);

extern void remove_store_data(const char *name,Cond *condition);

extern void update_store_data(const char *name,Decl declaration,void *data);

extern void get_store_data(const char *name,Cond *condition,Decl declaration,unsigned int size);

extern void log(LogLevel log_level,const char *log,...);

extern void scheduled_tasks(long long time,int (*scheduled_tasks)());

extern void monitor(BaseEventID base_event_id,int (*message_handle)(long long,Details));

extern void block_monitor(BaseEventID base_event_id,int (*message_handle)(long long,Details));

extern void init_config(const char *name,Decl decl);

extern void put_config(const char *name,Decl decl,void *config);

extern void get_config(const char *name,Decl decl,int (get_config)(void *));

extern void accept_module_message(char *id,int (*accept_module_message)(void *));

extern void send_module_message(long long target_module_id,char *id,void *data,unsigned int data_size);

extern void get_module_data(long long target_module_id,char *id,void *data,unsigned int data_size,int (*accept_module_message)(void *));

extern void put_cache_data(unsigned int data_size,void *data,int (*return_data_address)(unsigned long long));

extern void get_cache_data(unsigned long long address,int (*return_data)(unsigned int,void *));

extern void sleep();

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

#endif