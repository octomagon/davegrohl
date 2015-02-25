#import <iostream>
#import <thread>
#import "DGCloudController.h"
#import "DGTerminal.h"
#import "DGController.h"

bool allDone = false, master = false, runningAsServer = false;
bool shouldPrintReport = false;
std::atomic<bool> reportPrinted;
int masterFD = -1, serverID = -1, sigTerms = 0;
DGTerm theTerm;
DGAttack *theAttack;
DGCloudController cloudController;

@implementation DGController 

- (void)startCracking:(hashData_t *)theHashData withOptions:(crack_t *) theParams{
    
    if (theParams->serverID > -1) {
        runningAsServer = true;
        serverID = theParams->serverID;
    }
    
    allDone = false;
    reportPrinted = false;
    signal(SIGINT, bailout);
    theTerm.noecho();
    
    if (theParams->distributed == false) {
        theAttack = [[DGAttack alloc] init];
        
        [theAttack setDelegate:self];
        [theAttack loadHashData:theHashData withOptions:theParams];
        
        printf("-- Starting attack\n");
        [theAttack start];
    } else {
        master = true;
        cloudController.connect();
        if (theParams->totalThreads == 0) { theParams->totalThreads = cloudController.threads; }
        
        #ifdef DEBUG
        printf("-- Total available cores: %d\n", theParams->totalThreads);
        #endif
        
        cloudController.startCracking(theParams, theHashData);
        
    }
    
    allDone = true;
    theTerm.reset();
    
}



- (void)hasStatusUpdate:(DGAttackStatus *)update{
    
    printf("\n");
    
    if (update->didFindPassword == YES) {
        printf("-- Found password : '\033[22;31m%s\033[22;0m'\n", update->password);
        printf("-- (%s attack)\n", update->attackType);
    } else {
        printf("-- Did not find password\n");
    }
    
    if (runningAsServer) {
        DGPacket packet;
        packet.serverID = serverID;
        DGAttackStatus status = [theAttack getStatusUpdate];
    
        if (status.didFindPassword) {
            strcpy(packet.message, msgFOUNDPW);
            strcpy(packet.password, status.password);
            strcpy(packet.type, status.attackType);
        } else {
            strcpy(packet.message, msgKILLED);
        }
        
        packet.count = status.totalGuesses;
        packet.gps = (status.totalGuesses/status.totalSeconds);
        packet.serverID = serverID;
        
        #ifdef DEBUG
        printf("-- Telling master that we're done\n");
        #endif
        
        write(masterFD, &packet, sizeof(packet));
    }

    
    printFinalReport();
    
    if (update->done && !runningAsServer) {
        bailout();
    }

    
}

void updateStatus(){
    int ch;
    
    while (1) {
        ch = (getc(stdin) - 48);
        if (ch == -21){ // ESC
            getc(stdin);
            getc(stdin);
        }
        
        if (master) {
            cloudController.getUpdates();
        } else {
            if (!allDone) {
                [theAttack printStatusUpdate:ch];
            }
        }
    }
}

int watchMaster(int fd){
    masterFD = fd;
    DGPacket packet;
    
    #ifdef DEBUG
    printf("-- Watching fd %d\n", masterFD);
    #endif
    
    // Drain the socket
    while (1) {
        ssize_t read_count = read(masterFD, &packet, sizeof(packet));
        
        if (0 == read_count) {
            break; // EOF
        } else if (read_count == -1){
            perror("*** read");
            break; // Error
        } else {
            if (0 == strncmp(packet.message, msgDIE, strlen(msgDIE))) {
                [theAttack stop];
                #ifdef DEBUG
                printf("-- Killed by the master\n");
                #endif
                break;
            } else if (0 == strncmp(packet.message, msgUPDATEREQUEST, strlen(msgUPDATEREQUEST))){
                sendStatusUpdateToMaster();
                [theAttack printStatusUpdate:0];
            }
        }
        
    }
    #ifdef DEBUG
    printf("-- Stopped watching master\n");
    #endif
    allDone = YES;
    
    return 0;
}



void bailout(int signal){
    
    if (signal == SIGINT) { sigTerms++; }
    
    if (master) {
        cloudController.killThemAll(NULL, NULL);
        
        if (sigTerms < 3 && signal == SIGINT) {
            #ifdef DEBUG
            printf("-- %d SIGINTs\n", sigTerms);
            #endif
            return;
        }
    } else {
        if (allDone == false) {
            printFinalReport();
        }
    }
    
    if (runningAsServer) {
        DGPacket packet;
        packet.serverID = serverID;
        DGAttackStatus status = [theAttack getStatusUpdate];
        
        if (status.didFindPassword) {
            strcpy(packet.message, msgFOUNDPW);
            strcpy(packet.password, status.password);
            strcpy(packet.type, status.attackType);
        } else {
            strcpy(packet.message, msgKILLED);
        }
        
        packet.count = status.totalGuesses;
        packet.gps = (status.totalGuesses/status.totalSeconds);
        packet.serverID = serverID;
        
        #ifdef DEBUG
        printf("-- Telling master that we're done\n");
        #endif
        
        write(masterFD, &packet, sizeof(packet));
    }
    
    theTerm.reset();
    
    #ifdef DEBUG
    printf("-- bailout(%d)\n", signal);
    #endif
    
    
    exit(0);
}

void bailout(){ bailout(0); }


void printFinalReport(){
    if (reportPrinted == true) {
        return;
    } else {
        reportPrinted = true;
    }
    
    char gpsString[STR_BUFF_LEN], guessesString[STR_BUFF_LEN];
    
    DGAttackStatus status = [theAttack getStatusUpdate];
    uint64_t gps = (status.totalGuesses / status.totalSeconds);
    
    intwithcommas(gpsString, gps);
    intwithcommas(guessesString, status.totalGuesses);
    
    printf("\n");
    printf("Finished in %.3f seconds / %s guesses...\n", status.totalSeconds, guessesString);
    printf("%s guesses per second.\n", gpsString);
    
}


int sendStatusUpdateToMaster(){
    DGPacket packet;
    DGAttackStatus status = [theAttack getStatusUpdate];
    
    strcpy(packet.message, msgSTATUS);
    packet.count = status.totalGuesses;
    packet.gps = (status.totalGuesses/status.totalSeconds);
    packet.serverID = serverID;
    
    write(masterFD, &packet, sizeof(packet));
    
    #ifdef DEBUG
    printf("-- Sent status update\n");
    #endif
    
    return 0;
}


@end


