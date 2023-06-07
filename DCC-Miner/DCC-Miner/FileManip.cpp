
#include <iostream>
//#include "Console.cpp"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "FileManip.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>

//#include <dirent.h>

int ExtractZip(std::string path, std::string saveAs)
{
	//try
	//{
	//    elz::extractZip(path, saveAs);
	//}
	//catch (const std::exception&)
	//{
	//    return 1;
	//}
	return 0;
}

// Convert the SHA-256 char[] <hash> into a hex encoding, outputing into char[] <outputBuffer>
void sha256_hash_string(unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65])
{
	int i = 0;

	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}

	outputBuffer[64] = 0;
}

// Calculate the SHA-256 hash of char* <str>, and place the hex encoded version into char[] <outputBuffer>
void sha256_string(char* str, char outputBuffer[65])
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str, strlen(str));
	SHA256_Final(hash, &sha256);
	int i = 0;
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}
	outputBuffer[64] = 0;
}

// Calculate the SHA-256 hash of char* <str>, and place the raw version into unsigned char[] outputBuffer
void sha256_full_cstr(char* str, unsigned char outputBuffer[SHA256_DIGEST_LENGTH])
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str, strlen(str));
	SHA256_Final(outputBuffer, &sha256);
	//outputBuffer = (char*)hash;
}

char charArray[32];
char charArray64[65];

// Convert the SHA-256 unsigned char* <hash> into a hex encoding, outputing into char[] <outputBuffer>
void cstr_to_hexstr(unsigned char* str, int clen, char outputBuffer[65])
{
	//int i = 0;
	for (int i = 0; i < clen; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", str[i]);
	}
	outputBuffer[64] = 0;
}

// Convert the SHA-256 unsigned char* <hash> into a hex encoding, outputing into char[] <outputBuffer>
char* cstr_to_hexstr(unsigned char* str, int clen)
{
	//int i = 0;
	for (int i = 0; i < clen; i++)
	{
		sprintf(charArray64 + (i * 2), "%02x", str[i]);
	}
	charArray64[64] = 0;
}

// Function to convert a hexadecimal string into a char*
char* hexstr_to_cstr(const std::string& hexString) {
	size_t length = hexString.length();
	//size_t charArrayLength = length / 2 + length % 2; // Number of chars needed to represent the hex string
	//char* charArray = new char[32]; // +1 for the null terminator

	size_t index = 0;
	size_t charIndex = 0;
	//if (length % 2 != 0) {
	//	// If the hex string has an odd length, handle the first digit separately
	//	charArray[charIndex++] = static_cast<char>(std::stoi(hexString.substr(index, 1), nullptr, 16));
	//	index++;
	//}

	// Convert the remaining digits of the hex string
	while (index < length) {
		charArray[index/2] = static_cast<char>(std::stoi(hexString.substr(index, 2), nullptr, 16));
		index += 2;
	}


	return charArray;
}

//unsigned char* hexstr_to_cstr(char* str, int clen)
//{
//	unsigned char decVer[clen];
//	int x;
//	sscanf(str, "%02x", &x);
//	//int i = 0;
//	for (int i = 0; i < clen; i++)
//	{
//		sprintf(outputBuffer + (i * 2), "%02x", str[i]);
//	}
//	outputBuffer[64] = 0;
//}

int sha256_file(char* path, char outputBuffer[65])
{
	FILE* file = fopen(path, "rb");
	if (!file) return -534;

	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	unsigned char* buffer = (unsigned char*)malloc(bufSize);
	int bytesRead = 0;
	if (!buffer) return ENOMEM;
	while ((bytesRead = fread(buffer, 1, bufSize, file)))
	{
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);

	sha256_hash_string(hash, outputBuffer);
	fclose(file);
	free(buffer);
	return 0;
}

int FileCount(std::string dir)
{
	auto dirIter = std::filesystem::directory_iterator(dir);
	int fileCount = 0;

	for (auto& entry : dirIter)
	{
		if (entry.is_regular_file())
		{
			++fileCount;
		}
	}

	return fileCount;
}


bool generate_key()
{
	int ret = 0;
	RSA* r = NULL;
	BIGNUM* bne = NULL;
	BIO* bp_public = NULL, * bp_private = NULL;

	int bits = 2048;
	unsigned long e = RSA_F4;

	// 1. generate rsa key
	bne = BN_new();
	ret = BN_set_word(bne, e);
	if (ret != 1) {
		goto free_all;
	}

	r = RSA_new();
	ret = RSA_generate_key_ex(r, bits, bne, NULL);
	if (ret != 1) {
		goto free_all;
	}

	// 2. save public key
	bp_public = BIO_new_file("public.pem", "w+");
	ret = PEM_write_bio_RSAPublicKey(bp_public, r);
	if (ret != 1) {
		goto free_all;
	}

	// 3. save private key
	bp_private = BIO_new_file("private.pem", "w+");
	ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

	// 4. free
free_all:

	BIO_free_all(bp_public);
	BIO_free_all(bp_private);
	RSA_free(r);
	BN_free(bne);

	return (ret == 1);
}
