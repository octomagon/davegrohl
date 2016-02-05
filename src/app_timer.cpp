#include "app_timer.h"

void AppTimer::start(){
    running = true;
    gettimeofday(&tstart, NULL);
}

void AppTimer::stop(){
    gettimeofday(&tdone, NULL);
    running = false;
}


long double AppTimer::elapsedSeconds(){
    gettimeofday(&tnow, NULL);
    
    // Return 0 if the timer wasn't started.
    if (tstart.tv_sec == 0 && tstart.tv_usec == 0) {
        return 0;
    }
    
    double secs;
    long int usecs;
    
    if (running){
        secs = difftime(tnow.tv_sec, tstart.tv_sec);
        usecs = tnow.tv_usec - tstart.tv_usec;
    } else {
        secs = difftime(tdone.tv_sec, tstart.tv_sec);
        usecs = tdone.tv_usec - tstart.tv_usec;
    }

    return secs + (usecs * 0.000001);
}


std::string AppTimer::elapsedWithColons(){
    auto secs = elapsedSeconds();
    auto mins = 0, hours = 0;
    
    if (secs > 3600){
        hours = secs / 3600;
        secs = secs - (hours * 3600);
    }
    
    if (secs > 60){
        mins = secs / 60;
        secs = secs - (mins * 60);
    }
    
    std::stringstream ss;
    
    ss << std::setfill('0');
    ss << std::setw(4) << hours << ":";
    ss << std::setw(2) << mins << ":";
    ss << std::setw(2) << static_cast<long int>(secs) << "";
    
    return ss.str();
}


bool AppTimer::isRunning(){
    return running;
}