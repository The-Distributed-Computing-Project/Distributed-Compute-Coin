#ifndef CRYPTO_H
#define CRYPTO_H

/*

Thanks to https://blog.karatos.in/a?ID=01650-057d7aac-eb6b-4fa4-ad47-17fd98e05538 !!
	The only place I could find a good and easy example
	of openssl asymmetric dual-key encryption!

*/


#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/des.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <sys/types.h>

#include <openssl/engine.h>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>


static inline bool is_base64(unsigned char);

std::string encode64(unsigned char const*, unsigned int);
std::string decode64(std::string const&);
void md5(const std::string&, std::string&, std::string&);
void sha256(const std::string&, std::string&, std::string&);
std::string des_encrypt(const std::string&, const std::string&);
std::string des_decrypt(const std::string&, const std::string&);
std::string GenerateWalletPhrase();
void generateRSAKey(std::string strKey[2]);
std::string rsa_pub_encrypt(const std::string&, const std::string&);
std::string rsa_pri_encrypt(const std::string&, const std::string&);
std::string rsa_pri_decrypt(const std::string&, const std::string&);
std::string rsa_pub_decrypt(const std::string&, const std::string&);
std::vector<std::string> GenerateKeypair();
int cryptMain();
void sha256_hash_string(unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65]);
void sha256_string(char*, char outputBuffer[65]);
void sha256_full_cstr(char*, unsigned char outputBuffer[SHA256_DIGEST_LENGTH]);
void cstr_to_hexstr(unsigned char*, int, char outputBuffer[]);
void cstr_to_hexstr(unsigned char*, int);
char* hexstr_to_cstr(const std::string&);
void sha1_str(char* str, unsigned char outputBuffer[20]);
int sha256_file(char*, char outputBuffer[65]);
bool generate_key();
int stdlib_rand_numeric_string(char* buf, int num);

#endif