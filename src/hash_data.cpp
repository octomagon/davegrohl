#include "hash_data.h"

bool HashData::isEmpty(){
    if (strlen((const char *)hash) == 0){
        return true;
    } else {
        return false;
    }
}

int HashData::detectType(){
    if (hashLength == 128 && saltLength == 32 && rounds > 1) {
        hashType = kPBKDF2_SHA512HashType;
    } else if (hashLength == 16 && saltLength == 0 && rounds == 0) {
        hashType = kSMBNTHashType;
    }
    return -1;
}

std::string prettyHashType(HashData *theData){
    switch (theData->hashType) {
        case kPBKDF2_SHA512HashType:
            return std::string("PBKDF2 (Salted SHA512)");
            break;
        case kSHA512HashType:
            return std::string("Salted SHA512");
            break;
        case kSMBNTHashType:
            return std::string("SMB NT");
            break;
            
        default:
            break;
    }
    
    return std::string("Unknown Hash Type");
}

void dumpHashData(HashData *theData){
    printf("  appleLabel = %s\n", theData->appleLabel);
    printf("    Hash = %d <%s>\n", theData->hashLength, dumpHex(theData->hash, theData->hashLength));
    printf("    Salt = %d <%s>\n", theData->saltLength, dumpHex(theData->salt, theData->saltLength));
    printf("    Rounds = %d\n", theData->rounds);
    printf("    Type = %d\n", theData->hashType);
    printf("  cryptHash = <%s>\n", theData->cryptHash);
    printf("  cryptSalt = <%s>\n", theData->cryptSalt);
}



char * dumpHex(const void *hashData, int size){
    int x = 0;
    unsigned char *hashChars = (unsigned char *)hashData;
    char buff[128];
    static char hashString[2048];
    
    // printf("size: %d\n", size);
    
    for (x = 0; x < size; x++) {
        sprintf(buff, "%02x", hashChars[x]);
        strncpy((hashString+(x*2)), buff, 2);
    }
    hashString[2*size] = '\0';
    // printf("%s\n", hashString);
    return (char *)hashString;
}


int hexstring2uchar(std::string hexString, unsigned char charArray[]){
    int x = 0;
    size_t arrayLength = (hexString.length()/2);
    
    for(int y = 0; y < arrayLength; y++){
        sscanf(hexString.substr((y*2), 2).c_str(),"%02X", &x);
        charArray[y] = x;
    }
    
    return arrayLength;
}


