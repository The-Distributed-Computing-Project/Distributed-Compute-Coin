#ifndef filemanip_h
#define filemanip_h

#include <algorithm> 
#include <cctype>
#include <locale>
//#include "extlibs/elzip/include/elzip/elzip.hpp"
#include <openssl/sha.h>
#include <sys/types.h>

int ExtractZip(std::string path, std::string saveAs);

void sha256_hash_string(unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65]);

void sha256_string(char* string, char outputBuffer[65]);
void sha256_full_cstr(char* string, unsigned char outputBuffer[SHA256_DIGEST_LENGTH]);

void cstr_to_hexstr(unsigned char* str, int clen, char outputBuffer[65]);
char* cstr_to_hexstr(unsigned char* str, int clen);
char* hexstr_to_cstr(const std::string& hexString);

int sha256_file(char* path, char outputBuffer[65]);

int FileCount(std::string dir);

bool generate_key();

#endif
