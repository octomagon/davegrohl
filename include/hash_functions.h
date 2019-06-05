#ifndef HASH_FUNCTIONS_H_
#define HASH_FUNCTIONS_H_

#include <iostream>
#include <string>

#include "hash_data.h"

// __has_include() is a feature of the new and awesome Clang and is
// likely not supported by older versions of GCC (< v5).  If you want
// to use GNU compilers with dave, please contact our technical
// support department via a similiar technology like Western Union
// Telegram or carrier pigeon.  (GNU is Not Usable)


// Try to use Apple Common Crypto if using OS X.
#if __APPLE__ && __has_include(<CommonCrypto/CommonKeyDerivation.h>)
    #define USE_COMMON_CRYPTO
    #include <CommonCrypto/CommonDigest.h>
    #include <CommonCrypto/CommonKeyDerivation.h>
    #include <unistd.h> // for crypt()

// If not, try OpenSSL.
#elif __has_include(<openssl/evp.h>)
    extern "C" {
        #define USE_OPENSSL
        #include <openssl/opensslv.h>
        #include <openssl/md4.h>
        #include <openssl/sha.h>
        #include <openssl/evp.h>
        #include <crypt.h>
    }
#endif

// Fallback to MbedTLS
extern "C" {
    #define USE_MBEDTLS
    #include "mbedtls/version.h"
    #include "mbedtls/md4.h"
    #include "mbedtls/sha512.h"
    #include "mbedtls/pkcs5.h"
}



int hex2uchar(std::string &hexString, uint8_t *charArray, size_t arrayLength);

int SHA512Hash(unsigned char *fullHash, const char *inPassword, unsigned char *salt);
bool passwordMatchesCryptHash(const char password[], HashData *theHash);
bool passwordMatchesSMBNTHash(const char password[], HashData *theHash);
bool passwordMatchesPBKDF2Hash(const char password[], HashData *theHash);

// SMB NT - MD4 Functions
u_int16_t ByteSwapInt16(u_int16_t value);
void CStringToUnicode(char *cstr, u_int16_t *unicode);
//void MD4Encode(unsigned char *output, const unsigned char *input, unsigned int len);
void CalculateSMBNTHash(const char *utf8Password, unsigned char outHash[16]);

#endif // HASH_FUNCTIONS_H_
