#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <openssl/rand.h>

// These don't need to do anything if you don't have anything for them to do.
static void stdlib_rand_cleanup() {}
static void stdlib_rand_add(const void *buf, int num, double add_entropy) {}
static int stdlib_rand_status() { return 1; }

// Seed the RNG. srand() takes an unsigned int, so we just use the first
// sizeof(unsigned int) bytes in the buffer to seed the RNG.
static void stdlib_rand_seed(const void *buf, int num)
{
    assert(num >= sizeof(unsigned int)); /// Use if statement, if u don't want to terminate ur app
    srand(*((unsigned int *)buf));
}

// Fill the buffer with random bytes.  For each byte in the buffer, we generate
// a random number and clamp it to the range of a byte, 0-255.
static int stdlib_rand_bytes(unsigned char *buf, int num)
{
    for(int index = 0; index < num; ++index)
    {
        buf[index] = rand() % 256;
    }
    return 1;
}

// Create the table that will link OpenSSL's rand API to our functions.
static RAND_METHOD stdlib_rand_meth = {
    stdlib_rand_seed,
    stdlib_rand_bytes,
    stdlib_rand_cleanup,
    stdlib_rand_add,
    stdlib_rand_bytes,
    stdlib_rand_status
};

// This is a public-scope accessor method for our table.
RAND_METHOD *RAND_stdlib() { return &stdlib_rand_meth; }

void createRSAKey() {
    // Intilize data
    int bits = 2048; // 512, 1024, 2048, 4096
    std::string passW = "password";    
    unsigned int randBuff = sizeof(passW);
    unsigned int randNum[4];

    // Create BIGNUM
    BIGNUM *bn = BN_new();
    BN_set_word(bn, RSA_3);

    // Create RSA
    RSA *rsa = RSA_new();

    // Connect algorithms, another way it crashes    
    OpenSSL_add_all_algorithms();

    // Randomize bytes
    RAND_set_rand_method(RAND_stdlib());
    RAND_seed(&randBuff, sizeof(randBuff));
    RAND_bytes(reinterpret_cast<unsigned char *>(&randNum[0]), sizeof(randNum));

    if(RSA_generate_key_ex(rsa, bits, bn, nullptr) != 1) { qDebug() << "error"; }

    // Save private key
    FILE *fp = fopen("private.key", "w");
    PEM_write_RSAPrivateKey(fp, rsa, nullptr, nullptr, 0, nullptr, &passW);
    fclose(fp);

    // Clear memory
    BN_free(bn);
    RSA_free(rsa);
}
