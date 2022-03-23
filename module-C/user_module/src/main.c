//
// Created by lpyyxy on 2022/3/7.
//

#include "main.h"


int main(void)
{
    //初始化
    Config* config = load_config("config.cfg");
    if (!initialize(hash_string("user_module"),
        find_config(config, "address"),
        find_config(config, "private_key"))) {
        return 0;
    }
    initialize_decl_cond();
    //初始化数据库
    init_store_data("UserStoreData", user_store_data_decl);
    accept_message(message_submission_cond, user_information_decl, sizeof(UserInformation), message_submission_fun);
}
    void message_submission_fun(long long session_id, void* message){
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

           
            UserStoreDate user_store_data;
            bool cnt = false;
            user_store_data.UID =
                    hash_array(((UserInformation *) message)->name, ((UserInformation *) message)->name_size) &
                                  hash_array(((UserInformation *) message)->encryption_password, 32) & get_timestamp();
            user_store_data.name = ((UserInformation*)message)->name;
            user_store_data.name_size = ((UserInformation*)message)->name_size;
            int i;
            for (i = 0; i < 32; i++) {
                user_store_data.encryption_password[i] = ((UserInformation*)message)->encryption_password[i];
            }
            do {
               
                StoreData temp_storedata_UID;
               
                user_store_data.UID++;
               
                Cond store_data_UID = {
                     .operate = NONE,
                     .target = "UID",
                     .where_operate = EQUAL,
                     .type = LONG,
                     .value = long_to_byte_array(user_store_data.UID),
                     .successor = NULL

                };
                cnt =((temp_storedata_UID = get_store_data("UserStoreData", &store_data_UID, user_store_data_decl, sizeof(UserStoreDate))).data_size);
               
                free((UserStoreDate*)temp_storedata_UID.data[0]);
            } while (cnt);
            add_store_data("UserStoreData", &user_store_data);

            free((UserInformation*)temp_storedata_name.data[0]);
}
void initialize_decl_cond(){
    Decl* UID = normal_declaration("UID", LONG);
    Decl* name = normal_declaration("name", BYTE);
    name->is_dynamic_array = true;
    Decl* encryption_password = normal_declaration("encryption_password", BYTE);
    encryption_password->array_size = 32;
    user_store_data_decl = object_declaration("UserStoreDate", 3, UID, name, encryption_password);

    user_information_decl = object_declaration("UserInformation", 2,name, encryption_password);

    Cond message_submission_2 = {
            .operate = NONE,
            .target = "type",
            .where_operate = EQUAL,
            .type = OBJ,
            .value = "register",
            .successor = NULL
    };
    Cond message_submission_1 = {
            .operate = AND,
            .target = "id",
            .where_operate = EQUAL,
            .type = OBJ,
            .value = "user",
            .successor = &message_submission_2

    };
    message_submission_cond = &message_submission_1;
}

