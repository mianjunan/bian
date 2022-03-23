//
// Created by lpyyxy on 2022/3/10.
//

#ifndef _UTIL_H
#define _UTIL_H

#include "glowworm.h"
#include <sys/time.h>
#include <time.h>

typedef struct Config
{
    char *key;
    char *value;
    struct Config *config;
}Config;

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
    Decl* temp=malloc(sizeof(Decl));
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
            .array_size=0,
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
    Decl* temp= malloc(sizeof(Decl));
    Decl* child_node= malloc(child_size * sizeof(Decl));
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
            .array_size=0,
            .is_dynamic_array=false,
            .is_address=false,
            .child_node=child_node,
            .child_size=child_size,
            .max_length=max_length
    };
    memcpy(temp,&decl, sizeof(Decl));
    return temp;
}



extern char get_effective_length(const unsigned int data)
{
    for(char i=4;i>0;i--){
        if(data>>((i-1)<<3)&0xff){
            return i;
        }
    }
    return 0;
}

extern long long hash_array(void *data , unsigned long size){
    long long h = 0;
    for (unsigned long i=size-1;i>=0;i--) {
        h = 31 * h + (((char *)data)[i] & 0xff);
    }
    return h;
}

extern long long hash_string(const char* data) {
    return hash_array((void *)data, strlen(data));
}

extern char* find_config(Config *config,const char *key)
{
    Config *temp=config;
    for(;;)
    {
        if(strcmp(temp->key,key) == 0)
        {
            return temp->value;
        }
        else
        {
            if(temp->config== NULL)
            {
                return NULL;
            }
            else
            {
                temp=config->config;
            }
        }
    }
}

extern void remove_spaces(char *str)
{
    char *str_c=str;
    int i,j=0;
    for(i=0;str[i]!='\0';i++)
    {
        if(str[i]!=' ')
            str_c[j++]=str[i];
    }
    str_c[j]='\0';
    str=str_c;
}


extern Config* load_config(const char *config_path)
{
    FILE *fptr;
    if ((fptr = fopen(config_path, "r")) == NULL)
    {
        return NULL;
    }
    char c[1000];
    char *p=c;
    int ch;

    static Config *config= NULL;

    while ((ch = fgetc(fptr)) != EOF)// 读取到文件末尾或读取失败时返回 EOF
    {
        if (ch == '\n'){
            *p='\0';
            remove_spaces(c);
            if(c[0]=='#'|c[0]=='\0'){
                break;
            }
            Config cf={
                    .key=strtok(c, ":"),
                    .value=strtok(NULL, ":"),
                    .config=config
            };
            config=&cf;
            p=c;
        }else{
            *(p++)=(char)ch;
        }
    }
    return config;
}

long long get_timestamp(void)//获取时间戳函数
{
    long long tmp;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tmp = tv.tv_sec;
    tmp = tmp * 1000;
    tmp = tmp + (tv.tv_usec / 1000);

    return tmp;
}

typedef struct String
{
    unsigned int value_length;
    char *value;
}String;

static Decl string;

String* to_string(const char *value){
    void *temp= malloc(strlen(value));
    memcpy(temp,value, sizeof(temp));
    String *str= malloc(sizeof(String));
    str->value=temp;
    str->value_length= sizeof(temp);
    return str;
}

long long string_hash(String *value){
    return hash_array(value->value,value->value_length);
}

bool string_equals(String *tar_key,String *src_key){
    for(unsigned int i=(src_key->value_length-1);i>=0;i--){
        if(tar_key->value[i]!=src_key->value[i]){
            return false;
        }
    }
    return true;
}

typedef struct MapDatum
{
    struct MapDatum* left;
    struct MapDatum* right;
    void *key;
    void *value;
}MapDatum;

typedef struct Map
{
    long long (*hash)(void *key);
    bool (*equals)(void *tar_key,void *src_key);
    MapDatum *map_datum;
}Map;


extern void* map_get(Map *map,void *key){
    MapDatum *temp=map->map_datum;
    for(;;){
        if(!temp){
            return NULL;
        }else if(map->hash(key)==map->hash(temp->key)&&map->equals(key,temp->key)){
            return temp->value;
        }else{
            if(map->hash(temp->key)<=map->hash(key)){
                temp=temp->right;
            }else if(map->hash(temp->key)>map->hash(key)){
                temp=temp->left;
            }
        }
    }
}


bool map_exist(Map *map,void *key){
    return map_get(map,key)!=NULL;
}

bool map_put(Map *map,void *key,void *value){
    if(map_exist(map,key)){
        return false;
    }else{
        MapDatum *temp;
        MapDatum *mapDatum= malloc(sizeof(MapDatum));
        mapDatum->key=key;
        mapDatum->value=value;
        for (;;){
            if(map->hash(temp->key)<=map->hash(key)){
                if(!temp->right){
                    temp->right=mapDatum;
                    return true;
                }else{
                    temp=temp->right;
                }
            }else if(map->hash(temp->key)>map->hash(key)){
                if(!temp->left){
                    temp->left=mapDatum;
                    return true;
                }else{
                    temp=temp->left;
                }
            }
        }
    }
}


bool map_remove(Map *map,void *key,void *value){
    if(map_exist(map,key)){
        return false;
    }else{
        MapDatum *temp;
        MapDatum *temp_left;
        MapDatum *temp_right;
        for (;;){
            if(map->hash(temp->key)<=map->hash(key)){
                if(map->hash(key)==map->hash(temp->right->key)&&map->equals(key,temp->right->key)){
                    temp_left=temp->right->left;
                    temp_right=temp->right->right;
                    free(temp->right);
                    if(temp_right!=NULL) {
                        temp->right = temp_right;
                    } else{
                        temp->right = temp_left;
                        return true;
                    }
                    for(;;){
                        if(temp_right->left!=NULL){
                            temp_right=temp_right->left;
                        } else{
                            temp_right->left=temp_left;
                            return true;
                        }
                    }
                }else{
                    temp=temp->right;
                }
            }else {
                if(map->hash(key)==map->hash(temp->left->key)&&map->equals(key,temp->left->key)){
                    temp_left=temp->left->left;
                    temp_right=temp->left->right;
                    free(temp->left);
                    if(temp_right!=NULL) {
                        temp->left = temp_left;
                    } else{
                        temp->left = temp_right;
                        return true;
                    }
                    for(;;){
                        if(temp_left->right!=NULL){
                            temp_left=temp_left->right;
                        } else{
                            temp_left->right=temp_right;
                            return true;
                        }
                    }
                }else{
                    temp=temp->left;
                }
            }
        }
    }
}

#endif