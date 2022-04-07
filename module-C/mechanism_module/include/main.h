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
//UID ��֯ID ״̬
typedef struct OrganizationStoreDate {
	long long UID;
	long long OrganizationID;
	char state;
}OrganizationStoreDate;

//��֯ID ��֯���� ����ԱUID ������Ϣ(����ʱ�� ������ )
typedef struct OrganizationInformation {
	long long OrganizationID;
	unsigned int OrganizationNameSize;
	char* OrganizationName = (char*)malloc((char)*OrganizationNameSize);
	long long OrganizationManageUID;
	struct BasicInformation;
}OrganizationInformation;

static Decl* organization_store_date_decl_1;//��֯�洢�������
static Decl* organization_store_date_decl_2;//��֯�洢�������
static Decl* organization_information_decl;//��ȡ��Ϣ�Ľṹ
static Cond* message_submission_cond_1;//�û����������ָ֯��
static Cond* message_submission_cond_2;//�û���������û�ָ��

void initialize_decl_cond();//��ʼ������
void message_submission_fun();//�ж��Ƿ������֯
void message_submission_fun(long long session_id, void* message);//�ص�������������Ϣ���û�

typedef struct BasicInformation {
	long long CreationTime;
	long long CreationUID;
}BasicInformation;
#endif
