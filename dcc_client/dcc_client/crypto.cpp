#pragma once

/*

Thanks to https://blog.karatos.in/a?ID=01650-057d7aac-eb6b-4fa4-ad47-17fd98e05538 !!
	The only place I could find a good and easy example
	of openssl asymmetric dual-key encryption!

*/

#include "crypto.h"



static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string encode64(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}
std::string decode64(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}


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


	std::vector<std::string> wordlist;
	std::string line;
	std::ifstream fin("./bip39.txt");
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

//Private key encryption  
std::string rsa_pri_encrypt(const std::string& clearText, const std::string& priKey)
{
	std::string strRet;
	try
	{

		RSA* rsa = NULL;
		BIO* keybio = BIO_new_mem_buf((unsigned char*)priKey.c_str(), -1);
		//There are three methods here
		//1, read the key pair generated in the memory, and then generate rsa from the memory
		//2, Read the key pair text file generated in the disk, and generate rsa from the memory
		//3. Generate rsa directly from reading the file pointer
		rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

		int len = RSA_size(rsa);
		char* encryptedText = (char*)malloc(len + 1);
		memset(encryptedText, 0, len + 1);

		//Encryption function
		int ret = RSA_private_encrypt(strlen(clearText.c_str()), (unsigned char*)(clearText.c_str()), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
		if (ret >= 0)
			strRet = std::string(encryptedText, ret);

		//release memory
		free(encryptedText);
		BIO_free_all(keybio);
		RSA_free(rsa);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

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

//public key decryption  
std::string rsa_pub_decrypt(const std::string& cipherText, const std::string& pubKey)
{
	std::string strRet;
	RSA* rsa = RSA_new();
	BIO* keybio;
	keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);
	rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

	int len = RSA_size(rsa);
	char* decryptedText = (char*)malloc(len + 1);
	memset(decryptedText, 0, len + 1);

	//Decryption function
	int ret = RSA_public_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);
	if (ret >= 0)
		strRet = std::string(decryptedText, ret);

	//release memory
	free(decryptedText);
	BIO_free_all(keybio);
	RSA_free(rsa);

	return strRet;
}

// Fill the buffer with random numbers 0-9.  For each byte in the buffer, we generate
// a random number and clamp it to the range 0-9.
int stdlib_rand_numeric_string(char* buf, int num)
{
	for (int index = 0; index < num; index++)
	{
		buf[index] = (rand() % 9) + '0';
	}
	return 1;
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

// Convert the SHA unsigned char* <hash> into a hex encoding, outputing into char[] <outputBuffer>
void cstr_to_hexstr(unsigned char* str, int clen, char outputBuffer[])
{
	//int i = 0;
	for (int i = 0; i < clen; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", str[i]);
		//printf("%i+%s\n",i,outputBuffer);
	}
	//outputBuffer[clen*2-1] = 0;
}

// Convert the SHA unsigned char* <hash> into a hex encoding, outputting into char[] charArray64
void cstr_to_hexstr(unsigned char* str, int clen)
{
	//int i = 0;
	for (int i = 0; i < clen; i++)
	{
		sprintf(charArray64 + (i * 2), "%02x", str[i]);
	}
	//charArray64[clen*2-1] = 0;
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
		charArray[index / 2] = static_cast<char>(std::stoi(hexString.substr(index, 2), nullptr, 16));
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

void sha1_str(char* str, unsigned char outputBuffer[20]) {
	unsigned char* iBuf = (unsigned char*)str;

	SHA1(iBuf, strlen((char*)iBuf), outputBuffer);

	int i;
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
