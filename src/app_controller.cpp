#include "app_controller.h"

AppController::AppController(int argc, char * argv[]){
    // Get # of available CPU cores.
    options.cores = sysconf(_SC_NPROCESSORS_ONLN);

    // Get uname() data.
    struct utsname unameData;
    uname(&unameData);

    kernel = unameData.sysname;
    hostname = unameData.nodename;
    kernelVersion = unameData.release;
    arch = unameData.machine;

    // See if we're running as root.
    if (geteuid() == 0) {
        root = true;
    }

    // Read the current termios settings.
    tcgetattr(STDIN_FILENO, &old_flags);

    // Override found password function
    cracker.foundIt = foundPassword;
}



// ----------------------------------------------------------------------------
//
//     Da Main BIZ-NASTY
//
//  This is the where the main biz-nasty of this program happens.
//  This function is run at the end of main() and it decides the starting
//  behavior and run mode of dave.
// ----------------------------------------------------------------------------

void AppController::run(){

    if (cracker.theHash.isEmpty() == false) {
        cracker.theHash.detectType();
    } else {
        if (options.username.empty() == false) {
            hashDataForUser(options.username, &cracker.theHash);
        }
    }

    cracker.loadOptions(options);

    // Check user supplied options for errors
    std::string errMsg;

    if (-1 == cracker.checkOptions(errMsg)) {
        std::cout << errMsg << std::endl;
        return;
    }

    switch (runMode) {
        case kOneShot:
            oneShot();
            break;
        case kStandalone:
            standalone();
            break;
        default:
            help();
            break;
    }
}


// ----------------------------------------------------------------------------
//  Run dave in standalone mode.
//  This will run the standard attacks on a username or filename
//
// ----------------------------------------------------------------------------

void AppController::standalone(){

    terminalNoEcho();
    std::thread userInput(watchForKeyboardHits, this);
    userInput.detach();

    startBanner();
    cracker.start();

    cracker.joinThreads();
    terminalReset();
    printReport();
}


// ----------------------------------------------------------------------------
//  Run dave in one shot mode.
//  Only try one password given with a CLI flag.
//
// ----------------------------------------------------------------------------

void AppController::oneShot(){
    if (cracker.tryOnePassword()) {
        std::cout << "Correct password" << std::endl;
        exit(0);
    } else {
        std::cout << "Bad password" << std::endl;
        exit(-1);
    }
}


void AppController::daemonize(){
    std::cout << "daemonize..." << std::endl;
}



void AppController::onlyRoot(){
    if (root == false) {
        std::cout << "You must be root." << std::endl;
        exit(0);
    }
}

void AppController::bailout(){
    std::cout << "\n-- Stopping" << std::endl;
    cracker.timer.stop();
    cracker.stop();
}



// ----------------------------------------------------------------------------
//
//  UI Functions
//
// ----------------------------------------------------------------------------


int AppController::help(void){
    version();
    std::cout << std::endl;

    // Comments are yet to be implemented features.

    std::cout << "  -c,  --characters=CHARS   Define your own custom character set to use.\n";
    // std::cout << "  -C,  --char-set=SET       Use one of these predefined character sets.\n";
    // std::cout << "                  09        The ten arabic numerals\n";
    // std::cout << "                  az        The lower-case alphabet\n";
    // std::cout << "                  az09      The lower-case alphabet with numerals (default)\n";
    // std::cout << "                  azAZ09    The full alphabet with numbers\n";
    // std::cout << "                  all       The full alphabet with numbers and symbols\n";
    std::cout << "  -d,  --dictionary         Dictionary attack only.\n";
    // std::cout << "  -f,  --file=FILENAME      Crack a password from a specific file.\n";
    std::cout << "  -h,  --help               Show this help text.\n";
    std::cout << "  -i,  --incremental        Incremental attack only.\n";
    std::cout << "  -I,  --info               Show system info.\n";
    // std::cout << "  -j,  --john=USERNAME      Dump a user's hash formatted for John the Ripper.\n";
    std::cout << "  -m,  --min                Minimum number of digits for the incremental attack.\n";
    std::cout << "  -M,  --max                Maximum number of digits for the incremental attack.\n";
    std::cout << "  -s,  --shadow=USERNAME    Dump a user's ShadowHashData.\n";
    std::cout << "  -t,  --timeout=SECONDS    Give up after a number of seconds.\n";
    std::cout << "  -T,  --threads=THREADS    Specify number of threads.\n";
    std::cout << "  -u,  --user=USERNAME      Crack a user's password.\n";
    std::cout << "  -v,  --verbose            Show guessed passwords.\n";
    std::cout << "  -V,  --version            Show version info.\n";
    std::cout << "  -x,  --hashcat=USERNAME   Dump a user's ShadowHashData formatted for Hashcat.\n";
    std::cout << std::endl;

    return 0;
}



int AppController::version(void){
    std::cout << "\n";
    std::cout << "    DaveGrohl " << VERS << "\n\n";
    std::cout << "A password cracker for OS X\n";
    std::cout << "http://www.davegrohl.org\n\n";

    // This may be a bit sloppy
    std::cout << "    Built with:" << std::endl;
#ifdef USE_COMMON_CRYPTO
    std::cout << "CommonCrypto" << std::endl;
#endif
#ifdef USE_OPENSSL
    std::cout << OPENSSL_VERSION_TEXT << std::endl;
#endif
#ifdef USE_MBEDTLS
    std::cout << MBEDTLS_VERSION_STRING_FULL << std::endl;
#endif

    return 0;
}

void AppController::startBanner(){
    std::cout << "-- Loaded " << prettyHashType(&cracker.theHash) << std::endl;
    std::cout << "-- Starting attack" << std::endl;
}


void AppController::printUpdate(){

    if (firstUpdate) {
        std::cout << "\n      TIME             GUESSES" << std::endl;
        firstUpdate = false;
    }

    // Elapsed time.
    std::cout << cracker.timer.elapsedWithColons();
    std::cout << std::setfill (' ') << std::setw (20);
    std::cout << cracker.guesses << " ";

    // Print current guesses for each thread.
    for(int i = 0; i < cracker.dThreads.size(); i++){
        std::cout << "(" << cracker.guess[i] << ") ";
    }

    for(int i = 0; i < cracker.iThreads.size(); i++){
        std::cout << "[" << cracker.iStr[i].value << "] ";
    }

    std::cout << std::endl;
}

void AppController::printReport(){
    if (cracker.timer.isRunning()) {
        cracker.timer.stop();
    }

    auto secs = cracker.timer.elapsedSeconds();

    if (!cracker.winner.empty()) {
        std::cout << "-- (" << cracker.winner << " attack)" << std::endl;
    }

    std::cout << "\nFinished in ";
    std::cout << std::fixed;
    std::cout << std::setprecision(3) << secs;
    std::cout << " seconds / " << cracker.guesses << " total guesses..." << std::endl;
    std::cout << std::setprecision(2) << cracker.guesses/secs;
    std::cout << " guesses per second." << std::endl;
}




void AppController::watchForKeyboardHits(AppController *anApp){
    int ch;

    while (1) {
        ch = (getc(stdin) - 48);

        // -21 is for an escape sequence where the tty sends 3
        // characters in quick succession.  As of right now,
        // we don't care about escape sequences, so all of this
        // data is thrown away.

        if (ch == -21){
            getc(stdin);
            getc(stdin);
        }

        anApp->printUpdate();
    }
}

void AppController::foundPassword(const char *msg){
    std::cout << "\n-- Found password : '\033[22;31m" << msg << "\033[22;0m'" << std::endl;
}


void AppController::dumpSystemInfo(){
    onlyRoot();

    std::cout << "Kernel:    " << kernel << " " << kernelVersion << std::endl;
    std::cout << "Hostname:  " << hostname << std::endl;
    std::cout << "Arch:      " << arch << std::endl;
    std::cout << "Cores:     " << options.cores << std::endl;
}

void AppController::dumpUserHash(std::string user){
    onlyRoot();
    dumpHashData(user);
}

void AppController::dumpForHashcat(std::string user){
    onlyRoot();
    dumpHashcat(user);
}

void AppController::setCharSet(std::string charset){
    options.charset = charset;
}

void AppController::setMinDigits(int min){
    options.min = min;
}

void AppController::setMaxDigits(int max){
    options.max = max;
}

void AppController::setIncremental(){
    options.incremental = true;
}

void AppController::setDictionary(){
    options.dictionary = true;
}

void AppController::setUsername(std::string username){
    options.username = username;

    if (runMode == kNone) {
        runMode = kStandalone;
    }
}

void AppController::setVerbose(){
    options.verbose = true;
}

void AppController::setTimeout(int timeout){
    options.timeout = timeout;
}

void AppController::setThreads(int threads){
    options.cores = threads;
}


// ----------------------------------------------------------------------------
//
//  One Shot functions
//
// ----------------------------------------------------------------------------

void AppController::tryOnePassword(std::string password){
    runMode = kOneShot;
    options.oneTryPW = password;
}

int AppController::loadHash(std::string hash){
    cracker.theHash.hashLength = hexstring2uchar(hash, cracker.theHash.hash);
    return 0;
}

int AppController::loadSalt(std::string salt){
    cracker.theHash.saltLength = hexstring2uchar(salt, cracker.theHash.salt);
    return 0;
}

int AppController::loadRounds(int rounds){
    cracker.theHash.rounds = rounds;
    return 0;
}


// ----------------------------------------------------------------------------
//
//  Terminal IO functions
//
// ----------------------------------------------------------------------------

void terminalNoEcho(){
    new_flags = old_flags;
    new_flags.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    new_flags.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
    new_flags.c_cflag |= (CS8);
    new_flags.c_cc[VMIN] = 1;
    new_flags.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_flags);
}


void terminalReset(){
    tcsetattr(STDIN_FILENO, TCSANOW, &old_flags);
}
