#import <iostream>
#import <fstream>
#import <string.h>

#import "OCHashing.h"
#import "OCShadowHashData.h"


@implementation OCShadowHashData

- (id) init{
    self = [super init];
    // shadowHashData = [[NSMutableDictionary alloc] init];
    [self setErrorMessage:@"OMG! Unknown error\n"];
    
    return self;
}

- (void)setPlistFile:(NSString *)thePlist{
    
    plistFile = thePlist;
    
    if (-1 == access([plistFile UTF8String],F_OK)) {
        [self setErrorMessage:@"I couldn't find that plist.\n"];
        return;
    }
    
    NSDictionary *thePlistDictionary = [[NSDictionary alloc] initWithContentsOfFile:thePlist];
    NSData *hashData = [[NSData alloc] initWithData:[[thePlistDictionary objectForKey:@"ShadowHashData"] objectAtIndex:0]];
    // shadowHashData = [NSPropertyListSerialization propertyListFromData:hashData mutabilityOption:NSPropertyListMutableContainersAndLeaves format:NULL errorDescription:NULL];
    
    shadowHashData = [NSPropertyListSerialization propertyListWithData:hashData options:NSPropertyListMutableContainersAndLeaves format:NULL error:NULL];
    
}

- (id)plistFile{ return plistFile; }

- (void)readJohnPasswdFile:(NSString *)thePasswdFile{
    
    string line, buff;
    char buffer[4096];
    uint8_t charArray[2048], salt[64];
    size_t arrayLength;
    int x = 0;
    
    ifstream wordlist([thePasswdFile UTF8String]);
    if (!wordlist.is_open()){
        printf("Couldn't open the wordlist!\n");
        exit(-2);
    }
    
    if (wordlist.good()) {
        getline(wordlist,line);
    }
    
    // Strip out the second ':' seperated field.
    buff = line.substr((line.find_first_of(":") + 1),(line.length()-(line.find_first_of(":") + 1)));
    buff = buff.substr(0, buff.find_first_of(":"));
    
    // Cut out unexpected characters.  If well formatted, this should do nothing.
    buff = buff.substr(0, buff.find_first_not_of("0123456789abcdefABCDEF$ml"));
    
    strcpy(buffer, buff.c_str());
    
    if (32 == buff.length()) {  // MD4
        arrayLength = strlen(buffer)/2;
        
        for(int y = 0; y < arrayLength; y++){
            sscanf(&buffer[y*2],"%02X", &x);
            charArray[y] = x;
        }
        
        shadowHashData = [NSMutableDictionary dictionaryWithObject:[NSData dataWithBytes:charArray length:arrayLength] forKey:@"NT"];
    } else if (136 == buff.length()){  // Salted SHA-512
        arrayLength = strlen(buffer)/2;
        
        for(int y = 0; y < arrayLength; y++){
            sscanf(&buffer[y*2],"%02X", &x);
            charArray[y] = x;
        }
        
        shadowHashData = [NSMutableDictionary dictionaryWithObject:[NSData dataWithBytes:charArray length:arrayLength] forKey:@"SALTED-SHA512"];
    } else {
        [self setErrorMessage:@"I didn't recognize that hash.\n"];
    }
    
}

- (void)setTheUser:(NSString *)username{
    theUser = username;
    
    plistFile = @"/var/db/dslocal/nodes/Default/users/";
    plistFile = [plistFile stringByAppendingString:username];
    plistFile = [plistFile stringByAppendingString:@".plist"];
    
    if (0 != getuid()) {
        [self setErrorMessage:@"You must be root.\n"];
		return;
	}
    
    if (-1 == access([plistFile UTF8String],F_OK)) {
        if (-1 == access([username UTF8String],F_OK)) {
            [self setErrorMessage:@"I couldn't find that user.\n"];
            return;
        } else {
            [self setPlistFile:username];
        }
    }
    
    [self setPlistFile:plistFile];
}

- (id)theUser{ return theUser; }

- (void)dumpShadowHashData{
    
    printf("%s\n", [[shadowHashData description] UTF8String]);
    
}

- (void)dumpHashTypes{
    
    NSArray *hashTypes = [[NSArray alloc] initWithArray:[shadowHashData allKeys]];
    
    for (int i = 0; i < [hashTypes count]; i++) {
        printf("%s\n", [OCShadowHashData displayNameForHashType:[[hashTypes objectAtIndex:i] UTF8String]]);
    }
}

- (id)hashOfType:(DGHashType)hashType{
    
    if (hashType == MD4HashType){
            return [shadowHashData objectForKey:@"NT"];
    } else if (hashType == SHA512HashType){
            return [shadowHashData objectForKey:@"SALTED-SHA512"];
    } else if (hashType == PBKDF2HashType){
            return [shadowHashData objectForKey:@"SALTED-SHA512-PBKDF2"];
    }
    
    return 0;
}

- (DGHashType)weakestHashType{
    
    NSArray *hashTypes = [[NSArray alloc] initWithArray:[shadowHashData allKeys]];
    
    if ([hashTypes containsObject:@"NT"]) {
        return MD4HashType;
    } else if ([hashTypes containsObject:@"SALTED-SHA512"]) {
        return SHA512HashType;
    } else if ([hashTypes containsObject:@"SALTED-SHA512-PBKDF2"]) {
        return PBKDF2HashType;
    }

    return UnknownHashType;
}

- (void)dumpForJohn{
    // username:$ml$iterations$salt$hash
    // username:$pbkdf2-hmac-sha512$iterations.salt.hash
    
    id weakHash = [self hashOfType:[self weakestHashType]]; // [theSHD weakestHashType]
    
    if ([weakHash isKindOfClass:[NSData class]]) {
        printf("%s:%s\n", [[self theUser] UTF8String], dumpHex([weakHash bytes], [weakHash length]));
    } else if([weakHash isKindOfClass:[NSDictionary class]]){
        printf("%s:$pbkdf2-hmac-sha512$%d", [[self theUser] UTF8String], [[weakHash objectForKey:@"iterations"] intValue]);
        printf(".%s", dumpHex([[weakHash objectForKey:@"salt"] bytes], [[weakHash objectForKey:@"salt"] length]));
        printf(".%s", dumpHex([[weakHash objectForKey:@"entropy"] bytes], [[weakHash objectForKey:@"entropy"] length]));
        printf("\n");
        
    }else {
        printf("Sorry, I can't format this hash for JtR yet.\n");
        printf("%s\n", [[weakHash description] UTF8String]);
    }
    
}

+ (char *)displayNameForHashType:(const char *)hashType{
    char *newHashTypeName = (char *)hashType;
    
    if (0 == strcmp(hashType, "NT")) {
        newHashTypeName = (char *)"MD4";
    } else if (0 == strcmp(hashType, "SALTED-SHA512")) {
        newHashTypeName = (char *)"Salted SHA512";
    } else if (0 == strcmp(hashType, "SALTED-SHA512-PBKDF2")) {
        newHashTypeName = (char *)"PBKDF2 (Salted SHA512)";
    } else if (0 == strcmp(hashType, "RECOVERABLE")) {
        newHashTypeName = (char *)"Recoverable";
    }
    
    return newHashTypeName;
}

- (bool)userExists:(NSString *)username{
    char buff[2048];
    
    sprintf(buff, "/var/db/dslocal/nodes/Default/users/%s.plist", [username UTF8String]);
    
    if (-1 == access(buff,F_OK)){
        return NO;
    } else {
        return YES;
    }
}

- (bool)isLoaded{
    
    if ([shadowHashData count] > 0) {
        return true;
    } else{
        return false;
    }
}

- (void)dumpError{
    printf("%s", [[self errorMessage] UTF8String]);
}


- (void)exitWithErrorIfNotLoaded{
    if (![self isLoaded]) {
        [self dumpError];
        exit(0);
    }
}


@synthesize plistFile;
@synthesize theUser;
@synthesize shadowHashData;
@synthesize errorMessage;

@end

int loadHashForCracking(hashData_t *theHashData, OCShadowHashData *theSHD){
    DGHashType theType;
    
    if (theHashData->hashType == UnknownHashType) {
        theType = [theSHD weakestHashType];
    }
    
    if (theType == MD4HashType){
        memcpy(theHashData->hash, [[theSHD hashOfType:MD4HashType] bytes], [[theSHD hashOfType:MD4HashType] length]);
        theHashData->hashType = MD4HashType;
        printf("-- Loaded MD4 hash...\n");
    } else if (theType == SHA512HashType){
        memcpy(theHashData->hash, [[theSHD hashOfType:SHA512HashType] bytes], [[theSHD hashOfType:SHA512HashType] length]);
        memcpy(theHashData->salt, theHashData->hash, 4);
        theHashData->hashType = SHA512HashType;
        printf("-- Loaded Salted SHA512 hash...\n");
    }else if (theType == PBKDF2HashType){
        getPBKDF2Data([theSHD plistFile], theHashData->hash, theHashData->salt, theHashData->rounds);
        theHashData->hashType = PBKDF2HashType;
        printf("-- Loaded PBKDF2 (Salted SHA512) hash...\n");
    }
    
    return 0;
}
