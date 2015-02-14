#include <iostream>
#include <fstream>
#include <getopt.h>
#import <limits.h>
#import <Foundation/Foundation.h>

#import "DGController.h"
#import "DGAttack.h"
#import "OCHashing.h"
#import "DGServer.h"
#import "DGTime.h"
#import "DGTerminal.h"
#import "OCShadowHashData.h"
#import "OCMD4.h"
#import "IncString.h"

static struct option longopts[] = {
    { "about",          optional_argument,  NULL,   'a' },
    { "batch-size",     required_argument,  NULL,   'b' },
    { "characters",     required_argument,  NULL,   'c' },
    { "char-set",       required_argument,  NULL,   'C' },
    { "dictionary",     no_argument,        NULL,   'd' },
    { "distributed",    no_argument,        NULL,   'D' },
    { "help",           no_argument,        NULL,   'h' },
    { "file",           required_argument,  NULL,   'f' },
    { "generate",       required_argument,  NULL,   'g' },
    { "incremental",    no_argument,        NULL,   'i' },
    { "iterations",     required_argument,  NULL,   'I' },
    { "john",           required_argument,  NULL,   'j' },
    { "list",           required_argument,  NULL,   'l' },
    { "min",            required_argument,  NULL,   'm' },
    { "max",            required_argument,  NULL,   'M' },
    { "plist",          required_argument,  NULL,   'p' },
    { "port",           required_argument,  NULL,   'P' },
    { "server",         no_argument,        NULL,   'S' },
    { "shadow",         required_argument,  NULL,   's' },
    { "threads",        required_argument,  NULL,   't' },
    { "user",           required_argument,  NULL,   'u' },
    { "verbose",        no_argument,        NULL,   'v' },
    {  NULL,            0,                  NULL,    0  }
};

using namespace std;

int help(void);
int setPredefined(char *charset, const char *pd);
int about(void);
int main(int argc, char * argv[]);
