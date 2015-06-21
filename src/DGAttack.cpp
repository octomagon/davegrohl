#import <unistd.h>
#import <vector>
#import <thread>
#import <atomic>
#import <iostream>
#import <dirent.h>

#import "OCHashing.h"
#import "OCMD4.h"
#import "DGTime.h"
#import "DGAttack.h"

using namespace std;

@implementation DGAttack
@synthesize delegate;

std::atomic<bool> done;
std::atomic<bool> didFindPassword;
LocalStats iv, dv;
hashData_t *theHashData;
crack_t *theParams;
DGTimer theTimer;
char thePassword[MAX_PASSWD_LEN];
bool firstUpdate = true;
DGAttackStatus finalUpdate;


- (void)loadHashData:(hashData_t *)hashData withOptions:(crack_t *) params{
    theHashData = hashData;
    theParams = params;
}


- (void)start{
    done = false;
    theTimer.start();
    
    attack(theParams, theHashData);
    
    finalUpdate.done = YES;
    
    [delegate hasStatusUpdate:&finalUpdate];
}

- (void)stop{
    #ifdef DEBUG
    printf("-- stop method called\n");
    #endif
    
    done = YES;
}

- (DGAttackStatus)getStatusUpdate{
    
    getFinalCount();
    return finalUpdate;
}

- (void)printStatusUpdate:(char)ch{
    char prettyInt[STR_BUFF_LEN], buff[STR_BUFF_LEN];
    
    //strcpy(buff, theTimer.elapsed());
    uint64_t total = (iv.totalCount() + dv.totalCount());
    
    if (firstUpdate) {
        printf("\n      TIME             GUESSES\t\n");
        firstUpdate = NO;
    }
    
    intwithcommas(prettyInt, total);
    printf("%s\t%14s  ", theTimer.elapsed(), prettyInt);
    
    

    if (0 < ch && ch <= iv.threads) {
        printf("Thread %d of %d (%s)\n", ch, iv.threads, iv.str[ch-1].value);
    } else if (ch == 68){ // t
        printf("%s\n", timeLeft(theParams));
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
    
}

void attack(crack_t *theParams, hashData_t *theHashData){
    
    vector<thread> threadVector;
    static DGCloudController cloudController;
    
    // Reset
    memset(&dv, 0, sizeof(dv));
    memset(&iv, 0, sizeof(iv));
    memset(&thePassword, 0, sizeof(thePassword));
    memset(&finalUpdate, 0, sizeof(finalUpdate));
    done = false;
    didFindPassword = false;
    firstUpdate = true;
    
    // Start
    
    setBatchSize(theParams);
    
    if (theParams->distributed == false) {
        if (theParams->totalThreads == 0) { theParams->totalThreads = theParams->slots; }
        thread dict(dictionaryAttack, theParams, theHashData);
        
        if (theParams->dictionary == -1 || theParams->incremental == 1) {
            iv.threads = theParams->slots;
            iv.firstThreadNum = theParams->position;
            // printf("iv.threads = %d\n", iv.threads);
            for (int i = theParams->position; i < (theParams->position + theParams->slots); ++i) {
                threadVector.push_back(thread(incrementalAttack, theParams, theHashData, i));
            }
            
            for(auto &t : threadVector){
                t.join();
            }
        }
        dict.join();
        
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
}


int incrementalAttack(crack_t *theParams, hashData_t *theHashData, int threadNum){
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
            
            if (tryPassword(theHashData, iv.str[threadNum].value) == 0) {
                done = true;
                if (!didFindPassword) {
                    didFindPassword = true;
                    finalUpdate.didFindPassword = YES;
                    strcpy(finalUpdate.password, iv.str[threadNum].value);
                    strcpy(finalUpdate.attackType, "incremental");
                    getFinalCount();
                }
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



int dictionaryAttack(crack_t *theParams, hashData_t *theHashData){
    
    DIR *dp;
    struct dirent *entry;
    std::vector<std::thread> threadVector;
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
        threadVector.push_back(std::thread(crackWordlist, wordlist[x], x));
    }
    dv.threads = x;
    
    // printf("dv.threads = %d\n", dv.threads);
    
    for(auto &t : threadVector){
        t.join();
    }
    
    return -1;
}


int crackWordlist(const char * wordlist, int threadNum){
    
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
        
        if (tryPassword(theHashData, dv.word[threadNum]) == 0) {
            done = true;
            if (!didFindPassword) {
                didFindPassword = true;
                finalUpdate.didFindPassword = YES;
                strcpy(finalUpdate.password, dv.word[threadNum]);
                strcpy(finalUpdate.attackType, "dictionary");
                getFinalCount();
            }
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

int tryPassword(hashData_t *userHashData, const char *passwd){
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




void setBatchSize(crack_t *theParams){
    
    IncString str;
    str.setChars(theParams->charset);
    
    if (theParams->batchSize == -1) {
        theParams->batchSize = (str.rangeForMinAndMaxDigits(theParams->min, theParams->max) / iv.threads);
        if (theParams->batchSize > 10000) {
            theParams->batchSize = 10000;
        }
    }
}

char * timeLeft(crack_t *theParams){
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

void getFinalCount(){
    
    finalUpdate.totalGuesses = (iv.totalCount() + dv.totalCount());
    finalUpdate.totalSeconds = theTimer.stop();
    
}

@end



