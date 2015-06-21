#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/time.h>
#include <objc/objc-auto.h>
#include <exception>
#import <atomic>
#import <Foundation/Foundation.h>

#import "OCHashing.h"
#import "DGTerminal.h"
#import "DGTime.h"
#import "OCMD4.h"
#import "DGCloudController.h"
#import "DGServer.h"
#include "DGCracker.h"

int sigTermCount = 0;
bool distributed = false, killedThemAll = false, observeMasterFD;
bool connectedToMaster = false;
static bool serverMode = NO;


int crack(crack_t *theParams, hashData_t *theHashData){
    // @autoreleasepool{
        vector<thread> threadVector;
        
        done = NO;
        serverID = theParams->serverID;
        calibrateBatchSize(theParams);
    
        
        iv.started = true;
        theParams->running = YES;
        distributed = theParams->distributed;
        theTerm.noecho();
        theTimer.start();
        thread statusWatcher(updateStatus, theParams);
        
        if (theParams->distributed == false) {
            if (theParams->totalThreads == 0) { theParams->totalThreads = theParams->slots; }
            thread dict(theDictionaryAttack, theParams, theHashData);
            
            if (theParams->dictionary == -1 || theParams->incremental == 1) {
                iv.threads = theParams->slots;
                iv.firstThreadNum = theParams->position;
                 printf("iv.threads = %d\n", iv.threads);
                for (int i = theParams->position; i < (theParams->position + theParams->slots); ++i) {
                    threadVector.push_back(thread(theIncrementalAttack, theParams, theHashData, i));
                }
                
                printf("-- Started attack\n");
                
                for(auto &t : threadVector){
                    t.join();
                }
            }
            dict.join();
            
            fputs("\n",stdin);
            printReport(NO, NULL, NULL);
            #ifdef DEBUG
            printf("-- All cracking threads have joined\n");
            #endif
        } else {
            cloudController.connect();
            if (theParams->totalThreads == 0) { theParams->totalThreads = cloudController.threads; }
            
            #ifdef DEBUG
                printf("-- Total available cores: %d\n", theParams->totalThreads);
            #endif
            
            cloudController.startCracking(theParams, theHashData);
        }
    //}
    statusWatcher.join();
    theParams->running = NO;
    done = YES;
    // statusWatcher.detach();
    theTerm.reset();
    #ifdef DEBUG
    printf("-- crack() returned\n");
    #endif
    
    memset(&iv, 0, sizeof(iv));
    memset(&dv, 0, sizeof(dv));
    return 0;
}

int theDictionaryAttack(crack_t *theParams, hashData_t *theHashData){
    
    DIR *dp;
    struct dirent *entry;
    vector<thread> threadVector;
    int numThreads = 0;
    char wordlist[512][512];
    char buffer[512];

    
    if (theParams->dictionary == -1 && theParams->incremental == 1){
        return -1;
    }
    
    
    if((dp = opendir("./wordlists")) == NULL){
        // printf("-- No wordlists directory!\n");
        return -2;
    }
    
    int i = 0;
    while ((entry = readdir(dp))){
        if (entry->d_name[0] != '.') {
            sprintf(wordlist[i], "./wordlists/%s", entry->d_name);
            i++;
        }
    }
    closedir(dp);
    numThreads = i;
    
    int x;
    for (x = 0; x < numThreads; x++) {
        threadVector.push_back(thread(crackWordlist, theParams, theHashData, wordlist[x], x));
    }
    dv.threads = x;
    
    // printf("dv.threads = %d\n", dv.threads);
    
    for(auto &t : threadVector){
        t.join();
    }

    return -1;
}

int crackWordlist(crack_t *theParams, hashData_t *theHashData, const char * wordlist, int threadNum){
    
    dv.status[threadNum] = DGRunning;
    FILE *fp = fopen(wordlist , "r");
    
    while (!feof(fp)) {
        fscanf(fp, "%s\n", dv.word[threadNum]);
        
        if (theParams->verbose == YES) {
            if (!done) {
                #ifdef DEBUG
                printf("d%d: %s\n", threadNum, dv.word[threadNum]);
                #else
                printf("%s\n", dv.word[threadNum]);
                #endif
            } else { return 0; }
        }
        
        if (attemptPassword(theHashData, dv.word[threadNum]) == 0) {
            done = true;
            printReport(true, dv.word[threadNum], "dictionary");
            return 0;
        }
        if (done) {
            return 0;
        }
        
        dv.count[threadNum]++;
    }
    
    fclose(fp);
    
    dv.status[threadNum] = DGDone;
    return -1;
}


int theIncrementalAttack(crack_t *theParams, hashData_t *theHashData, int threadNum){
    iv.str[threadNum].setChars(theParams->charset);
    iv.str[threadNum].zero(theParams->min);
    
    iv.status[threadNum] = DGRunning;
    iv.position[threadNum] = iv.str[threadNum].num4str(iv.str[threadNum].value) + (theParams->batchSize * threadNum);
    iv.str[threadNum].setValue4num(iv.position[threadNum]);
    
    // printf("%d %s %.0Lf\n", threadNum, iv.str[threadNum].value, iv.position[threadNum]);
    
    while (1) {
        #ifdef DEBUG
        if (!done)
            printf("%d/%d - Starting batch at position: %.Lf\n", threadNum, theParams->totalThreads, iv.str[threadNum].num4str(iv.str[threadNum].value));
        #endif
        for (uint64_t i = 0; i < theParams->batchSize; i++) {
            if (strlen(iv.str[threadNum].value) > theParams->max) {
                iv.status[threadNum] = DGDone;
                return -1;
            }
            
            if (theParams->verbose == YES) {
                if (!done) {
                    #ifdef DEBUG
                    printf("i%d: %s\n", threadNum, iv.str[threadNum].value);
                    #else
                    printf("%s\n", iv.str[threadNum].value);
                    #endif
                } else { return 0; }
            }
            
            if (attemptPassword(theHashData, iv.str[threadNum].value) == 0) {
                done = true;
                printReport(true, iv.str[threadNum].value, "incremental");
                return 0;
            }
            
            if (done) {
                return 0;
            }
            
            iv.str[threadNum]++;
            iv.count[threadNum]++;
        }
        // Skip to the next batch
        iv.position[threadNum] = (iv.position[threadNum] + (theParams->batchSize * (theParams->totalThreads)));
        iv.str[threadNum].setValue4num(iv.position[threadNum]);
    }
    
    return -1;
}


int attemptPassword(hashData_t *userHashData, const char *passwd){
    static __thread unsigned char hash[HASH_LEN];

    if (userHashData->hashType == PBKDF2HashType) {
        CCKeyDerivationPBKDF(kCCPBKDF2, passwd, strlen(passwd), userHashData->salt, 32, kCCPRFHmacAlgSHA512, userHashData->rounds, hash, 128);
        
        if (memcmp(userHashData->hash, hash, 128) == 0){
            return 0;
        }
    } else if (userHashData->hashType == SHA512HashType) {
        SHA512Hash(hash, passwd, userHashData->salt);
        
        if (memcmp(&userHashData->hash[4] ,hash ,64) == 0) {
            return 0;
        }
    } else if (userHashData->hashType == MD4HashType){
        CalculateSMBNTHash(passwd, hash);
        
        if (memcmp(userHashData->hash ,hash ,16) == 0) {
            return 0;
        }
    }
    
    return -1;
}


int makePredefined(char *charset, const char *pd){
    
    if (strcmp(pd, "09") == 0) {
        strcpy(charset, "0123456789");
    } else if(strcmp(pd, "az") == 0) {
        strcpy(charset, "aeorisntlmdcphbukgyfwjvzxq");
    } else if(strcmp(pd, "az09") == 0) {
        strcpy(charset, "aeorisn1tl2md0cp3hbuk45g9687yfwjvzxq");
    } else if(strcmp(pd, "azAZ09") == 0) {
        strcpy(charset, "aeorisn1tl2md0cp3hbuk45g9687yfwjvzxqASERBTMLNPOIDCHGKFJUWYVZQX");
    } else if(strcmp(pd, "all") == 0) {
        strcpy(charset, "aeorisn1tl2md0cp3hbuk45g9687yfwjvzxqASERBTMLNPOIDCHGKFJUW.!Y*@V-ZQX_$#,/+?;^ %~=&`\\)][:<(>\"|{'}");
    } else {
        printf("I didn't recognize that charset.\n");
        exit(-1);
    }
    
    return 0;
}


int getHashForCracking(hashData_t *theHashData, OCShadowHashData *theSHD){
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

void updateStatus(crack_t *theParams){
    char *prettyInt, buff[128];
    bool firstUpdate = YES;
    int ch;
    
    while (1) {
        
        ch = (getc(stdin) - 48);
        if (ch == -21){ // ESC
            getc(stdin);
            getc(stdin);
        }
        /*
        uint64_t total = (iv.totalCount() + dv.totalCount());
        
        strcpy(buff, theTimer.elapsed());
        intwithcommas(prettyInt, total);
        
        if (firstUpdate) {
            printf("\n      TIME             GUESSES\t\n");
            firstUpdate = NO;
        }
        
        printf("%s\t%14s  ", buff, prettyInt);
        
        if (0 < ch && ch <= iv.threads) {
            printf("Thread %d of %d (%s)\n", ch, iv.threads, iv.str[ch-1].value);
        } else if (ch == 68){ // t
            printf("%s\n", timeRemaining(theParams));
        } else {
            for (int i = 0; i < dv.threads; i++) {
                // printf("%d ", dv.threads);
                if (dv.status[i] == DGRunning) {
                    printf("(%s) ", dv.word[i]);
                }
            }
            for (int i = theParams->position; i < (theParams->position + theParams->slots); i++) {
                // printf("%d ", iv.threads);
                if (iv.status[i] == DGRunning) {
                    printf("[%s] ", iv.str[i].value);
                }
            }
            printf("\n");
        }
        */
    }
}

void catchTerm(){
    while (!done) {
        sigTermCount++;
        signal(SIGINT, bail);
        // pause();
    }
    #ifdef DEBUG
    printf("catchTerm ended\n");
    #endif
}

void catchException(){
    theTerm.reset();
    #ifdef DEBUG
    printf("catchException() called\n");
    #endif
    bail(0);
}

void bail(int tid){
    #ifdef DEBUG
    printf("bail(%d) called\n", tid);
    #endif
    theTerm.reset();
        
    if (distributed && sigTermCount == 1 && killedThemAll == false) {
        killedThemAll = true;
        cloudController.killThemAll(NULL, NULL);
    }
    
    exit(0);
}




void calibrateBatchSize(crack_t *theParams){
    
    IncString str;
    str.setChars(theParams->charset);
    
    if (theParams->batchSize == -1) {
        theParams->batchSize = (str.rangeForMinAndMaxDigits(theParams->min, theParams->max) / iv.threads);
        if (theParams->batchSize > 10000) {
            theParams->batchSize = 10000;
        }
    }
}

char * timeRemaining(crack_t *theParams){
    IncString str;
    str.setChars(theParams->charset);
    static char fTime[2048];
    
    long double range = str.rangeForMinAndMaxDigits(theParams->min, theParams->max);
    float theTime = theTimer.stop();
    
    long double total = ((range - iv.totalCount()) / (iv.totalCount() / theTime));
    DGTimeUnits tu;
    secsToTimeUnits(&tu, total);
    
    
    sprintf(fTime, "%s left...", tu.prettyTime);
    return fTime;
}

void printReport(bool found, const char *pw, const char *at){
    char buffer[64];
    uint64_t total = (iv.totalCount() + dv.totalCount());
    float theTime = theTimer.stop();
    done = true;
    
    if (masterFD != -1) {
        DGPacket packet;
        packet.serverID = serverID;
        
        if (found) {
            strcpy(packet.message, msgFOUNDPW);
        } else {
            strcpy(packet.message, msgKILLED);
        }
        
        if (pw != NULL && at != NULL) {
            strcpy(packet.password, pw);
            strcpy(packet.type, at);
        }
        packet.count = total;
        packet.gps = (total/theTime);
        packet.serverID = serverID;
        
        #ifdef DEBUG
        printf("-- Telling master that we're done\n");
        #endif
        
        write(masterFD, &packet, sizeof(packet));
    }
    
    
    if (!reportPrinted || serverMode) {
        reportPrinted = true;
        theTerm.reset();
        
        if (found) {
            printf("\n-- Found password : '\033[22;31m%s\033[22;0m'\n", pw);
            printf("-- (%s attack)\n", at);
        }
        
        cout << endl;
        intwithcommas(buffer, total);
        printf("Finished in %.3f seconds / %s total guesses...\n",theTime ,buffer);
        intwithcommas(buffer, (total/theTime));
        printf("%s guesses per second.\n", buffer);
    }
    
    if (masterFD != -1) {
        connectedToMaster = NO;
        // printf("closing masterFD\n");
        // close(masterFD);
        done = YES;
    }
    
    
}

int observeMaster(int fd){
    connectedToMaster = YES;
    serverMode = YES;
    masterFD = fd;
    DGPacket packet;
    
    #ifdef DEBUG
    printf("-- Watching fd %d\n", masterFD);
    #endif
    
    // Drain the socket
    while (connectedToMaster) {
        ssize_t read_count = read(masterFD, &packet, sizeof(packet));
        
        if (0 == read_count) {
            break; // EOF
        } else if (read_count == -1){
            perror("*** read");
            break; // Error
        } else {
            if (0 == strncmp(packet.message, msgDIE, strlen(msgDIE))) {
                
                printf("-- Killed by the master\n");
                printReport(NO, NULL, NULL);
            }
        }
        
    }
    #ifdef DEBUG
    printf("-- Stopped watching master\n");
    #endif
    done = YES;
    
    return 0;
}




