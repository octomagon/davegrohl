// clang++ -E -dM - < /dev/null | grep MAX

#include <iostream>
#include <thread>

#import "OCShadowHashData.h"
#import "IncString.h"
#import "DGServer.h"
#import "DGCloud.h"
#import "DGTypes.h"




int crack(crack_t *theParams, hashData_t *theHashData);
int dictionaryAttack(crack_t *theParams, hashData_t *theHashData);
int crackWordlist(crack_t *theParams, hashData_t *theHashData, const char * wordlist, int threadNum);
int incrementalAttack(crack_t *theParams, hashData_t *theHashData, int threadNum);
int tryPassword(hashData_t *userHashData, const char *passwd);
int setPredefined(char *charset, const char *pd);
int loadHashForCracking(hashData_t *theHashData, OCShadowHashData *theSHD);
void updateStatus(crack_t *theParams);
void catchTerm();
void catchException();
void bailout(int tid);
void calibrateBatchSize(crack_t *theParams);
char * timeLeft(crack_t *theParams);
void printReport(bool found, const char *pw, const char *at);
int watchMaster(int fd);

