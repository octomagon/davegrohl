#ifndef DAVEGROHL_INCLUDE_OCHASHING_H_
#define DAVEGROHL_INCLUDE_OCHASHING_H_


#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <CommonCrypto/CommonDigest.h>
#include <CommonCrypto/CommonKeyDerivation.h>
#import <Foundation/Foundation.h>

using namespace std;

int hexstring2uchar(string &hexString, uint8_t *charArray, size_t arrayLength);
int getPBKDF2Data(NSString *plistPath, uint8_t theKey[], uint8_t theSalt[], int &rounds);
int SHA1Hash(unsigned char *fullHash, const char *inPassword, unsigned char *inSalt);
int SHA512Hash(unsigned char *fullHash, const char *inPassword, unsigned char *salt);
char * dumpHex(const void *hashData, int size);


#endif // DAVEGROHL_INCLUDE_OCHASHING_H_