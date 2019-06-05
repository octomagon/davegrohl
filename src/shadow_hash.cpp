#include "shadow_hash.h"


int ShadowData::loadUsers(){

#ifdef __APPLE__

#endif // __APPLE__

    return 0;
}



int hashDataForUser(std::string user, HashData *someHashData){
    memset(someHashData, 0, sizeof(HashData));

#ifdef __APPLE__

    char filename[256] = "/var/db/dslocal/nodes/Default/users/";
    strcat(filename, user.c_str());
    strcat(filename, ".plist");

    if (0 != hashDataFromPlist(filename, someHashData)){
        printf("Could not open shadow file.\n");
        exit(-1);
    }



#else  // Not __APPLE__

    char line[2048];
    FILE* file = fopen("/etc/shadow", "r");

    if (!file) {
        fprintf(stderr, "error: could not open /etc/shadow\n");
        return -1;
    }

    while (fgets(line, sizeof(line), file)) {
        char* pch;
        int x = 0;

        pch = strtok (line,":");
        while (pch != NULL)
        {
            if(x == 0){
                if(0 == strcmp(user.c_str(), pch)){
                    pch = strtok (NULL, ":");
                    strcpy(someHashData->cryptHash, pch);

                    for (int x = strlen(someHashData->cryptHash); x >= 0; x--) {
                        if (someHashData->cryptHash[x] == '$'){
                            memcpy(someHashData->cryptSalt, &someHashData->cryptHash, x+1);
                            someHashData->cryptSalt[x+1] = '\0';
                            return 0;
                        }
                    }
                }
            }
            pch = strtok (NULL, ":");
            x++;
        }
    }
    fclose(file);

#endif

    return -1;
}



void dumpHashData(std::string user){

#ifdef __APPLE__
    std::vector<HashData> theHashes;
    std::string filename = "/var/db/dslocal/nodes/Default/users/" + user + ".plist";

    if (-1 == hashDataFromPlist(filename, &theHashes)){
        std::cout << "I could not find that user." << std::endl;
        return;
    }

    for(int i=0; i < theHashes.size(); i++){
        dumpHashData(&theHashes[i]);
    }

#endif

}



void dumpHashcat(std::string user){

  #ifdef __APPLE__
      std::vector<HashData> theHashes;
      std::string filename = "/var/db/dslocal/nodes/Default/users/" + user + ".plist";

      if (-1 == hashDataFromPlist(filename, &theHashes)){
          std::cout << "I could not find that user." << std::endl;
          return;
      }

      for(int i=0; i < theHashes.size(); i++){
          if (strncmp(theHashes[i].appleLabel, "SALTED-SHA512-PBKDF2", strlen(theHashes[i].appleLabel)) == 0){
              std::cout << "$ml$" << theHashes[i].rounds;
              std::cout << "$" << dumpHex(theHashes[i].salt, theHashes[i].saltLength);
              // Hashcat only needs half of the hash... or the hash I'm showing is half garbage.... :(
              std::cout << "$" << dumpHex(theHashes[i].hash, theHashes[i].hashLength/2);
              std::cout << std::endl;
              return;
          }
      }
      std::cout << "I found the user but I couldn't find a compatable hash." << std::endl;

  #endif

}
