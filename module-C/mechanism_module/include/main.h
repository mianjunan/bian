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

//�û������ṹ�� UID ��֯�� ��֯ID ״̬
typedef struct OrganizationStoreDate {
	long long UID;
	long long OrganizationID;
	char State;
}OrganizationStoreDate;

//��֯�����ṹ�� ��֯ID ��֯���� ����ԱUID ������Ϣ(����ʱ�� ������UID)
typedef struct OrganizationInformation {
	long long OrganizationID;
	unsigned int OrganizationNameSize;
	void* OrganizationName;
	long long OrganizationManageUID;
	struct BasicInformation;
}OrganizationInformation;

//��֯�Ļ�����Ϣ������ʱ�� ������UID��
typedef struct BasicInformation {
	long long CreationTime;
	long long CreationUID;
}BasicInformation;

//��ȡ������֯�Ľṹ�壨�ͻ��˷��͸���������������UID ��֯���� 
typedef struct AcceptMessageAddOrganization {
	long long CreaterUID;
	unsigned int OrganizationNameSize;
	void* OrganizationName;
}AcceptMessageAddOrganization;

//��ȡ�����û��Ľṹ�壨�ͻ��˷��͸����������û�UID ��֯���� ��֯ID
typedef struct AcceptMessageAddUser {
	long long UserUID;
	unsigned int OrganizationNameSize;
	void* OrganizationName;
	long long OrganizationID;
}AcceptMessageAddUser;

//��֯�Ѿ�����ʱ���ظ��ͻ��˵���Ϣ�ṹ�� �û�UID ������Ϣ���ͣ�bool�� ����ֵ��false��
typedef struct OrganizationExist {
	String ID;
	String Type;
	bool Response;
}OrganizationExist;

//�û��ṹ�� 
״̬�������ж��Ƿ��Ѿ����룬���߹���Ա�Ƿ�ͬ����룩
static Decl* organization_store_date_user_decl;
//��֯�ṹ�� ��֯ID ��֯�� ��֯����ԱUID ������Ϣ������ʱ�� ������UID��
static Decl* organization_store_date_organization_decl;
//��ȡ������֯��Ϣ�Ľṹ�壨�ͻ��˷��͸���������
static Decl* organization_information_add_organization_decl;
//��ȡ�����û���Ϣ�Ľṹ�壨�ͻ��˷��͸���������
static Decl* organization_information_add_user_decl;
//��֯�Ѿ�����ʱ���ظ��ͻ��˵���Ϣ�ṹ��
struct Decl* organization_exist_return_decl;

//�û����������ָ֯��
static Cond* message_submission_add_organization_cond;
//�û���������û�ָ��
static Cond* message_submission_add_user_cond;

//��ʼ������
void initialize_decl_cond();

//������֯����
void message_submission_add_organization(long long session_id, void* message);
//����û�����
void message_submission_add_user(long long session_id, void* message);

#endif
