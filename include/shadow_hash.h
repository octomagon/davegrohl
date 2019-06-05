#ifndef SHADOW_HASH_H_
#define SHADOW_HASH_H_

#include <iostream>
#include <vector>
#include <string>


#include "apple_functions.h"
#include "hash_data.h"

struct shadow_user {
    std::string username;
    std::vector<HashData> hashes;
};



class ShadowData {
    std::vector<shadow_user> users;

public:
    int loadUsers();
};


int hashDataForUser(std::string user, HashData *someHashData);
void dumpHashData(std::string user);
void dumpHashcat(std::string user);



#endif // SHADOW_HASH_H_
