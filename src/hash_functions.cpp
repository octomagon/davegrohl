
#include "hash_functions.h"

int hex2uchar(std::string &hexString, uint8_t *charArray, size_t arrayLength){

    int x = 0;
    arrayLength = (hexString.length()/2);

    for(int y = 0; y < arrayLength; y++){
        sscanf(hexString.substr((y*2), 2).c_str(),"%02X", &x);
        charArray[y] = x;
    }

    return 0;
}


int SHA512Hash(unsigned char *fullHash, const char *inPassword, unsigned char *salt){
    mbedtls_sha512_context ctx;

    mbedtls_sha512_init( &ctx );
    mbedtls_sha512_starts( &ctx, 0 ); // Don't use SHA384
    mbedtls_sha512_update( &ctx, salt, 4 );
    mbedtls_sha512_update( &ctx, (unsigned char *)inPassword, strlen(inPassword) );
    mbedtls_sha512_finish( &ctx, fullHash );
    mbedtls_sha512_free( &ctx );

    return 0;
}

bool passwordMatchesCryptHash(const char password[], HashData *theHash){
    const char *result;

    result = crypt(password, theHash->cryptSalt);

    if (!result || result[0] == '*') {
        fprintf(stderr, "crypt failed.\n");
        return false;
    }

    // printf("result: %s\n", result);
    if (0 == memcmp(result, &theHash->cryptHash, strlen(theHash->cryptHash))) {
        return true;
    }else{
        return false;
    }
}


// ----------------------------------------------------------------------------
//
//   PBKDF2
//
// ----------------------------------------------------------------------------

bool passwordMatchesPBKDF2Hash(const char password[], HashData *theHash){
    static __thread unsigned char hash[2048];

#ifdef USE_COMMON_CRYPTO
    CCKeyDerivationPBKDF(kCCPBKDF2, password, strlen(password), theHash->salt, 32, kCCPRFHmacAlgSHA512, theHash->rounds, hash, 128);

#else
    PKCS5_PBKDF2_HMAC(password, strlen(password), theHash->salt, 32, theHash->rounds, EVP_sha512(), 128, hash);
    /*
    static __thread mbedtls_md_context_t sha512_ctx;
    const mbedtls_md_info_t *info_sha512;

    mbedtls_md_init( &sha512_ctx );
    info_sha512 = mbedtls_md_info_from_type( MBEDTLS_MD_SHA512 );
    mbedtls_md_setup( &sha512_ctx, info_sha512, 1 );

    mbedtls_pkcs5_pbkdf2_hmac (&sha512_ctx, (const unsigned char *)password, strlen(password), theHash->salt, 32, theHash->rounds, 128, hash);

    mbedtls_md_free( &sha512_ctx );
    */
#endif

    if (memcmp(theHash->hash, hash, 128) == 0){
        return true;
    }

    return false;
}


// ----------------------------------------------------------------------------
//
//   SMB NT - MD4 Hashing functions.
//
// ----------------------------------------------------------------------------

bool passwordMatchesSMBNTHash(const char password[], HashData *theHash){
    static __thread unsigned char hash[16];

    CalculateSMBNTHash(password, hash);

    if (memcmp(theHash->hash, hash, 16) == 0){
        return true;
    }

    return false;
}

u_int16_t ByteSwapInt16(u_int16_t value){
    u_int16_t mask = value;
    mask <<= 8;
    value >>= 8;
    value |= mask;
    return value;
}

void CStringToUnicode(char *cstr, u_int16_t *unicode){
    int i;
    u_int16_t val;
    int len;

    len = strlen(cstr);

    for(i = 0; i < len; i++)
    {
        val = *cstr;
        if (BYTE_ORDER == BIG_ENDIAN)
            *unicode = ByteSwapInt16(val);
        else
            *unicode = val;
        unicode++;
        cstr++;
        if (val == 0) break;
    }
}


void MD4Encode(unsigned char *output, const unsigned char *input, unsigned int len){

#ifdef USE_COMMON_CRYPTO
    CC_MD4_CTX context = {};

    CC_MD4_Init (&context);
    CC_MD4_Update (&context, (unsigned char *)input, len);
    CC_MD4_Final (output, &context);

#else
    MD4_CTX context = {};

    MD4_Init (&context);
    MD4_Update (&context, (unsigned char *)input, len);
    MD4_Final (output, &context);


    //mbedtls_md4_context ctx;

    //mbedtls_md4_init( &ctx );
    //mbedtls_md4_starts( &ctx );
    //mbedtls_md4_update( &ctx, (unsigned char *)input, len );
    //mbedtls_md4_finish( &ctx, output );
    //mbedtls_md4_free( &ctx );

#endif

}


void CalculateSMBNTHash(const char *utf8Password, unsigned char outHash[16]){
    u_int16_t unicodeLen = 0;
    u_int16_t unicodepwd[258] = {0};
    char *password[128] = {0};
    int passLen = 0;
    //unsigned char P21[21] = {0};

    if (utf8Password == NULL || outHash == NULL) return;

    if (strlen(utf8Password) < 128)
        passLen = strlen(utf8Password);
    else
        passLen = 128;

    memmove(password, utf8Password, passLen);
    unicodeLen = strlen((char *)password) * sizeof(u_int16_t);

    CStringToUnicode((char *)password, unicodepwd);
    MD4Encode(outHash, (unsigned char *)unicodepwd, unicodeLen);
}
