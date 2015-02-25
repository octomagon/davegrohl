#import <iostream>
#import <thread>
#import <vector>
#import <Foundation/Foundation.h>

#import "DGController.h"
#import "DGCloudController.h"

DGCloudController::DGCloudController(){
    theCloud = [[DGCloud alloc] init];
    foundPassword = NO;
}


int DGCloudController::connect(){
    
    printf("-- Looking for servers...\n");
    
    [theCloud searchForServers];
    [theCloud connect];
    sayHello();
    
    serverCount = [theCloud serverCount];
    
    printf("-- %d server(s) connected...\n", serverCount);
    
    #ifdef DEBUG
    printf("-- %d total threads\n", threads);
    #endif  
    
    return 0;
}

int DGCloudController::sayHello(){
    #ifdef DEBUG
    printf("-- Saying hello to the servers...\n");
    #endif
    
    for (int i = 0; i < [theCloud serverCount]; i++) {
        int fd = [theCloud fdForServerNumber:i];
        
        if (fd != -1) {
            DGPacket packet, response;
            strcpy(packet.message, msgHELLO);
            packet.serverID = i;
            
            write(fd, &packet, sizeof(packet));
            read(fd, &response, sizeof(response));
            
            threadsForHost[i] = response.threads;
            hostStatus[i] = DGReady;
        }
    }
    
    threads = 0;
    for (int i = 0; i < [theCloud serverCount]; i++) {
        threads = threads + threadsForHost[i];
    }
    
    return 0;
}


int DGCloudController::startCracking(crack_t *theParams, hashData_t *theHashData){
    std::vector<std::thread> threadVector;
    char buffer[4096];
    DGPacket packet;
    
    int i = 0;
    
    cloudTimer.start();
    
    while (theParams->position < (theParams->position + threadsForHost[i])) {
        int fd = [theCloud fdForServerNumber:i];
        
        if (fd != -1) {
            theParams->slots = threadsForHost[i];
            strcpy(packet.message, msgCRACK);
            packet.crackData = *theParams;
            packet.hashData = *theHashData;
            packet.crackData.serverID = i;
            
            write(fd, &packet, sizeof(packet));
            
            theParams->position = theParams->position + threadsForHost[i];
            threadVector.push_back(std::thread(watchServer, fd, this));
            
            hostStatus[i] = DGRunning;
        }
        i++;
    }
    
    printf("-- Started attack\n");
    
    for(auto &t : threadVector){
        t.join();
    }
    return 0;
}


int DGCloudController::killThemAll(uint64_t *total, uint64_t *agps){
    #ifdef DEBUG
    printf("-- killThemAll() called\n\n");
    #endif
    
    for (int i = 0; i < [theCloud serverCount]; i++) {
        DGPacket dieP;
        strcpy(dieP.message, msgDIE);
        
        write([theCloud fdForServerNumber:i], &dieP, sizeof(dieP));
    }
    
    return 0;
}

int DGCloudController::getUpdates(){
    
    for (int i = 0; i < [theCloud serverCount]; i++) {
        DGPacket updateP;
        strcpy(updateP.message, msgUPDATEREQUEST);
        
        write([theCloud fdForServerNumber:i], &updateP, sizeof(updateP));
    }
    
    return 0;
}



int watchServer(int fd, DGCloudController *cloudController){
    DGPacket packet;
    
    #ifdef DEBUG
    printf("-- Watching fd %d\n", fd);
    #endif
    
    // Drain the socket
    while (1) {
        ssize_t read_count = read(fd, &packet, sizeof(packet));
        
        if (0 == read_count) {
            break; // EOF
        } else if (read_count == -1){
            perror("*** read");
            break; // Error
        } else {
            readPacketFromServer(packet, cloudController);
        }
        
    }
    return 0;
}

int readPacketFromServer(DGPacket packet, DGCloudController *cloudController){
    
    if (0 == strncmp(packet.message, msgFOUNDPW, strlen(msgFOUNDPW))) {
        cloudController->killThemAll(NULL, NULL);
        #ifdef DEBUG
        printf("-- Server %d: %s, %s, %s\n-- %lld guesses, %lld gps\n", packet.serverID, packet.message, packet.password, packet.type, packet.count, packet.gps);
        #endif
        cloudController->hostStatus[packet.serverID] = DGDone;
        cloudController->guessesForHost[packet.serverID] = packet.count;
        cloudController->gpsForHost[packet.serverID] = packet.gps;
        strcpy(cloudController->passwd, packet.password);
        strcpy(cloudController->attackType, packet.type);
        cloudController->foundPassword = YES;
        
    } else if (0 == strncmp(packet.message, msgKILLED, strlen(msgKILLED))){
        #ifdef DEBUG
        printf("-- Server %d: %s, %lld guesses, %lld gps\n", packet.serverID, packet.message, packet.count, packet.gps);
        #endif
        
        cloudController->hostStatus[packet.serverID] = DGDone;
        cloudController->guessesForHost[packet.serverID] = packet.count;
        cloudController->gpsForHost[packet.serverID] = packet.gps;
    }
    else if (0 == strncmp(packet.message, msgSTATUS, strlen(msgSTATUS))){
        #ifdef DEBUG
        printf("-- Server %d: %s, %lld guesses, %lld gps\n", packet.serverID, packet.message, packet.count, packet.gps);
        #endif
        
        cloudController->hostStatus[packet.serverID] = DGSentStatus;
        cloudController->guessesForHost[packet.serverID] = packet.count;
        cloudController->gpsForHost[packet.serverID] = packet.gps;
        
        bool allUpdatesSent = true;
        for (int i = 0; i < cloudController->serverCount; i++) {
            if ((cloudController->hostStatus[i] != DGSentStatus) && allUpdatesSent){
                allUpdatesSent = false;
            }
        }
        
        if (allUpdatesSent) {
            uint64_t total = 0, totalGPS = 0;
            char prettyInt[STR_BUFF_LEN];
            
            for (int i = 0; i < cloudController->serverCount; i++) {
                cloudController->hostStatus[i] = DGRunning;
                total = total + cloudController->guessesForHost[i];
                totalGPS = totalGPS + cloudController->gpsForHost[i];
            }
            
            if (cloudController->firstUpdate) {
                printf("\n      TIME               GUESSES            PER SECOND\n");
                cloudController->firstUpdate = false;
            }
            
            intwithcommas(prettyInt, total);
            printf("%s\t%16s", cloudController->cloudTimer.elapsed(), prettyInt);
            
            intwithcommas(prettyInt, totalGPS);
            printf("%22s\n", prettyInt);
        }
    }
    
    
    
    // Print report if they are all done
    
    if (cloudController->hostStatus[packet.serverID] == DGDone) {
        bool shouldPrintReport = YES;
        
        for (int i = 0; i < cloudController->serverCount; i++) {
            // printf("status %d: %d\n", i, cloudController->hostStatus[i]);
            if (cloudController->hostStatus[i] != DGDone || shouldPrintReport == NO) {
                shouldPrintReport = NO;
            }
        }
        if (shouldPrintReport) {
            printCloudReport(cloudController);
            bailout();
        }
    }
    
    
    return 0;
}

void printCloudReport(DGCloudController *cloudController){
    uint64_t totalGuesses = 0, totalGPS = 0;
    float seconds = cloudController->cloudTimer.stop();
    char buffer[64];
    
    for (int i = 0; i < cloudController->serverCount; i++) {
        totalGuesses = totalGuesses + cloudController->guessesForHost[i];
        totalGPS = totalGPS + cloudController->gpsForHost[i];
    }
    
    if (cloudController->foundPassword) {
        printf("\n-- Found password : '\033[22;31m%s\033[22;0m'\n", cloudController->passwd);
        printf("-- (%s attack)\n", cloudController->attackType);
    }
    
    printf("\n");
    intwithcommas(buffer, totalGuesses);
    printf("Finished in %.3f seconds / %s total guesses...\n", seconds, buffer);
    intwithcommas(buffer, totalGPS);
    printf("%s guesses per second.\n", buffer);
}



