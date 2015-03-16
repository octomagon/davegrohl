#ifndef DAVEGROHL_INCLUDE_DGCLOUDCONTROLLER_H_
#define DAVEGROHL_INCLUDE_DGCLOUDCONTROLLER_H_


// #import "DGCracker.h"
#import "DGTypes.h"
#import "DGTime.h"
#import "DGCloud.h"

#define dgMAX_HOSTS 512

class DGCloudController{
    
    DGCloud *theCloud;
    int threadsForHost[dgMAX_HOSTS];
    
    int sayHello();
    
public:
    int threads;
    DGStatus hostStatus[dgMAX_HOSTS];
    uint64_t guessesForHost[dgMAX_HOSTS], gpsForHost[dgMAX_HOSTS];
    bool foundPassword;
    char passwd[MAX_PASSWD_LEN], attackType[64];
    int serverCount = 0;
    DGTimer cloudTimer;
    bool firstUpdate = true;
    
    DGCloudController();
    int connect();
    int startCracking(crack_t *theParams, hashData_t *theHashData);
    int killThemAll(uint64_t *total, uint64_t *agps);
    int getUpdates();
};


int watchServer(int fd, DGCloudController *cloudController);
int readPacketFromServer(DGPacket packet, DGCloudController *cloudController);
void printCloudReport(DGCloudController *cloudController);


#endif // DAVEGROHL_INCLUDE_DGCLOUDCONTROLLER_H_