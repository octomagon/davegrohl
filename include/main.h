#include <iostream>
#include <thread>

extern "C" {
    #include <getopt.h>
}

#include "app_controller.h"


static volatile sig_atomic_t sig_caught = 0;

static struct option longopts[] = {
    { "characters",     required_argument,  NULL,   'c' },
    { "char-set",       required_argument,  NULL,   'C' },
    { "daemon",         no_argument,        NULL,   'D' },
    { "dictionary",     no_argument,        NULL,   'd' },
    { "help",           no_argument,        NULL,   'h' },
    { "hash",           required_argument,  NULL,   'H' },
    { "info",           no_argument,        NULL,   'I' },
    { "incremental",    no_argument,        NULL,   'i' },
    { "min",            required_argument,  NULL,   'm' },
    { "max",            required_argument,  NULL,   'M' },
    { "password",       required_argument,  NULL,   'p' },
    { "rounds",         required_argument,  NULL,   'R' },
    { "shadow",         required_argument,  NULL,   's' },
    { "salt",           required_argument,  NULL,   'S' },
    { "timeout",        required_argument,  NULL,   't' },
    { "threads",        required_argument,  NULL,   'T' },
    { "user",           required_argument,  NULL,   'u' },
    { "verbose",        no_argument,        NULL,   'v' },
    { "version",        no_argument,        NULL,   'V' },
    { "hashcat",        required_argument,  NULL,   'x' },
    {  NULL,            0,                  NULL,    0  }
};

// This is the signal handler. It's registered with signal()
void handleSigint(int signum);

// This runs on a seperate thread wathing the signal count and informing
// the AppController class.
void signalWatcher(AppController *anApp);

int main(int argc, char * argv[]);
