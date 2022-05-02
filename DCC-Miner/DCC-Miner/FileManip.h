#ifndef filemanip_h
#define filemanip_h

#include <algorithm> 
#include <cctype>
#include <locale>
#include "extlibs/elzip/include/elzip/elzip.hpp"
#include <openssl/sha.h>
#include <sys/types.h>

int ExtractZip(std::string path, std::string saveAs);

void sha256_hash_string(unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65]);

void sha256_string(char* string, char outputBuffer[65]);

int sha256_file(char* path, char outputBuffer[65]);

int FileCount(std::string dir);

#endif
