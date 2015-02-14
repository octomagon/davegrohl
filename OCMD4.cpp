
#import "OCMD4.h"

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
    
    CC_MD4_CTX context = {};
    
    CC_MD4_Init (&context);
    CC_MD4_Update (&context, (unsigned char *)input, len);
    CC_MD4_Final (output, &context);
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

/*
int main(int argc, char * argv[]){

    unsigned char fullHash[16];
    char hashString[512];
    unsigned long buff[64];
    
    CalculateSMBNTHash("a",fullHash);
    dumpHash(fullHash, hashString, sizeof(fullHash));
    
    printf("%s\n", hashString);
    
    return 0;
}
*/

