#ifndef DAVEGROHL_INCLUDE_DGTIME_H_
#define DAVEGROHL_INCLUDE_DGTIME_H_


#include <iostream>
#include <sys/time.h>
#include <math.h>

#define STR_BUFF_LEN 128

struct DGTimer {
    struct timeval tstart, tstop, tnow;
    char prettyTime[64];
    
    void start();
    float stop();
    char *elapsed();
};

struct DGTimeUnits {
    uint64_t years = 0;
    uint64_t days = 0;
    uint64_t hours = 0;
    uint64_t minutes = 0;
    uint64_t seconds = 0;
    char prettyTime[2048];
};


void secsToTimeUnits(DGTimeUnits *tu, long double total);
int intwithcommas(char theString[STR_BUFF_LEN], uint64_t theInt);


#endif // DAVEGROHL_INCLUDE_DGTIME_H_