#ifndef APP_TIMER_H_
#define APP_TIMER_H_

#include <string>
#include <iomanip>
#include <sstream>

extern "C" {
    #include <sys/time.h>
}


class AppTimer{
private:
    struct timeval tstart, tdone, tnow;
    bool running{ false };
    
public:
    void start();
    void stop();
    long double elapsedSeconds();
    std::string elapsedWithColons();
    
    bool isRunning();
};


#endif // APP_TIMER_H_