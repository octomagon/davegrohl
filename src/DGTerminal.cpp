#include "DGTerminal.h"

void DGTerm::noecho(){
    ttyfd = -1;
    
    if ((ttyfd = open("/dev/tty", O_RDONLY | O_NONBLOCK)) < 0){
        return;
    }
    
    tcgetattr(ttyfd, &old_flags);
    
    new_flags = old_flags;
    new_flags.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    new_flags.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN);
    new_flags.c_cflag |= (CS8);
    new_flags.c_cc[VMIN] = 1;
    new_flags.c_cc[VTIME] = 0;
    
    tcsetattr(ttyfd, TCSANOW, &new_flags);
}

void DGTerm::prompt(){
    ttyfd = -1;
    
    if ((ttyfd = open("/dev/tty", O_RDONLY | O_NONBLOCK)) < 0){
        return;
    }
    
    tcgetattr(ttyfd, &old_flags);
    
    new_flags = old_flags;
    new_flags.c_iflag &= ~(ISTRIP | INPCK);
    new_flags.c_lflag &= ~(EXTPROC);
    // new_flags.c_cflag |= (CSTOPB);
    //new_flags.c_cc[VMIN] = 1;
    //new_flags.c_cc[VTIME] = 0;
    
    tcsetattr(ttyfd, TCSANOW, &new_flags);
}


void DGTerm::reset(){
    if (ttyfd != 0) {
        tcsetattr(ttyfd, TCSANOW, &old_flags);
    }
}