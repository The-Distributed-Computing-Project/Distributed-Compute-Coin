#ifndef settings_h
#define settings_h

#define DELUGE_CHUNK_SIZE 32768
#define DELUGE_MAX_SIZE_B 134217728 /* 2^27, also known as 128 megabytes */
// The number of chunks allowed, should be 4096 if using 128 MB max
#define DELUGE_MAX_CHUNKS DELUGE_MAX_SIZE_B / DELUGE_CHUNK_SIZE

#if UNIX
#define PLATFORMSTR " LINUX"
#define PLATFORMCOLOR console::yellowFGColor
#else
#define PLATFORMSTR " WINDOWS"
#define PLATFORMCOLOR console::greenFGColor
#endif

const std::string VERSION = "v0.8.0-alpha";
const std::string BLOCK_VERSION = "v0.8.5-alpha-coin";
const std::string DELUGE_VERSION = "v0.1.2-alpha-deluge";

const std::string serverURL = "http://dccpool.us.to";


namespace WalletSettingValues
{
    constexpr uint8_t verbose{ 3 };

}

#endif
