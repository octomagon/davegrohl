
#include "OCHashing.h"

int hexstring2uchar(string &hexString, uint8_t *charArray, size_t arrayLength){
    
    int x = 0;
    arrayLength = (hexString.length()/2);
    
    for(int y = 0; y < arrayLength; y++){
        sscanf(hexString.substr((y*2), 2).c_str(),"%02X", &x);
        charArray[y] = x;
    }
    
    return 0;
}

int getPBKDF2Data(NSString *plistPath, uint8_t theKey[], uint8_t theSalt[], int &rounds){
    
    if (-1 == access([plistPath UTF8String],F_OK)) {
        printf("Couldn't find the user...\n");
        exit(0);
    }
    
    NSDictionary *plist = [[NSDictionary alloc] initWithContentsOfFile:plistPath];
    NSData *hashData = [[NSData alloc] initWithData:[[plist objectForKey:@"ShadowHashData"] objectAtIndex:0]];
    NSDictionary *shadowHashData = [NSPropertyListSerialization propertyListWithData:hashData options:NSPropertyListMutableContainersAndLeaves format:NULL error:NULL];
    NSDictionary *pbkdf2 = [[NSDictionary alloc] initWithDictionary:[shadowHashData objectForKey:@"SALTED-SHA512-PBKDF2"]];
    
    memcpy(theKey, [[pbkdf2 objectForKey:@"entropy"] bytes], [[pbkdf2 objectForKey:@"entropy"] length]);
    memcpy(theSalt, (unsigned char*)[[pbkdf2 objectForKey:@"salt"] bytes], [[pbkdf2 objectForKey:@"salt"] length]);
    rounds = [[pbkdf2 objectForKey:@"iterations"] intValue];
    
    return 0;
}

int SHA1Hash(unsigned char *fullHash, const char *inPassword, unsigned char *salt){
    
    CC_SHA1_CTX		sha_context	= {};
    
    CC_SHA1_Init( &sha_context );
    CC_SHA1_Update( &sha_context, salt, 4 );
    CC_SHA1_Update( &sha_context, (unsigned char *)inPassword, strlen(inPassword));
    CC_SHA1_Final( fullHash, &sha_context );
    
    return 0;
}

int SHA512Hash(unsigned char *fullHash, const char *inPassword, unsigned char *salt){
    
    CC_SHA512_CTX	sha_context	= {};
    
    CC_SHA512_Init( &sha_context );
    CC_SHA512_Update( &sha_context, salt, 4 );
    CC_SHA512_Update( &sha_context, (unsigned char *)inPassword, strlen(inPassword));
    CC_SHA512_Final( fullHash, &sha_context );
    
    return 0;
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