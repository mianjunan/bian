//
// Created by lpyyxy on 2022/3/7.
//

#ifndef _USER_MODULE_MAIN_H_
#define _USER_MODULE_MAIN_H_

#include <glowworm.h>
#include <stdio.h>
#include "util.h"
#include "shared.h"
#include<string.h>

typedef struct UserStoreDate {
	long long UID;
	unsigned int name_size;
	void* name;
	char encryption_password[32];
}UserStoreDate;

typedef struct UserInformation {
	unsigned int name_size;
	void* name;
	char encryption_password[32];
}UserInformation;

static Decl* user_store_data_decl;
static Decl* user_information_decl;
static Cond* message_submission_cond;

#endif
