#ifndef DAVEGROHL_INCLUDE_DGTYPES_H_
#define DAVEGROHL_INCLUDE_DGTYPES_H_


#import <atomic>
#import "IncString.h"

#define msgCRACK "Crack"
#define msgDIE "Die"
#define msgFAILED "Failed"
#define msgFOUNDPW "Found Password"
#define msgHELLO "Hello"
#define msgKILLED "Killed"
#define msgOKAY "Okay"
#define msgTHREADS "Threads"
#define msgUPDATEREQUEST "Request Update"
#define msgSTATUS "Status"

#define HASH_LEN 512
#define MAX_PASSWD_LEN 128
#define MAX_THREADS 2048


typedef enum {
    MD4HashType,
    MD5HashType,
    SHA1HashType,
    SHA512HashType,
    PBKDF2HashType,
    UnknownHashType,
} DGHashType;

typedef enum {
    DGReady,
    DGRunning,
    DGSentStatus,
    DGDone,
} DGStatus;

struct hashData_t {
    unsigned char hash[HASH_LEN], salt[64];
    char passwd[MAX_PASSWD_LEN], saltString[64];
    int rounds;
    DGHashType hashType = UnknownHashType;
};

struct crack_t {
    bool verbose = 0, distributed = false, running = false;
    int totalThreads = 0, slots = 1, position = 0, serverID = -1;
    int dictionary = -1, incremental = -1;
    long double batchSize = -1;
    int min = 0, max = 16;
    char charset[256] = {0};
};

struct DGPacket {
    char message[32];
    char password[MAX_PASSWD_LEN], type[32];
    uint64_t count, gps;
    uint16_t serverID, threads;
    struct crack_t crackData;
    struct hashData_t hashData;
    
};

struct DGAttackStatus {
    bool done = false;
    bool didFindPassword = false;
    char password[MAX_PASSWD_LEN] = {0};
    char attackType[32] = {0};
    uint64_t totalGuesses = 0;
    float totalSeconds = 0;
};

struct LocalStats {
    bool started = false;
    IncString str[MAX_THREADS];
    char word[MAX_THREADS][MAX_PASSWD_LEN];
    DGStatus status[MAX_THREADS];
    uint64_t count[MAX_THREADS];
    long double position[MAX_THREADS];
    int threads = 0, firstThreadNum = 0;
    
    
    uint64_t totalCount(){
        uint64_t totalCount = 0;
        for (int i = firstThreadNum; i < (firstThreadNum + threads); i++) {
            totalCount = totalCount + count[i];
            // printf("Count for thread %d: %lld\n", i, count[i]);
        }
        
        return totalCount;
    }
};


#endif // DAVEGROHL_INCLUDE_DGTYPES_H_

