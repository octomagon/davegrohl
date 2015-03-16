#ifndef DAVEGROHL_INCLUDE_DGATTACK_H_
#define DAVEGROHL_INCLUDE_DGATTACK_H_


#import <Foundation/Foundation.h>

#import "OCShadowHashData.h"
#import "DGCloudController.h"
#import "DGTypes.h"

@class DGAttack;
@protocol DGAttackDelegate <NSObject>
- (void)hasStatusUpdate:(DGAttackStatus *)update;
@end



@interface DGAttack : NSObject{
    
}

@property (nonatomic, assign) id <DGAttackDelegate> delegate;

- (void)loadHashData:(hashData_t *)hashData withOptions:(crack_t *) params;
- (void)start;
- (void)stop;
- (DGAttackStatus)getStatusUpdate;
- (void)printStatusUpdate:(char)ch;


// Kick it old school

void attack(crack_t *theParams, hashData_t *theHashData);
int incrementalAttack(crack_t *theParams, hashData_t *theHashData, int threadNum);
int dictionaryAttack(crack_t *theParams, hashData_t *theHashData);
int crackWordlist(const char * wordlist, int threadNum);
int tryPassword(hashData_t *userHashData, const char *passwd);
int loadHashForCracking(hashData_t *theHashData, OCShadowHashData *theSHD);
void setBatchSize(crack_t *theParams);
char * timeLeft(crack_t *theParams);
void getFinalCount();

@end


#endif // DAVEGROHL_INCLUDE_DGATTACK_H_