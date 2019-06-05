#include "main.h"

void handleSigint(int signum){
    sig_caught++;
}


void signalWatcher(AppController *anApp){
    while (1) {
        if (sig_caught > 0) {
            sig_caught = 0;
            anApp->bailout();
        }
    }
}


int main(int argc, char * argv[]){
    std::string optstring;
    AppController *app = new AppController(argc, argv);

    // atexit(AppController::bailout);

    // Generate optstring
    if (optstring.empty()) {
        for (int x = 0; x < (sizeof(longopts)/sizeof(struct option)); x++){
            if (longopts[x].flag == NULL) {
                optstring.push_back((char)longopts[x].val);
            }

            if (longopts[x].has_arg == 1) {
                optstring.append(":");
            } else if (longopts[x].has_arg == 2){
                optstring.append("::");
            }
        }
    }


    // Parse arguments
    int ch;
    while ((ch = getopt_long(argc, argv, optstring.c_str(), longopts, NULL)) != -1) {
        switch (ch) {
            case 'c':
                app->setCharSet(optarg);
                break;
            case 'd':
                app->setDictionary();
                break;
            case 'D':
                app->daemonize();
                exit(0);
                break;
            case 'h':
                app->help();
                exit(0);
                break;
            case 'H':
                app->loadHash(optarg);
                break;
            case 'i':
                app->setIncremental();
                break;
            case 'I':
                app->dumpSystemInfo();
                exit(0);
                break;
            case 'm':
                app->setMinDigits(atoi(optarg));
                break;
            case 'M':
                app->setMaxDigits(atoi(optarg));
                break;
            case 'p':
                app->tryOnePassword(optarg);
                break;
            case 'R':
                app->loadRounds(atoi(optarg));
                break;
            case 's':
                app->dumpUserHash(optarg);
                exit(0);
                break;
            case 'S':
                app->loadSalt(optarg);
                break;
            case 't':
                app->setTimeout(atoi(optarg));
                break;
            case 'T':
                app->setThreads(atoi(optarg));
                break;
            case 'u':
                app->setUsername(optarg);
                break;
            case 'v':
                app->setVerbose();
                break;
            case 'V':
                app->version();
                exit(0);
                break;
            case 'x':
                app->dumpForHashcat(optarg);
                exit(0);
                break;
            case '?':
                exit(0);
            case ':':
                exit(0);
        }
    }

    // Register signal handler
    signal(SIGINT, handleSigint);

    std::thread sigWatch(signalWatcher, app);
    sigWatch.detach();

    // Let's get funky.
    app->run();

    // Cleanup
    delete app;

    return 0;
}
