//
// Created by lpyyxy on 2022/3/7.
//

#include "main.h"

int main(void)
{
    //��ʼ��
    Config* config = load_config("config.cfg");
    if (!initialize(hash_string("mechanism_module"),
        find_config(config, "address"),
        find_config(config, "private_key"))) {
        return 0;
    }
    initialize_decl_cond();
    //��ʼ�����ݿ�
    //�û������ UID ��֯�� ״̬
    init_store_data("MechanismStoreData_User", organization_store_date_user_decl);
    //��֯����� ��֯�� ����Ա ������Ϣ������ʱ�� ������UID��
    init_store_data("MechanismStoreData_Organization", organization_store_date_organization_decl);
    //��ȡ�ͻ��˷��͸�����������Ϣ
    //��ȡ������֯��Ϣ
    accept_message(message_submission_add_organization_cond,
                    organization_information_add_organization_decl,
                    sizeof(AcceptMessageAddOrganization),
                    message_submission_add_organization);

    //��ȡ�����û���Ϣ
    accept_message(message_submission_add_user_cond,
                    organization_information_add_user_decl,
                    sizeof(AcceptMessageAddUser),
                    message_submission_add_user);
}

//�û����������֯ʱ����������͵���Ϣ �û�UID ��Ϣ����֯����
void message_submission_add_organization(long long session_id, void* message) {
    //����Ҫ�Ҵ�������������֯������
    Cond store_data_organization_name = {
         .operate = NONE,
         .target = "organization_name",
         .where_operate = EQUAL,
         .type = BYTE,
         .value = ((OrganizationInformation*)message)->OrganizationName,
         .successor = NULL

    };
    StoreData storedata_Organization_Name;
    if ((storedata_Organization_Name = get_store_data("MechanismStoreData_Organization",
        &store_data_organization_name,
        organization_information_add_organization_decl,
        sizeof(OrganizationInformation))).data_size) {
        OrganizationExist existmessage;
        existmessage.ID = ;//��֪
        existmessage.Type = BOOLEAN;
        existmessage.Response = false;
        send_message(session_id, organization_exist_return_decl, existmessage);
        free((OrganizationInformation*)storedata_Organization_Name.data[0]);
        return;
    };
    OrganizationInformation add_organization_information;
    add_organization_information.OrganizationID = 
        hash_array(session_id,sizeof(long long))
        &hash_array(((AcceptMessageAddOrganization*)message)->OrganizationName, ((AcceptMessageAddOrganization*)message)->OrganizationNameSize)
        &get_timestamp();
    add_organization_information.OrganizationNameSize = sizeof((AcceptMessageAddOrganization*)message)->OrganizationName);
    add_organization_information.OrganizationName = (AcceptMessageAddOrganization*)message)->OrganizationName;
    add_organization_information.OrganizationManageUID = ;//����ԱUID���ǿյ�
    add_organization_information.BasicInformation->CreationTime = get_timestamp();
    add_organization_information.BasicInformation->CreationUID = session_id;
    bool cnt = false;
    do {
        StoreDate storedate_OrganizationID;
        add_organization_information.OrganizationID++;
        Cond date_OrganizationID = {
                .operate = NONE,
                .target = "OrganizationID",
                .where_operate = EQUAL,
                .type = LONG,
                .value = long_to_byte_array(add_organization_information.OrganizationID),
                .successor = NULL
        };
        cnt = ((storedate_OrganizationID = get_store_data("OrganizationInformation", &date_OrganizationID, organization_store_date_organization_decl, sizeof(OrganizationInformation))).data_size);
        free((OrganizationInformation*)storedata_Organization_Name.date[0]);
    } while (cnt);
    add_store_data("OrganizationInformation", &add_organization_information);

    free((AcceptMessageAddOrganization*)storedata_Organization_Name.date[0]);
};

//�û���������û�ʱ����������͵���Ϣ �û�UID ��Ϣ����֯�� ��֯ID��
void message_submission_add_user(long long session_id, void* message) {
    //����Ҫ�Ҵ�������������֯������
    Cond store_data_organization_name = {
         .operate = NONE,
         .target = "organization_name",
         .where_operate = EQUAL,
         .type = BYTE,
         .value = ((OrganizationInformation*)message)->OrganizationName,
         .successor = NULL

    };
    StoreData storedata_Organization_Name;
    if (!(storedata_Organization_Name = get_store_data("MechanismStoreData_Organization",
        &store_data_organization_name,
        organization_information_add_organization_decl,
        sizeof(OrganizationInformation))).data_size) {
        OrganizationExist existmessage;
        existmessage.ID = ;//��֪
        existmessage.Type = BOOLEAN;
        existmessage.Response = false;
        send_message(session_id, organization_exist_return_decl, existmessage);
        free((OrganizationInformation*)storedata_Organization_Name.data[0]);
        return;
    };
    OrganizationStoreDate add_user_information;
    add_user_information.UID = session_id;
    add_user_information.OrganizationName = ((AcceptMessageAddUser*)message)->OrganizationName;
    add_user_information.OrganizationID = ((AcceptMessageAddUser*)message)->OrganizationID;
    add_user_information.State = ;//�ж�
}

void initialize_decl_cond() {//��ʼ������
    //UID
    Decl* UID = normal_declaration("UID", LONG);
    //OrganizationNmae ��֯��
    Decl* OrganizationName = normal_declaration("OrganizationNmae", BYTE);
    OrganizationName->is_dynamic_array = ture;//�ɱ�����
    //OrganizationID ��֯ID
    Decl* OrganizationID = normal_declaration("OrganizationID", LONG);
    //����ԱUID
    Decl* OrganizationManageUID = normal_declaration("OrganizationManageUID", LONG);
    //������UID
    Decl* CreationUID = normal_declaration("CreationUID", LONG);
    //������Ϣ�еķ���ֵ
    Decl* Response = normal_declaration("Response", BOOLEAN);
    //�û�������֯״̬(�����ж��Ƿ��Ѿ����룬���߹���Ա�Ƿ�ͬ����룩
    Decl* State = normal_declaration("State", BYTE);
    //��֯������Ϣ������ʱ�� ������UID��
    Decl* BasicInformation = normal_declaration("BasicInformation", OBJ);

    //����û��ṹ�� �û�UID ��֯�� ��֯ID ״̬(�����ж��Ƿ��Ѿ����룬���߹���Ա�Ƿ�ͬ����룩
    organization_store_date_user_decl = object_declaration("OrganizationStoreDate", 4, UID, OrganizationName, OrganizationID, State);
    //�����֯�ṹ�� ��֯ID ��֯�� ��֯����ԱUID ������Ϣ������ʱ�� ������UID��
    organization_store_date_organization_decl = object_declaration("OrganizationInformation", 4, OrganizationID, OrganizationName, OrganizationManageUID, BasicInformation);
    //��ȡ������֯�Ľṹ�壨�ͻ��˷��͸���������������UID ��֯����
    organization_information_add_organization_decl = object_declaration("AcceptMessageAddOrganization", 2, CreationUID, OrganizationName);
    //��ȡ�����û��Ľṹ�壨�ͻ��˷��͸����������û�UID ��֯�� ��֯ID
    organization_information_add_user_decl = object_declaration("AcceptMessageAddUser", 3, UID, OrganizationName, OrganizationID);
    //��֯�Ѿ�����ʱ���ظ��ͻ��˵���Ϣ�ṹ��
    organization_exist_return_decl = object_declaration("OrganizationExist", 3,UID�� );
    //�û����������ָ֯��
    Cond message_add_organization_2 = {
            .operate = NONE,
            .target = "type",
            .where_operate = EQUAL,
            .type = OBJ,
            .value = "add_organization",
            .successor = NULL
    };
    Cond message_add_organization_1 = {
            .operate = AND,
            .target = "id",
            .where_operate = EQUAL,
            .type = OBJ
            .value = "mechanism",
            .successor = &add_message_2;

    };
    message_add_organization_cond = &message_add_organization_1;

    //�û���������û�ָ��
    Cond message_add_user_2 = {
            .operate = NONE,
            .target = "type",
            .where_operate = EQUAL,
            .type = OBJ,
            .value = "add_user",
            .successor = NULL
    };
    Cond message_add_user_1 = {
            .operate = AND,
            .target = "id",
            .where_operate = EQUAL,
            .type = OBJ
            .value = "mechanism",
            .successor = &message_add_user_2;
    };
    message_add_user_cond = &message_add_user_1;
}