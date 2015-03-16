#ifndef DAVEGROHL_INCLUDE_DGCRACKER_H_
#define DAVEGROHL_INCLUDE_DGCRACKER_H_


// clang++ -E -dM - < /dev/null | grep MAX

#include <iostream>
#include <thread>

#import "OCShadowHashData.h"
#import "IncString.h"
#import "DGServer.h"
#import "DGCloud.h"
#import "DGTypes.h"

extern int serverID;
extern atomic<bool> done,reportPrinted;
extern LocalStats iv;
extern DGTerm theTerm;
extern DGTimer theTimer;
extern LocalStats dv;
extern int masterFD;
extern DGCloudController cloudController;

int crack(crack_t *theParams, hashData_t *theHashData);
int theDictionaryAttack(crack_t *theParams, hashData_t *theHashData);
int crackWordlist(crack_t *theParams, hashData_t *theHashData, const char * wordlist, int threadNum);
int theIncrementalAttack(crack_t *theParams, hashData_t *theHashData, int threadNum);
int attemptPassword(hashData_t *userHashData, const char *passwd);
int makePredefined(char *charset, const char *pd);
int getHashForCracking(hashData_t *theHashData, OCShadowHashData *theSHD);
void updateStatus(crack_t *theParams);
void catchTerm();
void catchException();
void bail(int tid);
void calibrateBatchSize(crack_t *theParams);
char * timeRemaining(crack_t *theParams);
void printReport(bool found, const char *pw, const char *at);
int observeMaster(int fd);


#endif // DAVEGROHL_INCLUDE_DGCRACKER_H_
