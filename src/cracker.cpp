#include "cracker.h"

Cracker::Cracker(){
    // Set default message functions.
    printm = &printMessage;
    foundIt = &foundPassword;
    
    
    // Load wordlists
    DIR *dp;
    struct dirent *entry;
    
    if((dp = opendir("./wordlists")) != NULL){
        while ((entry = readdir(dp))){
            if (entry->d_name[0] != '.') {
                wordlists.push_back(std::string("./wordlists/").append(entry->d_name));
            }
        }
        closedir(dp);
    }
}




int Cracker::loadHashData(HashData someHashData){
    theHash = someHashData;
    return 0;
}



int Cracker::loadOptions(CrackerOptions someOptions){
    // Copy the options
    options = someOptions;
    
    // Create the first IncString
    IncString aStr;
    aStr.setChars(someOptions.charset.c_str());
    aStr = 0;
    iStr.push_back(aStr);
    
    range = iStr[0].rangeForMinAndMaxDigits(options.min, options.max);
    
    
    // Set the correct hashing function.
    switch (theHash.hashType) {
        case kSMBNTHashType:
            tryPassword = &passwordMatchesSMBNTHash;
            break;
        case kCryptHashType:
            tryPassword = &passwordMatchesCryptHash;
            break;
        case kPBKDF2_SHA512HashType:
            tryPassword = &passwordMatchesPBKDF2Hash;
            break;
        default:
            tryPassword = nullptr;
            break;
    }
    
    return 0;
}


int Cracker::checkOptions(std::string& errorMessage){
    
    if (options.max == 0) {
        errorMessage = "Max cannot be zero.";
        return -1;
    }
    
    if (options.min > options.max) {
        errorMessage = "Min cannot be greater than max.";
        return -1;
    }
    
    return 0;
}


// ----------------------------------------------------------------------------
//     ___ _            _
//    / __| |_ __ _ _ _| |_
//    \__ \  _/ _` | '_|  _|
//    |___/\__\__,_|_|  \__|
//
// ----------------------------------------------------------------------------


int Cracker::start(){
    
    if (tryPassword == nullptr) {
        printm("Couldn't find the correct hashing function.");
        return -1;
    }
    
    // Copy the IncString for each incremental thread.
    while (iStr.size() < options.cores) {
        iStr.push_back(iStr[0]);
    }
    
    // If no method is specified, do both.
    if (options.incremental == false && options.dictionary == false) {
        options.incremental = true;
        options.dictionary = true;
    }
    
    // Seperate thread that stops the attack after the specified timeout.
    std::thread timeoutThread(&Cracker::stopAfterTimeout, this);
    timeoutThread.detach();
    
    // Times the attack.  Unrelated to timeout.
    timer.start();
    
    // Start incremental threads
    if (options.incremental) {
        for (int x = 0; x < options.cores; x++) {
            iThreads.push_back(std::thread(&Cracker::incrementalAttack, this, x));
        }
    }
    
    // Start dictionary threads
    if (options.dictionary) {
        for (int x = 0; x < wordlists.size(); x++) {
            dThreads.push_back(std::thread(&Cracker::dictionaryAttack, this, x));
        }
    }
    
    return 0;
}


void Cracker::stop(){
    done = true;
}


bool Cracker::isDone(){
    return done;
}

int Cracker::joinThreads(){
    for(auto &t : dThreads){ t.join(); }
    for(auto &t : iThreads){ t.join(); }
    return 0;
}

void Cracker::stopAfterTimeout(){
    if (options.timeout != 0) {
        std::this_thread::sleep_for (std::chrono::seconds(options.timeout));
        done = true;
    }
    return;
}



bool Cracker::tryOnePassword(){
    if (this->tryPassword(options.oneTryPW.c_str(), &this->theHash)) {
        return true;
    } else {
        return false;
    }
}


// ----------------------------------------------------------------------------
//       _  _   _           _     ___             _   _
//      /_\| |_| |_ __ _ __| |__ | __|  _ _ _  __| |_(_)___ _ _  ___
//     / _ \  _|  _/ _` / _| / / | _| || | ' \/ _|  _| / _ \ ' \(_-<
//    /_/ \_\__|\__\__,_\__|_\_\ |_| \_,_|_||_\__|\__|_\___/_||_/__/
//
// ----------------------------------------------------------------------------



bool Cracker::incrementalAttack(int threadID){
    int round = 0;
    
    iStr[threadID].zero(options.min);
    long double startingPoint = (long double)iStr[threadID];

    while (1) {
        iStr[threadID] = (round * batchSize * options.cores) + (threadID * batchSize) + startingPoint;
        
        // This statement is for debugging.  Using std::cout here causes a mangled spooge of
        // outputs from every thread.
        //
        // std::printf("Starting thread %d at position %.0Lf (%s)\n", threadID, (long double)iStr[threadID], (char *)iStr[threadID]);
        
        for (int i = 0; i < batchSize; i++) {
            if (strlen(iStr[threadID]) > options.max) {
                iStr[threadID] = 0;  // Empty the guess variable to show [] in the status updates.
                return false;
            }
            
            if (options.verbose) {
                // printf("%d ", threadID);
                printm((char *)iStr[threadID]);
            }
            guesses++;
            
            if (tryPassword((const char *)iStr[threadID], &theHash)) {
                timer.stop();
                stop();
                winner = "incremental";
                foundIt((char *)iStr[threadID]);
                return true;
            }
            
            if (isDone()) { return 0; }
            
            iStr[threadID]++;
        }
        round++;
    }
    return false;
}


bool Cracker::dictionaryAttack(int threadID){
    guess.resize(wordlists.size());
    
    std::ifstream myfile(wordlists[threadID]);
    
    if (myfile.is_open()){
        while ( getline (myfile, guess[threadID]) ){
            if (isDone()) { return 0; }
            if (options.verbose) {
                printm(guess[threadID].c_str());
            }
            guesses++;
            
            if (tryPassword(guess[threadID].c_str(), &theHash)) {
                timer.stop();
                stop();
                winner = "dictionary";
                foundIt(guess[threadID].c_str());
                return true;
            }
        }
        myfile.close();
    } else{
        std::cout << "Unable to open file";
    }
    
    return false;
}


// ----------------------------------------------------------------------------
//
//
//     Non-Member Functions
//
//
// ----------------------------------------------------------------------------

void printMessage(const char *msg){
    printf("%s\n", msg);
}




void foundPassword(const char *msg){
    printf("--\n");
    printf("-- Found Password: '%s'\n", msg);
    printf("--\n");
}
