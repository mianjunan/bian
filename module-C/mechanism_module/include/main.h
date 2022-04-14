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

//用户储存表结构体 UID 组织名 组织ID 状态
typedef struct OrganizationStoreDate {
	long long UID;
	long long OrganizationID;
	char State;
}OrganizationStoreDate;

//组织储存表结构体 组织ID 组织名字 管理员UID 基本信息(创建时间 创建者UID)
typedef struct OrganizationInformation {
	long long OrganizationID;
	unsigned int OrganizationNameSize;
	void* OrganizationName;
	long long OrganizationManageUID;
	struct BasicInformation;
}OrganizationInformation;

//组织的基本信息（创建时间 创建者UID）
typedef struct BasicInformation {
	long long CreationTime;
	long long CreationUID;
}BasicInformation;

//获取增添组织的结构体（客户端发送给服务器）创建者UID 组织名字 
typedef struct AcceptMessageAddOrganization {
	long long CreaterUID;
	unsigned int OrganizationNameSize;
	void* OrganizationName;
}AcceptMessageAddOrganization;

//获取增添用户的结构体（客户端发送给服务器）用户UID 组织名字 组织ID
typedef struct AcceptMessageAddUser {
	long long UserUID;
	unsigned int OrganizationNameSize;
	void* OrganizationName;
	long long OrganizationID;
}AcceptMessageAddUser;

//组织已经存在时返回给客户端的消息结构体 用户UID 返回信息类型（bool） 返回值（false）
typedef struct OrganizationExist {
	String ID;
	String Type;
	bool Response;
}OrganizationExist;

//用户结构体 
状态（用于判断是否已经加入，或者管理员是否同意加入）
static Decl* organization_store_date_user_decl;
//组织结构体 组织ID 组织名 组织管理员UID 基本信息（创建时间 创建者UID）
static Decl* organization_store_date_organization_decl;
//获取增添组织信息的结构体（客户端发送给服务器）
static Decl* organization_information_add_organization_decl;
//获取增添用户信息的结构体（客户端发送给服务器）
static Decl* organization_information_add_user_decl;
//组织已经存在时返回给客户端的消息结构体
struct Decl* organization_exist_return_decl;

//用户发送添加组织指令
static Cond* message_submission_add_organization_cond;
//用户发送添加用户指令
static Cond* message_submission_add_user_cond;

//初始化数据
void initialize_decl_cond();

//增添组织函数
void message_submission_add_organization(long long session_id, void* message);
//添加用户函数
void message_submission_add_user(long long session_id, void* message);

#endif
