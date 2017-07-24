#ifndef INCSTRING_H_
#define INCSTRING_H_


#include <iostream>
#include <math.h>
#include <cstring>

class IncString{
    
    char nextchar[2048] = {};
    
public:
    char charset[2048] = {};
    unsigned char value[128] = {};
    uint8_t base = 0;
    uint8_t length = 0;
    
    void operator++(int);
    IncString& operator=(long double num);
    operator const char*();
    operator char*();
    operator long double();
    
    void setChars(const char *input);
    void zero(int digits);
    int num4char(char c);
    long double num4str(const char *input);
    char char4num(int num);
    void setValue4num(long double num);
    long double rangeForMinAndMaxDigits(int min, int max);
};


std::ostream& operator<<(std::ostream& os, const IncString& is);

#endif // INCSTRING_H_
