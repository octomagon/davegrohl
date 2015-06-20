#ifndef DAVEGROHL_INCLUDE_OCMD4_H_
#define DAVEGROHL_INCLUDE_OCMD4_H_


#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>

#include <CommonCrypto/CommonDigest.h>
#import <Foundation/Foundation.h>

using namespace std;

u_int16_t ByteSwapInt16(u_int16_t value);
void CStringToUnicode(char *cstr, u_int16_t *unicode);
void MD4Encode(unsigned char *output, const unsigned char *input, unsigned int len);
void CalculateSMBNTHash(const char *utf8Password, unsigned char outHash[16]);


#endif // DAVEGROHL_INCLUDE_OCMD4_H_
