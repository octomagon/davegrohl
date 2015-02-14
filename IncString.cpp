#include "IncString.h"

void IncString::operator++(int){
    
    int digit = 0;
    while (1) {
        if (value[digit] == '\0') { // Add a digit if NULL
            value[digit] = charset[0];
            value[digit+1] = '\0';
            length++;
            break;
        } else if (value[digit] == charset[base-1]){ // Roll over to the first char
            value[digit] = charset[0];
            digit++;
        } else {
            value[digit] = nextchar[value[digit]]; // Increment digit.
            break;
        }
    }
    return;
}


void IncString::setChars(const char *input){
    char buff[1024] = {};
    bool already = 0;
    
    int y = 0;
    for (int i = 0; i < strlen(input); i++) {
        for (int x = 0; x < strlen(buff); x++) {
            if (buff[x] == input[i]) {
                already = 1;
            }
        }
        if(already != 1){
            buff[y] = input[i];
            y++;
            buff[y] = '\0';
        }
        already = 0;
    }
    
    strcpy(charset,buff);
    
    // Initialize an array of powers for the base.
    for (int i = 0; i < (sizeof(pArr)/sizeof(pArr[0])); i++) {
        pArr[i] = pow(strlen(charset), i);
    }
    
    // Set the nextchar array.
    for (int i = 0; i < strlen(charset); i++) {
        if (i == (strlen(charset) - 1)) {
            nextchar[(int)charset[i]] = charset[0];
        } else {
            nextchar[(int)charset[i]] = charset[i+1];
        }
    }
    
    // This will be left as a monument to
    // my stupidity...
    //
    // nextchar[strlen(charset)] = '\0';
    
    
    base = strlen(charset);
    
}

void IncString::zero(int digits){
    
    for (int x = 0;x < digits; x++){
        value[x] = charset[0];
    }
    value[digits] = '\0';
    
    length = strlen(value);
    
}

int IncString::num4char(char c){
    
    for (int x = 0;x < strlen(charset); x++){
        if (c == charset[x]){
            return (x+1);
        }
    }
    return -1;
}

long double IncString::num4str(const char *input){
    
    long double arr[128], total = 0;
    
    for (int i = 0; i < strlen(input); i++) {
        arr[i] = (num4char(input[i]) * pow(strlen(charset), i));
    }
    
    for (int i = 0; i < strlen(input); i++) {
        total = total + arr[i];
    }
    
    return total;
}

char IncString::char4num(int num){
    return charset[num-1];
}

void IncString::setValue4num(long double num){
    int x = 0;
    long double total = 0;
    
    for (x = 0; (total + pArr[x]) <= num; x++) {
        total = total + pArr[x];
        value[x] = charset[0];
    }
    value[x] = '\0';
    
    while (x >= 0) {
        int i;
        for (i = 1; (i*pArr[x] + total) <= num; i++) {
            value[x] = charset[i];
        }
        total = total + (i-1)*pArr[x];
        x--;
    }
    
    length = strlen(value);
}

long double IncString::rangeForMinAndMaxDigits(int min, int max){
    long double range, top, bottom;
    char aString[256];
    
    int i;
    for (i = 0; i < max; i++) {
        aString[i] = charset[base-1];
    }
    aString[max] = '\0';
    
    top = num4str(aString);
    // printf("%s:%.0f\n", aString, top);
    
    
    for (i = 0; i < min; i++) {
        aString[i] = charset[0];
    }
    aString[min] = '\0';
    
    bottom = num4str(aString);
    // printf("%s:%.0f\n", aString, bottom);
    
    range = top - bottom;
    // printf("Range:%.0f\n", range);
    
    return range;
}


