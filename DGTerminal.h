#import <termios.h>
#import <fcntl.h>

struct DGTerm{
    struct termios old_flags, new_flags;
    int ttyfd;
    
    void noecho();
    void prompt();
    void reset();
    
};

