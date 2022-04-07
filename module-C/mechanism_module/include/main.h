//
// Created by lpyyxy on 2022/3/7.
//

#ifndef _USER_MODULE_MAIN_H_
#define _USER_MODULE_MAIN_H_

#include <glowworm.h>
#include <stdio.h>
#include "util.h"
#include "shared.h"
#include <string.h>
//UID 组织ID 状态
typedef struct OrganizationStoreDate {
	long long UID;
	long long OrganizationID;
	char state;
}OrganizationStoreDate;

//组织ID 组织名字 管理员UID 基本信息(创建时间 创建者 )
typedef struct OrganizationInformation {
	long long OrganizationID;
	unsigned int OrganizationNameSize;
	char* OrganizationName = (char*)malloc((char)*OrganizationNameSize);
	long long OrganizationManageUID;
	struct BasicInformation;
}OrganizationInformation;

static Decl* organization_store_date_decl_1;//组织存储表的名字
static Decl* organization_store_date_decl_2;//组织存储表的名字
static Decl* organization_information_decl;//获取消息的结构
static Cond* message_submission_cond_1;//用户发送添加组织指令
static Cond* message_submission_cond_2;//用户发送添加用户指令

void initialize_decl_cond();//初始化数据
void message_submission_fun();//判断是否存在组织
void message_submission_fun(long long session_id, void* message);//回调函数，返回信息给用户

typedef struct BasicInformation {
	long long CreationTime;
	long long CreationUID;
}BasicInformation;
#endif
