#ifndef CRACKER_H_
#define CRACKER_H_

extern "C" {
    #include <unistd.h>
    #include <dirent.h>
}

#include <fstream>
#include <thread>
#include <vector>
#include <atomic>

#include "app_timer.h"
#include "hash_data.h"
#include "hash_functions.h"
#include "inc_string.h"

#define CHARSET_all_ "aeorisn1tl2md0cp3hbuk45g9687yfwjvzxqASERBTMLNPOIDCHGKFJUW.!Y*@V-ZQX_$#,/+?;^ %~=&`\\)][:<(>\"|{'}"
#define CHARSET_azAZ09_ "aeorisn1tl2md0cp3hbuk45g9687yfwjvzxqASERBTMLNPOIDCHGKFJUWYVZQX"
#define CHARSET_az09_ "aeorisn1tl2md0cp3hbuk45g9687yfwjvzxq"
#define CHARSET_az_ "aeorisntlmdcphbukgyfwjvzxq"
#define CHARSET_09_ "0123456789"

#define MAX_THREADS 128


// ----------------------------------------------------------------------------
//    The Cracker class may need to be rewritten.  Maybe build a pure C++11
//  version and then benchmark old & new versions side-by-side.  Even without a
//  rewrite, this class needs better distinction between C & C++ and a
//  buttload of cleanup.
//
// ----------------------------------------------------------------------------
// --- The following is some example usage.
//
// hashDataForUser(options.username, &cracker.theHash);
// cracker.loadOptions(options);
//
// cracker.start();
//
// --- The legwork happens here.
//
// cracker.joinThreads();
//
// ----------------------------------------------------------------------------


struct CrackerOptions{
    bool incremental = false;
    bool dictionary = false;
    bool verbose = false;
    std::string username;
    std::string oneTryPW;
    std::string charset = CHARSET_az09_;
    unsigned int min = 0, max = 12;
    unsigned int timeout = 0;
    unsigned int cores = 1;
};

typedef bool (*hashFunction)(const char[], HashData*);
typedef void (*messageFunction)(const char *);


class Cracker{
private:
    CrackerOptions options;
    std::atomic<bool> done;
    std::vector<std::string> wordlists;
    long double range;
    long double batchSize = 100;
    
    inline bool isDone();
    
public:
    HashData theHash;
    AppTimer timer;
    std::vector<std::thread> iThreads;
    std::vector<std::thread> dThreads;
    std::vector<IncString> iStr;
    std::vector<std::string> guess;
    std::atomic<uint64_t> guesses{ 0 };
    std::string winner;
    
    hashFunction tryPassword;
    messageFunction printm;
    messageFunction foundIt;
    
    Cracker();
    int loadHashData(HashData someHashData);
    int loadOptions(CrackerOptions someOptions);
    int checkOptions(std::string& errorMessage);
    int start();
    void stop();
    int joinThreads();
    void stopAfterTimeout();
    bool tryOnePassword();
    
    bool incrementalAttack(int threadID);
    bool dictionaryAttack(int threadID);
};


// Non member functions

void printMessage(const char *msg);
void foundPassword(const char *msg);


#endif // CRACKER_H_
