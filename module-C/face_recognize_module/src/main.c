//
// Created by lpyyxy on 2022/3/7.
//

#include "main.h"



int main(void)
{
    //初始化
    Config *config= load_config("config.cfg");
    if(!initialize(hash_string("module"),
                  find_config(config,"address"),
                  find_config(config,"private_key"))){
        return 0;
    }
    initialize_decl_cond();
    //初始化数据库
    init_store_data("FaceRecognizeStoreData", face_recognize_store_data_decl);
    accept_message(message_submission_cond,
                   eigenvalues_package_decl,
                   sizeof(EigenvaluesPackage),
                   message_submission_fun);
}

void message_submission_fun(long long session_id,void* message)
{
    SessionIdObtainUIdSrcData sessionIdObtainUIdSrcData={
            .session_id=session_id
    };
    Cond store_data_eigenvalues={
            .operate=NONE,
            .target="UID",
            .where_operate=EQUAL,
            .type=LONG,
            .value=(char *) &(((SessionIdObtainUId *) get_module_data(hash_string("user_module"),
                                                                      "SessionIdObtainUID",
                                                                      &sessionIdObtainUIdSrcData,
                                                                      sizeof(sessionIdObtainUIdSrcData)))
                                                                              ->UID),
            .successor=NULL
    };

    FaceRecognizeStoreData* face_recognize_store_data=get_store_data("FaceRecognizeStoreData",
                                                                     &store_data_eigenvalues,
                                                                     face_recognize_store_data_decl,
                                                                     sizeof(FaceRecognizeStoreData))
            .data[0];

    EigenvaluesPackage* src_data = message;


    EigenvaluesPackage store_data = {
            .eigenvalues=face_recognize_store_data->eigenvalues,
            .eigenvalues_size=face_recognize_store_data->eigenvalues_size
    };
    bool is_match = 0;
    //TODO 判断面容是否相似。
    long long temp=face_recognize_store_data->timestamp[0];
    for(int i=1;i<9;i++){
        if(i<temp){
            face_recognize_store_data->timestamp[i]=get_timestamp();
            face_recognize_store_data->is_success[i]=is_match;
            continue;
        }else{
            temp=face_recognize_store_data->timestamp[i];
        }
    }
    if(temp==face_recognize_store_data->timestamp[9]){
        face_recognize_store_data->timestamp[0]=get_timestamp();
        face_recognize_store_data->is_success[0]=is_match;
    }

    StoreDataUpdate store_data_update={
            .timestamp=face_recognize_store_data->timestamp[0],
            .is_success=face_recognize_store_data->is_success[0]
    };

    update_store_data("FaceRecognizeStoreData",
                      store_data_update_decl,
                      &store_data_update);

    free(src_data);
    free(face_recognize_store_data);
    free(store_data_eigenvalues.value);
}

void initialize_decl_cond()
{
    Decl *UID = normal_declaration("UID",LONG);
    Decl *eigenvalues= normal_declaration("eigenvalues_package_decl",BYTE);
    eigenvalues->is_dynamic_array=true;
    Decl *timestamp= normal_declaration("timestamp",LONG);
    timestamp->array_size=10;
    Decl *is_success= normal_declaration("is_success",BOOLEAN);
    is_success->array_size=10;
    face_recognize_store_data_decl = object_declaration("StoreData",
                                                        4,
                                                        UID, eigenvalues, timestamp, is_success);

    Cond message_submission_2={
            .operate=NONE,
            .target="type",
            .where_operate=EQUAL,
            .type=OBJ,
            .value="submission",
            .successor=NULL
    };
    Cond message_submission_1={
            .operate=AND,
            .target="id",
            .where_operate=EQUAL,
            .type=OBJ,
            .value="face_recognize",
            .successor=&message_submission_2
    };
    message_submission_cond=&message_submission_1;

    Cond message_initialize_2={
            .operate=NONE,
            .target="type",
            .where_operate=EQUAL,
            .type=OBJ,
            .value="initialize",
            .successor=NULL
    };
    Cond message_initialize_1={
            .operate=AND,
            .target="id",
            .where_operate=EQUAL,
            .type=OBJ,
            .value="",
            .successor=&message_initialize_2
    };
    message_initialize_cond=&message_initialize_1;

    //TODO
}