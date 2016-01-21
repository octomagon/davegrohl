#ifndef HASH_DATA_H_
#define HASH_DATA_H_

#include <stdio.h>
#include <string.h>
#include <string>

enum {
    kHDMaxHashLen = 1024,
    kHDMaxSaltLen = 64
};

typedef enum {
    kUnknownHashType = 0,
    kCryptHashType,
    kSMBNTHashType,
    kMD4HashType,
    kMD5HashType,
    kSHA1HashType,
    kSHA512HashType,
    kPBKDF2_SHA512HashType,
} HashType;


class HashData {
public:
    char cryptHash[kHDMaxHashLen];
    char cryptSalt[kHDMaxSaltLen];
    unsigned char hash[kHDMaxHashLen];
    unsigned char salt[kHDMaxSaltLen];
    unsigned int hashLength;
    unsigned int saltLength;
    unsigned int rounds;
    HashType hashType = kUnknownHashType;
    char appleLabel[256];
    
    bool isEmpty();
    int detectType();
};

std::string prettyHashType(HashData *theData);
void dumpHashData(HashData *theData);
char * dumpHex(const void *hashData, int size);
int hexstring2uchar(std::string hexString, unsigned char charArray[]);

#endif // HASH_DATA_H_