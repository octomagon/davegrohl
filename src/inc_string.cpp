#include "inc_string.h"

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



// Set the IncString's value from a base 10 number.
// For example:
//    myIncString = 69;

IncString& IncString::operator=(long double num){
    int x = 0;
    long double total = 0;
    
    for (x = 0; (total + pow(base, x)) <= num; x++) {
        total = total + pow(base, x);
        value[x] = charset[0];
    }
    value[x] = '\0';
    
    while (x >= 0) {
        int i;
        for (i = 1; (i*pow(base, x) + total) <= num; i++) {
            value[x] = charset[i];
        }
        total = total + (i-1)*pow(base, x);
        x--;
    }
    
    length = strlen((char *)value);
    
    return *this;
}

// Typecast overloads so you can do stuff like this:
// printf("iString: %s\n", (const char *)iString);

IncString::operator const char*(){ return (char *)value; }
IncString::operator char*(){ return (char *)value; }
IncString::operator long double(){
    long double arr[128], total = 0;
    
    for (int i = 0; i < strlen((char *)value); i++) {
        arr[i] = (num4char(value[i]) * pow(strlen(charset), i));
    }
    
    for (int i = 0; i < strlen((char *)value); i++) {
        total = total + arr[i];
    }
    
    return total;
}


// Set the IncStrings base and character set.
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
    
    // Set the nextchar array.
    for (int i = 0; i < strlen(charset); i++) {
        if (i == (strlen(charset) - 1)) {
            nextchar[(int)charset[i]] = charset[0];
        } else {
            nextchar[(int)charset[i]] = charset[i+1];
        }
    }
    
    base = strlen(charset);
}

void IncString::zero(int digits){
    
    for (int x = 0;x < digits; x++){
        value[x] = charset[0];
    }
    value[digits] = '\0';
    
    length = strlen((char *)value);
    
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
    
    for (x = 0; (total + pow(base, x)) <= num; x++) {
        total = total + pow(base, x);
        value[x] = charset[0];
    }
    value[x] = '\0';
    
    while (x >= 0) {
        int i;
        for (i = 1; (i*pow(base, x) + total) <= num; i++) {
            value[x] = charset[i];
        }
        total = total + (i-1)*pow(base, x);
        x--;
    }
    
    length = strlen((char *)value);
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
    
    
    for (i = 0; i < min; i++) {
        aString[i] = charset[0];
    }
    aString[min] = '\0';
    
    bottom = num4str(aString);
    
    range = top - bottom;
    
    return range;
}


// Non member overloads

std::ostream& operator<<(std::ostream& os, const IncString& is){
    os << is.value;
    return os;
}


