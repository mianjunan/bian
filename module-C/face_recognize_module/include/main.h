//
// Created by lpyyxy on 2022/3/7.
//

#ifndef _USER_MODULE_MAIN_H_
#define _USER_MODULE_MAIN_H_

#include "glowworm.h"
#include <stdio.h>
#include "util.h"
#include "shared.h"

typedef struct FaceRecognizeStoreData{

    long long UID;

    unsigned int eigenvalues_size;
    void *eigenvalues;

    long long timestamp[10];

    bool is_success[10];

}FaceRecognizeStoreData;

typedef struct StoreDataUpdate{

    long long timestamp[10];

    bool is_success[10];

}StoreDataUpdate;

typedef struct EigenvaluesPackage{

    unsigned int eigenvalues_size;
    void *eigenvalues;

}EigenvaluesPackage;

static Decl *face_recognize_store_data_decl;

static Decl *store_data_update_decl;

static Decl *eigenvalues_package_decl;

static Cond *message_submission_cond;

static Cond *message_initialize_cond;

#endif
