#ifndef settings_h
#define settings_h

#define DELUGE_CHUNK_SIZE 32768
#define DELUGE_MAX_SIZE_B 134217728 /* 2^27, also known as 128 megabytes */
#define DELUGE_MAX_CHUNKS DELUGE_MAX_SIZE_B / DELUGE_CHUNK_SIZE  /* The number of chunks allowed */

const std::string VERSION = "v0.7.2-alpha";
const std::string BLOCK_VERSION = "v0.8.2-alpha-coin";
const std::string DELUGE_VERSION = "v0.1.2-alpha-deluge";

const std::string serverURL = "http://dccpool.us.to";


namespace WalletSettingValues
{
    constexpr uint8_t verbose{ 2 };

}

#endif
