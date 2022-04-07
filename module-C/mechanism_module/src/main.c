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
    //UID 组织名 状态
    init_store_data("MechanismStoreData_1", organization_store_date_decl_1);
    //组织名 管理员 基本信息（创建时间 创建者UID）
    init_store_data("MechanismStoreData_2", organization_store_date_decl_2);
    //获取客户端发送给服务器的信息
    accept_message(message_submission_cond，
                    organization_information_decl，
                    sizeof(OrganizationInformation),
                    message_submission_fun);
}
void message_submission_fun(long long session_id, void* message) {//回调函数，返回信息给用户
    };

void initialize_decl_cond() {//初始化数据
    //用户发送添加组织指令
    Cond add_message_2 = {
            .operate = NONE,
            .target = "type",
            .where_operate = EQUAL,
            .type = OBJ,
            .value = "add",
            .successor = NULL
    };
    Cond add_message_1 = {
            .operate = AND,
            .target = "id",
            .where_operate = EQUAL,
            .type = OBJ
            .value = "mechanism",
            .successor = &add_message_2;

    };
    message_submission_cond_1 = &add_message_1;
    //用户发送添加用户指令
    Cond add_message_4 = {
            .operate = NONE,
            .target = "type",
            .where_operate = EQUAL,
            .type = OBJ,
            .value = "add",
            .successor = NULL
    };
    Cond add_message_3 = {
            .operate = AND,
            .target = "id",
            .where_operate = EQUAL,
            .type = OBJ
            .value = "mechanism",
            .successor = &add_message_4;

    };
    message_submission_cond_2 = &add_message_3;

}
void message_submission_fun(long long session_id, void* message) {
    Cond store_data_name = {
        .operate = NONE,
        .target = "name",
        .where_operate = EQUAL,
        .type = OBJ,
        .value = ((UserInformation*)message)->name,
        .successor = NULL

    };

    StoreData temp_storedata_name;
    if ((temp_storedata_name = get_store_data("UserStoreData", &store_data_name, user_information_decl, sizeof(UserInformation))).data_size) {
        free((UserInformation*)temp_storedata_name.data[0]);
        return;
    }
}
