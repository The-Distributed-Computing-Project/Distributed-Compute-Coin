
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

void sha256_hash_string(unsigned char hash[SHA256_DIGEST_LENGTH], char outputBuffer[65])
{
	int i = 0;

	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}

	outputBuffer[64] = 0;
}


void sha256_string(char* string, char outputBuffer[65])
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, string, strlen(string));
	SHA256_Final(hash, &sha256);
	int i = 0;
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}
	outputBuffer[64] = 0;
}

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