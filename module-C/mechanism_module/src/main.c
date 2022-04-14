//
// Created by lpyyxy on 2022/3/7.
//

#include "main.h"

int main(void)
{
    //初始化
    Config* config = load_config("config.cfg");
    if (!initialize(hash_string("mechanism_module"),
        find_config(config, "address"),
        find_config(config, "private_key"))) {
        return 0;
    }
    initialize_decl_cond();
    //初始化数据库
    //用户储存表 UID 组织名 状态
    init_store_data("MechanismStoreData_User", organization_store_date_user_decl);
    //组织储存表 组织名 管理员 基本信息（创建时间 创建者UID）
    init_store_data("MechanismStoreData_Organization", organization_store_date_organization_decl);
    //获取客户端发送给服务器的信息
    //获取增添组织消息
    accept_message(message_submission_add_organization_cond,
                    organization_information_add_organization_decl,
                    sizeof(AcceptMessageAddOrganization),
                    message_submission_add_organization);

    //获取增添用户消息
    accept_message(message_submission_add_user_cond,
                    organization_information_add_user_decl,
                    sizeof(AcceptMessageAddUser),
                    message_submission_add_user);
}

//用户发送添加组织时向服务器发送的消息 用户UID 消息（组织名）
void message_submission_add_organization(long long session_id, void* message) {
    //描述要找储存表里面这个组织的名字
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
        existmessage.ID = ;//不知
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
    add_organization_information.OrganizationManageUID = ;//管理员UID还是空的
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

//用户发送添加用户时向服务器发送的消息 用户UID 消息（组织名 组织ID）
void message_submission_add_user(long long session_id, void* message) {
    //描述要找储存表里面这个组织的名字
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
        existmessage.ID = ;//不知
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
    add_user_information.State = ;//判断
}

void initialize_decl_cond() {//初始化数据
    //UID
    Decl* UID = normal_declaration("UID", LONG);
    //OrganizationNmae 组织名
    Decl* OrganizationName = normal_declaration("OrganizationNmae", BYTE);
    OrganizationName->is_dynamic_array = ture;//可变数组
    //OrganizationID 组织ID
    Decl* OrganizationID = normal_declaration("OrganizationID", LONG);
    //管理员UID
    Decl* OrganizationManageUID = normal_declaration("OrganizationManageUID", LONG);
    //创建者UID
    Decl* CreationUID = normal_declaration("CreationUID", LONG);
    //返回信息中的返回值
    Decl* Response = normal_declaration("Response", BOOLEAN);
    //用户加入组织状态(用于判断是否已经加入，或者管理员是否同意加入）
    Decl* State = normal_declaration("State", BYTE);
    //组织基本信息（创建时间 创建者UID）
    Decl* BasicInformation = normal_declaration("BasicInformation", OBJ);

    //添加用户结构体 用户UID 组织名 组织ID 状态(用于判断是否已经加入，或者管理员是否同意加入）
    organization_store_date_user_decl = object_declaration("OrganizationStoreDate", 4, UID, OrganizationName, OrganizationID, State);
    //添加组织结构体 组织ID 组织名 组织管理员UID 基本信息（创建时间 创建者UID）
    organization_store_date_organization_decl = object_declaration("OrganizationInformation", 4, OrganizationID, OrganizationName, OrganizationManageUID, BasicInformation);
    //获取增添组织的结构体（客户端发送给服务器）创建者UID 组织名字
    organization_information_add_organization_decl = object_declaration("AcceptMessageAddOrganization", 2, CreationUID, OrganizationName);
    //获取增添用户的结构体（客户端发送给服务器）用户UID 组织名 组织ID
    organization_information_add_user_decl = object_declaration("AcceptMessageAddUser", 3, UID, OrganizationName, OrganizationID);
    //组织已经存在时返回给客户端的消息结构体
    organization_exist_return_decl = object_declaration("OrganizationExist", 3,UID， );
    //用户发送添加组织指令
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

    //用户发送添加用户指令
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