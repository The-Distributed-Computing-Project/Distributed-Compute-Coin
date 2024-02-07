#ifndef settings_h
#define settings_h

#define DELUGE_CHUNK_SIZE 32768

const std::string VERSION = "v0.7.2-alpha";
const std::string BLOCK_VERSION = "v0.8.2-alpha-coin";
const std::string DELUGE_VERSION = "v0.1.2-alpha-deluge";

const std::string serverURL = "http://dccpool.us.to";


namespace WalletSettingValues
{
    constexpr uint8_t verbose{ 2 };

}

#endif
