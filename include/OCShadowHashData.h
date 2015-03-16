#ifndef DAVEGROHL_INCLUDE_OCSHADOWHASHDATA_H_
#define DAVEGROHL_INCLUDE_OCSHADOWHASHDATA_H_


#import <Foundation/Foundation.h>
#import "DGTypes.h"

@interface OCShadowHashData : NSObject{
    
}

- (void)setPlistFile:(NSString *)thePlist;
- (id)plistFile;
- (void)readJohnPasswdFile:(NSString *)thePasswdFile;
- (void)setTheUser:(NSString *)username;
- (id)theUser;
- (void)dumpShadowHashData;
- (void)dumpHashTypes;
- (id)hashOfType:(DGHashType)hashType;
- (DGHashType)weakestHashType;
- (void)dumpForJohn;
- (bool)userExists:(NSString *)username;
- (bool)isLoaded;
- (void)dumpError;
- (void)exitWithErrorIfNotLoaded;

+ (char *)displayNameForHashType:(const char *)hashType;

@property(readwrite, strong) NSString *plistFile;
@property(readwrite, strong) NSString *theUser;
@property(readwrite, strong) NSMutableDictionary *shadowHashData;
@property(readwrite, strong) NSString *errorMessage;

@end

int loadHashForCracking(hashData_t *theHashData, OCShadowHashData *theSHD);


#endif // DAVEGROHL_INCLUDE_OCSHADOWHASHDATA_H_