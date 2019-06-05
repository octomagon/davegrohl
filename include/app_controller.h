#ifndef APP_CONTROLLER_H_
#define APP_CONTROLLER_H_

#include <iostream>
#include <iomanip>
#include <thread>

extern "C" {  // This may be paranoia...
    #include <termios.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/signal.h>
    #include <sys/utsname.h>
}

#include "cracker.h"
#include "etc_shadow.h"
#include "shadow_hash.h"

#define VERS "3.01 alpha"

static struct termios new_flags, old_flags;

typedef enum {
    kOneShot,
    kStandalone,
    kNone,
} run_mode_t;


class AppController{
    Cracker cracker;
    bool root = false;
    std::atomic<bool> firstUpdate{ true };
    std::string kernel, hostname, kernelVersion, arch;

    void onlyRoot();
    void startBanner();
    void printUpdate();
public:
    CrackerOptions options;
    run_mode_t runMode = kNone;

    AppController(int argc, char * argv[]);
    void run();
    void standalone();
    void oneShot();
    void daemonize();
    void bailout();

    void setCharSet(std::string charset);
    void setMinDigits(int min);
    void setMaxDigits(int max);
    void setIncremental();
    void setDictionary();
    void setUsername(std::string username);
    void setVerbose();
    void setThreads(int threads);
    void setTimeout(int timeout);

    void tryOnePassword(std::string password);
    int loadHash(std::string hash);
    int loadSalt(std::string salt);
    int loadRounds(int rounds);

    int help(void);
    int version(void);
    void printReport();
    void dumpSystemInfo();
    void dumpUserHash(std::string user);
    void dumpForHashcat(std::string user);

    static void watchForKeyboardHits(AppController *anApp);
    static void foundPassword(const char *msg);
};

void terminalNoEcho();
void terminalReset();


#endif // APP_CONTROLLER_H_
