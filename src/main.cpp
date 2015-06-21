#include "main.h"

#define VERS "2.11"
#define BUILD "2B010"

#define BUFF_SIZE 2048


int help(void){
    
    cout << endl;
    cout << "Dave Grohl " << VERS << endl;
    cout << endl;
    
    cout << "  -u,  --user=USERNAME      Crack a user's password.\n";
    cout << "  -v,  --verbose            Show guessed passwords.\n";
    cout << "  -d,  --dictionary         Dictionary attack only.\n";
    cout << "  -i,  --incremental        Incremental attack only.\n";
    cout << "  -p,  --plist=PLIST        Crack a password from a user plist.\n";
    cout << "  -m,  --min                Minimum number of digits for the incremental attack.\n";
    cout << "  -M,  --max                Maximum number of digits for the incremental attack.\n";
    cout << "  -c,  --characters=CHARS   Define your own custom character set to use.\n";
    cout << "  -C,  --char-set=SET       Use one of these predefined character sets.\n";
    cout << "                  09        The ten arabic numerals\n";
    cout << "                  az        The lower-case alphabet\n";
    cout << "                  az09      The lower-case alphabet with numerals (default)\n";
    cout << "                  azAZ09    The full alphabet with numbers\n";
    cout << "                  all       The full alphabet with numbers and symbols\n";
    cout << endl;
    cout << "  -D,  --distributed        Start cracking on all available DaveGrohl servers.\n";
    cout << "  -S,  --server             Run in server mode.\n";
    cout << "  -P,  --port=PORT          Run server on a the specified port.\n";
    cout << endl;
    cout << "  -s,  --shadow=USERNAME    Dump a user's ShadowHashData.\n";
    cout << "  -j,  --john=USERNAME      Dump a user's hash formatted for John the Ripper.\n";
    cout << "  -l,  --list=USERNAME      List the used hash types for a user.\n";
    cout << "  -h,  --help               Show this help text ...but you knew that.\n";
    cout << endl;
    
    return 0;
}

int setPredefined(char *charset, const char *pd){
    
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

int about(void){
    
    cout << endl;
    cout << "Dave Grohl " << VERS << ", (" << BUILD << ") " << endl;
    cout << "davegrohl.org\n\n";
    
    
    return 0;
}



int main(int argc, char * argv[]){
    // @autoreleasepool{
    
        string optstring;
        crack_t theParams;
        OCShadowHashData * theSHD = [[OCShadowHashData alloc] init];
        DGController * theController = [[DGController alloc] init];
        BOOL toCrack = NO, toServe = NO;
        int portNum = 6090;
        
        set_terminate(bailout);
        
        theParams.slots = [[NSProcessInfo processInfo] activeProcessorCount];
        strcpy(theParams.charset, "aeorisn1tl2md0cp3hbuk45g9687yfwjvzxq");
        
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
                case 'a':
                    about();
                    exit(0);
                case 'b':
                    theParams.batchSize = atoi(optarg);
                    break;
                case 'c':
                    strcpy(theParams.charset, optarg);
                    break;
                case 'C':
                    setPredefined(theParams.charset, optarg);
                    break;
                case 'd':
                    theParams.dictionary = 1;
                    break;
                case 'D':
                    theParams.distributed = true;
                    break;
                case 'f':
                    [theSHD readJohnPasswdFile:[NSString stringWithUTF8String:optarg]];
                    // [theSHD dumpShadowHashData];
                    [theSHD exitWithErrorIfNotLoaded];
                    toCrack = YES;
                    break;
                case 'g':
                    printf("There are four lights!\n");
                    break;
                case 'h':
                    help();
                    exit(0);
                case 'i':
                    theParams.incremental = 1;
                    break;
                case 'j':
                    [theSHD setTheUser:[NSString stringWithUTF8String:optarg]];
                    [theSHD exitWithErrorIfNotLoaded];
                    [theSHD dumpForJohn];
                    break;
                case 'I':
                    //genHash.rounds = atoi(optarg);
                    break;
                case 'l':
                    [theSHD setTheUser:[NSString stringWithUTF8String:optarg]];
                    [theSHD exitWithErrorIfNotLoaded];
                    [theSHD dumpHashTypes];
                    break;
                case 'M':
                    theParams.max = atoi(optarg);
                    break;
                case 'm':
                    theParams.min = atoi(optarg);
                    break;
                case 'P':
                    portNum = atoi(optarg);
                    break;
                case 'p':
                    [theSHD setPlistFile:[NSString stringWithUTF8String:optarg]];
                    [theSHD exitWithErrorIfNotLoaded];
                    toCrack = YES;
                    break;
                case 'S':
                    toServe = YES;
                    break;
                case 's':
                    [theSHD setTheUser:[NSString stringWithUTF8String:optarg]];
                    [theSHD exitWithErrorIfNotLoaded];
                    [theSHD dumpShadowHashData];
                    return 0;
                case 't':
                    theParams.slots = atoi(optarg);
                    // Was: theParams.totalThreads = atoi(optarg);
                    break;
                case 'u':
                    [theSHD setTheUser:[NSString stringWithUTF8String:optarg]];
                    [theSHD exitWithErrorIfNotLoaded];
                    toCrack = YES;
                    break;
                case 'v':
                    theParams.verbose = YES;
                    break;
                case '?':
                    exit(0);
                case ':':
                    exit(0);
            }
        }
        
        
        if (argc == 1) {  // No arguments
            printf("\n");
            // prompt(&theParams, theSHD);
            help();
            exit(0);
        }
        
        if (toServe == YES || toCrack == YES) {
            std::thread statusWatcher(updateStatus);
            
            if (toServe == YES) {
                while (1) {
                    server(portNum);
                    printf("Second run\n");
                }
            }
            
            if (toCrack == YES) {
                hashData_t theHashData;
                
                loadHashForCracking(&theHashData, theSHD);
                [theController startCracking:&theHashData withOptions:&theParams];
            }
            
            statusWatcher.join();
        }
    //}
    
    return 0;
}


