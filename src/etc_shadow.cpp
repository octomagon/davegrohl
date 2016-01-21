#include "etc_shadow.h"


int EtcShadow::dumpUsers(){
    for (int i = 0; i < users.size(); i++) {
        printf("%s %s\n", users[i].name, users[i].hash);
    }
    return 0;
}

int EtcShadow::hashDataForUser(const char user[], HashData *someHashData){
    // Overwrite garbage with '\0'
    memset(someHashData, 0, sizeof(HashData));
    
    // Load hash data
    for (int i = 0; i < users.size(); i++){
        if (0 == strcmp(user, users[i].name)) {
            
            someHashData->hashType = kCryptHashType;
            strcpy(someHashData->cryptHash, users[i].hash);
            
            for (int x = strlen(users[i].hash); x >= 0; x--) {
                if (users[i].hash[x] == '$'){
                    memcpy(someHashData->cryptSalt, &users[i].hash, x+1);
                    someHashData->cryptSalt[x+1] = '\0';
                    return true;
                }
            }
            
            if (someHashData != nullptr) {
                return false;
            }
            
            return true;
        }
    }
    
    
    return false;
}

int EtcShadow::rawFile(){
    char line[LINE_BUFF];
    FILE* file = fopen("/etc/shadow", "r");
    
    if (!file) {
        fprintf(stderr, "error: could not open /etc/shadow\n");
        return EXIT_FAILURE;
    }
    
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
    
    return EXIT_SUCCESS;
}


EtcShadow::EtcShadow(){
    char line[LINE_BUFF];
    char* pch;
    int x = 0;
    FILE* file = fopen("/etc/shadow", "r");
    
    if (!file) {
        fprintf(stderr, "error: could not open /etc/shadow\n");
        return;
    }
    
    ShadowUser aUser;
    
    // Split and parse each line.
    
    while (fgets(line, sizeof(line), file)) {
        x = 0;
        memset(&aUser, 0, sizeof(ShadowUser));
        
        pch = strtok (line,":");
        while (pch != NULL)
        {
            if(x == 0){ strcpy(aUser.name, pch); }
            if(x == 1){ strcpy(aUser.hash, pch); }
            pch = strtok (NULL, ":");
            x++;
        }
        
        if (0 != strcmp(aUser.hash, "*") && 0 != strcmp(aUser.hash, "!")) { // Only users with hashes.
            users.push_back(aUser);
        }
    }
    fclose(file);
    
}