#ifndef ETC_SHADOW_H_
#define ETC_SHADOW_H_


#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>

#include "hash_data.h"

#define LINE_BUFF 2048
#define NAME_LEN 128
#define HASH_LEN 2048


struct ShadowUser {
    char name[NAME_LEN];
    char hash[HASH_LEN];
};


class EtcShadow{
    std::vector<ShadowUser> users;
    
public:
    EtcShadow();
    int dumpUsers();
    int hashDataForUser(const char user[], HashData *someHashData);
    int rawFile();
};


#endif // ETC_SHADOW_H_