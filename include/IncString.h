#ifndef OCTOMAGON_INCSTRING_H_
#define OCTOMAGON_INCSTRING_H_


#include <iostream>
#include <math.h>


class IncString{
    
    char nextchar[2048] = {};
    long double pArr[64];
    
public:
    char charset[2048] = {};
    char value[128] = {};
    uint8_t base = 0;
    uint8_t length = 0;
    
    void operator++(int);
    void setChars(const char *input);
    void zero(int digits);
    int num4char(char c);
    long double num4str(const char *input);
    char char4num(int num);
    void setValue4num(long double num);
    long double rangeForMinAndMaxDigits(int min, int max);
};


#endif // OCTOMAGON_INCSTRING_H_