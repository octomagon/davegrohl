#ifndef DAVEGROHL_INCLUDE_DGTERMINAL_H_
#define DAVEGROHL_INCLUDE_DGTERMINAL_H_


#import <termios.h>
#import <fcntl.h>

struct DGTerm{
    struct termios old_flags, new_flags;
    int ttyfd;
    
    void noecho();
    void prompt();
    void reset();
    
};


#endif // DAVEGROHL_INCLUDE_DGTERMINAL_H_