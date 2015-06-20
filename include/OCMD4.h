#ifndef DAVEGROHL_INCLUDE_OCMD4_H_
#define DAVEGROHL_INCLUDE_OCMD4_H_


#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>

// El Capitan drops support for openssl
#include <Availability.h>
#ifdef MAC_OS_X_VERSION_10_11
        #include <apr-1/apr_md4.h>
#else
        #include <openssl/md4.h>
#endif

#include <CommonCrypto/CommonDigest.h>
#import <Foundation/Foundation.h>

using namespace std;

u_int16_t ByteSwapInt16(u_int16_t value);
void CStringToUnicode(char *cstr, u_int16_t *unicode);
void MD4Encode(unsigned char *output, const unsigned char *input, unsigned int len);
void CalculateSMBNTHash(const char *utf8Password, unsigned char outHash[16]);


#endif // DAVEGROHL_INCLUDE_OCMD4_H_
