#include "apple_functions.h"

#ifdef __APPLE__

int hashDataFromPlist(std::string filename, std::vector<HashData> *hashes){
    
    NSDictionary *thePlistDictionary = [[NSDictionary alloc] initWithContentsOfFile:[NSString stringWithUTF8String:filename.c_str()]];
    
    if (thePlistDictionary == nil) {
        return -1;
    }
    
    NSData *hashData = [[NSData alloc] initWithData:[[thePlistDictionary objectForKey:@"ShadowHashData"] objectAtIndex:0]];
    
    NSMutableDictionary *shadowHashData = [[NSMutableDictionary alloc] init];
    shadowHashData = [NSPropertyListSerialization propertyListWithData:hashData options:NSPropertyListMutableContainersAndLeaves format:NULL error:NULL];
    
    // Iterate thru all the keys in ShadowHashData
    for(NSString *aKey in shadowHashData) {
        // printf("%s\n", [aKey UTF8String]);
        HashData *someHashData = new HashData;
        bzero(someHashData, sizeof(HashData));
        
        if (0 == strcmp([aKey UTF8String], "SRP-RFC5054-4096-SHA512-PBKDF2")) {
            NSDictionary *aDic = [[NSDictionary alloc] initWithDictionary:[shadowHashData objectForKey:aKey]];
            
            strcpy(someHashData->appleLabel, "SRP-RFC5054-4096-SHA512-PBKDF2");
            memcpy(someHashData->hash, [[aDic objectForKey:@"verifier"] bytes], [[aDic objectForKey:@"verifier"] length]);
            someHashData->hashLength = [[aDic objectForKey:@"verifier"] length];
            memcpy(someHashData->salt, (unsigned char*)[[aDic objectForKey:@"salt"] bytes], [[aDic objectForKey:@"salt"] length]);
            someHashData->saltLength = [[aDic objectForKey:@"salt"] length];
            someHashData->rounds = [[aDic objectForKey:@"iterations"] intValue];
            someHashData->hashType = kPBKDF2_SHA512HashType;
            
            [aDic release];
            hashes->push_back(*someHashData);
        }else if (0 == strcmp([aKey UTF8String], "SALTED-SHA512-PBKDF2")) {
            NSDictionary *aDic = [[NSDictionary alloc] initWithDictionary:[shadowHashData objectForKey:aKey]];
            
            strcpy(someHashData->appleLabel, "SALTED-SHA512-PBKDF2");
            memcpy(someHashData->hash, [[aDic objectForKey:@"entropy"] bytes], [[aDic objectForKey:@"entropy"] length]);
            someHashData->hashLength = [[aDic objectForKey:@"entropy"] length];
            memcpy(someHashData->salt, (unsigned char*)[[aDic objectForKey:@"salt"] bytes], [[aDic objectForKey:@"salt"] length]);
            someHashData->saltLength = [[aDic objectForKey:@"salt"] length];
            someHashData->rounds = [[aDic objectForKey:@"iterations"] intValue];
            
            [aDic release];
            hashes->push_back(*someHashData);
        }else if (0 == strcmp([aKey UTF8String], "NT")){
            
            strcpy(someHashData->appleLabel, "NT");
            memcpy(someHashData->hash, [[shadowHashData objectForKey:aKey] bytes], [[shadowHashData objectForKey:aKey] length]);
            someHashData->hashLength = [[shadowHashData objectForKey:aKey] length];
            someHashData->hashType = kMD4HashType;
            
            hashes->push_back(*someHashData);
        }
        
        delete someHashData;
    }
    
    return 0;
}

int hashDataFromPlist(std::string filename, HashData *someHashData){
    
    NSDictionary *thePlistDictionary = [[NSDictionary alloc] initWithContentsOfFile:[NSString stringWithUTF8String:filename.c_str()]];
    
    if (thePlistDictionary == nil) {
        return -1;
    }
    
    NSData *hashData = [[NSData alloc] initWithData:[[thePlistDictionary objectForKey:@"ShadowHashData"] objectAtIndex:0]];
    
    NSMutableDictionary *shadowHashData = [[NSMutableDictionary alloc] init];
    shadowHashData = [NSPropertyListSerialization propertyListWithData:hashData options:NSPropertyListMutableContainersAndLeaves format:NULL error:NULL];
    
    NSDictionary *pbkdf2 = [[NSDictionary alloc] initWithDictionary:[shadowHashData objectForKey:@"SALTED-SHA512-PBKDF2"]];
    
    memcpy(someHashData->hash, [[pbkdf2 objectForKey:@"entropy"] bytes], [[pbkdf2 objectForKey:@"entropy"] length]);
    someHashData->hashLength = [[pbkdf2 objectForKey:@"entropy"] length];
    memcpy(someHashData->salt, (unsigned char*)[[pbkdf2 objectForKey:@"salt"] bytes], [[pbkdf2 objectForKey:@"salt"] length]);
    someHashData->saltLength = [[pbkdf2 objectForKey:@"salt"] length];
    someHashData->rounds = [[pbkdf2 objectForKey:@"iterations"] intValue];
    someHashData->hashType = kPBKDF2_SHA512HashType;
    
    [pbkdf2 release];
    [thePlistDictionary release];
    [hashData release];
    [shadowHashData release];
    
    return 0;
}


void dictionaryToHashData(NSDictionary *theDic, HashData *someHashData){
    
}

void rawShadowHashData(std::string user){
    char filename[256] = "/var/db/dslocal/nodes/Default/users/";
    strcat(filename, user.c_str());
    strcat(filename, ".plist");
    
    NSDictionary *thePlistDictionary = [[NSDictionary alloc] initWithContentsOfFile:[NSString stringWithUTF8String:filename]];
    NSData *hashData = [[NSData alloc] initWithData:[[thePlistDictionary objectForKey:@"ShadowHashData"] objectAtIndex:0]];
    
    NSMutableDictionary *shadowHashData = [[NSMutableDictionary alloc] init];
    shadowHashData = [NSPropertyListSerialization propertyListWithData:hashData options:NSPropertyListMutableContainersAndLeaves format:NULL error:NULL];
    
    printf("%s\n", [[shadowHashData description] UTF8String]);
    
    
    [thePlistDictionary release];
    [hashData release];
    [shadowHashData release];
}


#endif // __APPLE__