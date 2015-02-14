
#import <Foundation/Foundation.h>
#import "DGAttack.h"
#import "DGTypes.h"

@interface DGController : NSObject <DGAttackDelegate>{
    
    
}

- (void)startCracking:(hashData_t *)theHashData withOptions:(crack_t *) theParams;

void updateStatus();
int watchMaster(int fd);
void bailout(int signal);
void bailout();
void printFinalReport();
int sendStatusUpdateToMaster();

@end