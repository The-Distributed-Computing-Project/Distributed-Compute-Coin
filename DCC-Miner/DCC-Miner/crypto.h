/*

Thanks to https://blog.karatos.in/a?ID=01650-057d7aac-eb6b-4fa4-ad47-17fd98e05538 !!
	The only place I could find a good and easy example
	of openssl asymmetric dual-key encryption!

*/

#ifndef crypto_h
#define crypto_h

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/des.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

#include <openssl/engine.h>

//#pragma comment(lib,"libcrypto.lib")

//---- md5 digest hash----//
void md5(const std::string& srcStr, std::string& encodedStr, std::string& encodedHexStr)
{
	unsigned char mdStr[33] = { 0 };
	MD5((const unsigned char*)srcStr.c_str(), srcStr.length(), mdStr);//call md5 hash
	encodedStr = std::string((const char*)mdStr);//hashed string

	char buf[65] = { 0 };
	char tmp[3] = { 0 };
	for (int i = 0; i < 32; i++)//hashed hexadecimal string 32 bytes  
	{
		sprintf(tmp, "%02x", mdStr[i]);
		strcat(buf, tmp);
	}
	buf[32] = '\0';//followed by 0, truncated from 32 bytes  
	encodedHexStr = std::string(buf);
}

////---- sha256 digest hash----// 
//void sha256(const std::string& srcStr, std::string& encodedStr, std::string& encodedHexStr)
//{
//
//	unsigned char mdStr[33] = { 0 };
//	SHA256((const unsigned char*)srcStr.c_str(), srcStr.length(), mdStr);//call sha256 hash
//	encodedStr = std::string((const char*)mdStr);//hashed string
//
//	char buf[65] = { 0 };
//	char tmp[3] = { 0 };
//	for (int i = 0; i < 32; i++)//hashed hexadecimal string 32 bytes  
//	{
//		sprintf(tmp, "%02x", mdStr[i]);
//		strcat(buf, tmp);
//	}
//	buf[32] = '\0';//followed by 0, truncated from 32 bytes  
//	encodedHexStr = std::string(buf);
//}
//---- sha256 digest hash----// 
void sha256(const std::string& srcStr, std::string& encodedStr, std::string& encodedHexStr)
{

	unsigned char mdStr[65] = { 0 };
	SHA256((const unsigned char*)srcStr.c_str(), srcStr.length(), mdStr);//call sha256 hash
	encodedStr = std::string((const char*)mdStr);//hashed string

	char buf[129] = { 0 };
	char tmp[3] = { 0 };
	for (int i = 0; i < 64; i++)//hashed hexadecimal string 32 bytes  
	{
		sprintf(tmp, "%02x", mdStr[i]);
		strcat(buf, tmp);
	}
	buf[64] = '\0';//followed by 0, truncated from 32 bytes  
	encodedHexStr = std::string(buf);
}

//---- des symmetric encryption and decryption ----// 
//Encrypted ecb mode  
std::string des_encrypt(const std::string& clearText, const std::string& key)
{
	std::string cipherText;//ciphertext

	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);

	//Construct the completed key  
	if (key.length() <= 8)
		memcpy(keyEncrypt, key.c_str(), key.length());
	else
		memcpy(keyEncrypt, key.c_str(), 8);

	//key replacement  
	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	//Cycle encryption, once every 8 bytes  
	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCiphertext;
	unsigned char tmp[8];

	for (int i = 0; i < clearText.length() / 8; i++)
	{
		memcpy(inputText, clearText.c_str() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	if (clearText.length() % 8 != 0)
	{
		int tmp1 = clearText.length() / 8 * 8;
		int tmp2 = clearText.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, clearText.c_str() + tmp1, tmp2);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);//Encryption function
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	cipherText.clear();
	cipherText.assign(vecCiphertext.begin(), vecCiphertext.end());
	return cipherText;
}

//Decrypt ecb mode  
std::string des_decrypt(const std::string& cipherText, const std::string& key)
{
	std::string clearText;//clear text  

	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);

	if (key.length() <= 8)
		memcpy(keyEncrypt, key.c_str(), key.length());
	else
		memcpy(keyEncrypt, key.c_str(), 8);

	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCleartext;
	unsigned char tmp[8];

	for (int i = 0; i < cipherText.length() / 8; i++)
	{
		memcpy(inputText, cipherText.c_str() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCleartext.push_back(tmp[j]);
	}

	if (cipherText.length() % 8 != 0)
	{
		int tmp1 = cipherText.length() / 8 * 8;
		int tmp2 = cipherText.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, cipherText.c_str() + tmp1, tmp2);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);//Decryption function
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCleartext.push_back(tmp[j]);
	}

	clearText.clear();
	clearText.assign(vecCleartext.begin(), vecCleartext.end());

	return clearText;
}

std::string GenerateWalletPhrase()
//std::vector<std::string> GenerateWalletPhrase()
{
	BIGNUM *r;
	static const char rnd_seed[] = "string to make the random number generator think it has entropy";

	r = BN_new();
	RAND_seed(rnd_seed, sizeof rnd_seed); /* or BN_generate_prime_ex may fail */

	BN_generate_prime_ex(r, 512, 0, NULL, NULL, NULL);
	std::cout << r << std::endl;

	BN_free(r);
	
	return "";
	
	//OpenSSL_add_all_algorithms();

	//ERR_load_crypto_strings();

	//ENGINE_load_dynamic();
	//ENGINE* randEngine_engine = ENGINE_by_id("./randEngine.c");

	//if (randEngine_engine == NULL)
	//{
	//	printf("Could not Load randEngine Engine!\n");
	//	exit(1);
	//}
	//printf("randEngine Engine successfully loaded\n");

	//int init_res = ENGINE_init(randEngine_engine);
	//printf("Engine name: %s init result : %d \n", ENGINE_get_name(randEngine_engine), init_res);
	//return "";



	RAND_set_rand_method(NULL);

	// Seeding, haven't tried yet...
	//static const char rnd_seed[] = "This is the seed"; // This will be replaced by the 16 word passphrase and block height
	srand(0);
	RAND_seed(rnd_seed, sizeof(rnd_seed));

	std::vector<std::string> wordlist;
	std::string line;
	std::ifstream fin("./wordlist");
	while (getline(fin, line)) {
		wordlist.push_back(line);
	}
	std::cout << wordlist.size() << std::endl;

	unsigned char buffer[128];
	int rc = RAND_pseudo_bytes(buffer, sizeof(buffer));

	int compressed[16] = { 0 };
	// Make the 128 random 0 - 256 bytes into 16 random 0 - 2048 ints
	for (int i = 0; i < 16; i++)
		for (int b = 0; b < 8; b++)
			compressed[i] += (int)buffer[i * 8 + b];

	std::string walletPhrase;
	for (int i = 0; i < 16; i++) {
		walletPhrase += wordlist[compressed[i]];
		if (i < 15)
			walletPhrase += " ";
	}

	return walletPhrase;
}



//---- rsa asymmetric encryption and decryption ----// 
#define KEY_LENGTH 2048//Key length
#define PUB_KEY_FILE "./sec/pubkey.pem"//public key path
#define PRI_KEY_FILE "./sec/prikey.pem"//private key path

//Function method to generate key pair 
void generateRSAKey(std::string strKey[2])
{
	//Public and private key pair  
	size_t pri_len;
	size_t pub_len;
	char* pri_key = NULL;
	char* pub_key = NULL;

	//Generate key pair  
	RSA* keypair = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);

	// Seeding, haven't tried yet...
	static const char rnd_seed[] = "This is the seed"; // This will be replaced by the 16 word passphrase and block height
	srand(sizeof(rnd_seed));
	RAND_seed(rnd_seed, sizeof(rnd_seed));

	BIO* pri = BIO_new(BIO_s_mem());
	BIO* pub = BIO_new(BIO_s_mem());

	PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
	PEM_write_bio_RSAPublicKey(pub, keypair);

	//Get the length  
	pri_len = BIO_pending(pri);
	pub_len = BIO_pending(pub);

	//The key pair reads the string  
	pri_key = (char*)malloc(pri_len + 1);
	pub_key = (char*)malloc(pub_len + 1);

	BIO_read(pri, pri_key, pri_len);
	BIO_read(pub, pub_key, pub_len);

	pri_key[pri_len] = '\0';
	pub_key[pub_len] = '\0';

	//Store the key pair  
	strKey[0] = pub_key;
	strKey[1] = pri_key;

	//Stored to disk (this way is stored at the beginning of begin rsa public key/begin rsa private key)
	FILE* pubFile = fopen(PUB_KEY_FILE, "w");
	if (pubFile == NULL)
	{
		assert(false);
		return;
	}
	fputs(pub_key, pubFile);
	fclose(pubFile);

	FILE* priFile = fopen(PRI_KEY_FILE, "w");
	if (priFile == NULL)
	{
		assert(false);
		return;
	}
	fputs(pri_key, priFile);
	fclose(priFile);

	//memory release
	RSA_free(keypair);
	BIO_free_all(pub);
	BIO_free_all(pri);

	free(pri_key);
	free(pub_key);
}

//The command line method generates a public and private key pair (begin public key/begin private key)
//Find the openssl command line tool and run the following
//openssl genrsa -out prikey.pem 1024 
//openssl rsa-in privkey.pem-pubout-out pubkey.pem

//Public key encryption  
std::string rsa_pub_encrypt(const std::string& clearText, const std::string& pubKey)
{
	std::string strRet;
	RSA* rsa = NULL;
	BIO* keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);
	//There are three methods here
	//1, read the key pair generated in the memory, and then generate rsa from the memory
	//2, Read the key pair text file generated in the disk, and generate rsa from the memory
	//3. Generate rsa directly from reading the file pointer
	RSA* pRSAPublicKey = RSA_new();
	rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

	int len = RSA_size(rsa);
	char* encryptedText = (char*)malloc(len + 1);
	memset(encryptedText, 0, len + 1);

	//Encryption function
	int ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
	if (ret >= 0)
		strRet = std::string(encryptedText, ret);

	//release memory
	free(encryptedText);
	BIO_free_all(keybio);
	RSA_free(rsa);

	return strRet;
}

//private key decryption  
std::string rsa_pri_decrypt(const std::string& cipherText, const std::string& priKey)
{
	std::string strRet;
	RSA* rsa = RSA_new();
	BIO* keybio;
	keybio = BIO_new_mem_buf((unsigned char*)priKey.c_str(), -1);

	//There are three methods here
	//1, read the key pair generated in the memory, and then generate rsa from the memory
	//2, Read the key pair text file generated in the disk, and generate rsa from the memory
	//3. Generate rsa directly from reading the file pointer
	rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

	int len = RSA_size(rsa);
	char* decryptedText = (char*)malloc(len + 1);
	memset(decryptedText, 0, len + 1);

	//Decryption function
	int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);
	if (ret >= 0)
		strRet = std::string(decryptedText, ret);

	//release memory
	free(decryptedText);
	BIO_free_all(keybio);
	RSA_free(rsa);

	return strRet;
}

std::vector<std::string> GenerateKeypair()
{
	std::string kp[2];
	generateRSAKey(kp);

	std::vector<std::string> vKp;

	vKp.push_back(kp[0]);
	vKp.push_back(kp[1]);

	return vKp;
}

int cryptMain()
{
	////original plaintext  
	//std::string srcText = "this is an example";

	//std::string encryptText;
	//std::string encryptHexText;
	//std::string decryptText;

	//std::cout << "=== original plaintext ===" << std::endl;
	//std::cout << srcText << std::endl;

	////md5  
	//std::cout << "=== md5 hash ===" << std::endl;
	//md5(srcText, encryptText, encryptHexText);
	//std::cout << "Summary character: " << encryptText << std::endl;
	//std::cout << "Summary String: " << encryptHexText << std::endl;

	////sha256  
	//std::cout << "=== sha256 hash ===" << std::endl;
	//sha256(srcText, encryptText, encryptHexText);
	//std::cout << "Summary character: " << encryptText << std::endl;
	//std::cout << "Summary String: " << encryptHexText << std::endl;

	////des  
	//std::cout << "=== des encryption and decryption ===" << std::endl;
	//std::string desKey = "12345";
	//encryptText = des_encrypt(srcText, desKey);
	//std::cout << "Encrypted character: " << std::endl;
	//std::cout << encryptText << std::endl;
	//decryptText = des_decrypt(encryptText, desKey);
	//std::cout << "Decryption character: " << std::endl;
	//std::cout << decryptText << std::endl;

	// RSA Example
	std::string inputString = "Hello World!";
	std::cout << "Input string: " << inputString << std::endl << std::endl;

	std::vector<std::string> keyPair = GenerateKeypair();
	std::cout << "Public key: " << std::endl;
	std::cout << keyPair[0] << std::endl;

	std::string encryptText;
	std::string encryptHexText;
	sha256(keyPair[0], encryptText, encryptHexText);
	std::string publicKeyAsAddress = encryptHexText;
	std::cout << "Address: " << std::endl;
	std::cout << publicKeyAsAddress << std::endl;

	std::cout << "Private Key: " << std::endl;
	std::cout << keyPair[1] << std::endl;

	std::string encryptedText = rsa_pub_encrypt(inputString, keyPair[0]);
	std::cout << "Encrypted text: " << std::endl;
	std::cout << encryptedText << std::endl;

	std::string decryptedText = rsa_pri_decrypt(encryptedText, keyPair[1]);
	std::cout << "Decrypted text: " << std::endl;
	std::cout << decryptedText << std::endl;

	system("pause");
	return 0;
}

#endif
