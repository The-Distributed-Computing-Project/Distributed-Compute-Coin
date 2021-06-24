<<<<<<< HEAD
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <functional> 
#include <cctype>
#include <locale>
//#include <graphics.h>
#include "CImg.h"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8001"

using namespace std;
using namespace cimg_library;

static string wallet = "";
static int blockchainlength = 0;
static string confirmTransact = "";
static string transactionDetails = "";
PCSTR ipAddress = "192.168.56.1";

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo* result = NULL,
    * ptr = NULL,
    hints;
const char* sendbuf;
char recvbuf[DEFAULT_BUFLEN];
int iResult;
int recvbuflen = DEFAULT_BUFLEN;

void WaitForConfirmation();
void Trade();
static void Help();
static float GetBalance(string walletAddress);
static void Sync(int whichBlock);
static int GetBlockChainLength();
vector<string> split(string s, string delimiter);
static inline void ltrim(std::string& s);
static inline void rtrim(std::string& s);
static inline void trim(std::string& s);
static inline std::string trim_copy(std::string s);
static void Connect();

int main()
{
    /*unsigned int width = 500;
    unsigned int height = 500;
    CImg<unsigned char> bg(width, height, 1, 3, 255);

    const unsigned char blue[] = { 0,20,255 };

    bg.draw_circle(width / 2, height / 2, 100, blue);

    CImgDisplay dsp(width, height, "Compute Coin Client", 0);

    dsp.display(bg);

    while (!dsp.is_closed())
    {

        dsp.wait();
    }*/

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    Connect();

    closesocket(ConnectSocket);
    cout << "Enter your wallet address : " << endl;
    cin >> wallet;
    cout << "Syncing with server..." << endl;
    float balance = GetBalance(wallet);
    std::ostringstream ss;
    ss << balance;
    cout << "You have " + ss.str() + " Compute Credits" << endl;

    fstream readConfig;
    readConfig.open("D:\\Code\\Blockchain Main\\cppClient\\config.txt");
    readConfig >> confirmTransact;
    readConfig >> transactionDetails;
    readConfig.close();

    if (confirmTransact == "true")
        WaitForConfirmation();

    while (true)
    {
        cout << "What do you want to do :  " << endl;
        string command;
        cin >> command;
        if (command == "trade")
            Trade();
        if (command == "help")
            Help();
        if (command == "sync")
        {
            for (int i = 1; i < blockchainlength; i++)
            {
                Sync(i);
            }
        }
    }
    WSACleanup();
    return 0;
}

static void Connect()
{
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ipAddress, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return;
    }
}

void WaitForConfirmation()
{
    Connect();

    cout << "Waiting for transaction confirmation..." << endl;
    try
    {
        string str = "$ISTRANSACTIONREADY##" + transactionDetails;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
       
        closesocket(ConnectSocket);
        fstream writeBlock;
        writeBlock.open("D:\\Code\\Blockchain Main\\cppClient\\config.txt", std::ios_base::app);
        writeBlock << (" \n ");
        writeBlock.close();
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return;
    }
}

void Trade()
{
    Connect();

    cout << ("Wallet address of who you want to send to : ") << endl;
    string recipient;
    cin >> recipient;

    cout << ("Amount of Compute Credits : ") << endl;
    string sendAmount;
    cin >> sendAmount;

    try
    {
        cout << ("\nStarting trading dialogue...") << endl;

        string str = sendAmount + "->" + wallet + "->" + recipient;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        cout << (iResult) << endl;

        closesocket(ConnectSocket);

        fstream writeBlock;
        writeBlock.open("D:\\Code\\Blockchain Main\\cppClient\\config.txt", std::ios_base::app);
        writeBlock << ("true\n" + sendAmount + "->" + wallet + "->" + recipient);
        writeBlock.close();
    }
    catch (const std::exception& )
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return;
    }

    //cout << ("Your wallet has:  " + walletValue + " Compute Tokens");
}

static void Help()
{
    cout << ("Possible Actions:\n") << endl;
    cout << ("trade: send tokens to anybody") << endl;
}

static float GetBalance(string walletAddress)
{
    Connect();

    try
    {
        cout << ("\nGetting current balance...") << endl;

        string str = "$GETWALLLETBALANCE##" + walletAddress;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return 0.0f;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        closesocket(ConnectSocket);

        std::ostringstream ss;
        ss << recvbuf;
        string s;
        s = ss.str();
        blockchainlength = stoi(split(s, "##")[1]);
        return stof(split(trim_copy(s), "##")[0]);
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return 0;
    }
}

static void Sync(int whichBlock)
{
    Connect();

    try
    {
        std::ostringstream ww;
        ww << whichBlock;
        string w;
        w = ww.str();
        string str = "$GETBLOCKCHAIN##" + w;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        std::ostringstream ss;
        ss << recvbuf;
        string s;
        s = ss.str();
        std::replace(s.begin(), s.end(), (char)"##DIVIDE$LINE##", '\n');
        cout << (s) << endl;
        
        closesocket(ConnectSocket);

        fstream writeBlock;
        writeBlock.open("D:\\Code\\Blockchain Main\\cppClient\\blockchain\\block" + to_string(whichBlock) + ".txt", std::ios_base::app);
        writeBlock << (s);
        writeBlock.close();
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return;
    }

    int waitTime = 0;
    while (waitTime < 10000000)
    {
        waitTime++;
    }
}

static int GetBlockChainLength()
{
    Connect();

    try
    {
        cout << ("\nGetting blockchain length...") << endl;

        string str = "$GETBLOCKCHAINLENGTH##";
        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return 0;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        closesocket(ConnectSocket);

        std::ostringstream ss;
        ss << recvbuf;
        string s;
        s = ss.str();
        return stoi(trim_copy(s));
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return 0;
    }
}

// for string delimiter
vector<string> split(string s, string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;

    //  EXAMPLE
    //int main()
    //{
    //    string str = "adsf-+qwret-+nvfkbdsj-+orthdfjgh-+dfjrleih";
    //    string delimiter = "-+";
    //    vector<string> v = split(str, delimiter);
    //
    //    for (auto i : v) cout << i << endl;
    //
    //    return 0;
    //}
}

// trim from start (in place)
static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}
// trim from both ends (in place)
static inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
=======
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <functional> 
#include <cctype>
#include <locale>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8001"

using namespace std;

static string wallet = "";
static int blockchainlength = 0;
static string confirmTransact = "";
static string transactionDetails = "";
PCSTR ipAddress = "192.168.56.1";

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo* result = NULL,
    * ptr = NULL,
    hints;
const char* sendbuf;
char recvbuf[DEFAULT_BUFLEN];
int iResult;
int recvbuflen = DEFAULT_BUFLEN;

void WaitForConfirmation();
void Trade();
static void Help();
static float GetBalance(string walletAddress);
static void Sync(int whichBlock);
static int GetBlockChainLength();
vector<string> split(string s, string delimiter);
static inline void ltrim(std::string& s);
static inline void rtrim(std::string& s);
static inline void trim(std::string& s);
static inline std::string trim_copy(std::string s);
static void Connect();

int main()
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    Connect();

    closesocket(ConnectSocket);
    cout << "Enter your wallet address : " << endl;
    cin >> wallet;
    cout << "Syncing with server..." << endl;
    float balance = GetBalance(wallet);
    std::ostringstream ss;
    ss << balance;
    cout << "You have " + ss.str() + " Compute Credits" << endl;

    fstream readConfig;
    readConfig.open("D:\\Code\\Blockchain Main\\Blockchain Client\\config.txt");
    readConfig >> confirmTransact;
    readConfig >> transactionDetails;
    readConfig.close();

    if (confirmTransact == "true")
        WaitForConfirmation();

    while (true)
    {
        cout << "What do you want to do :  " << endl;
        string command;
        cin >> command;
        if (command == "trade")
            Trade();
        if (command == "help")
            Help();
        if (command == "sync")
        {
            for (int i = 1; i < blockchainlength; i++)
            {
                Sync(i);
            }
        }
    }
    WSACleanup();
    return 0;
}

static void Connect()
{
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ipAddress, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return;
    }
}

void WaitForConfirmation()
{
    Connect();

    cout << "Waiting for transaction confirmation..." << endl;
    try
    {
        string str = "$ISTRANSACTIONREADY##" + transactionDetails;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
       
        closesocket(ConnectSocket);
        fstream writeBlock;
        writeBlock.open("D:\\Code\\Blockchain Main\\Blockchain Client\\config.txt", std::ios_base::app);
        writeBlock << (" \n ");
        writeBlock.close();
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return;
    }
}

void Trade()
{
    Connect();

    cout << ("Wallet address of who you want to send to : ") << endl;
    string recipient;
    cin >> recipient;

    cout << ("Amount of Compute Credits : ") << endl;
    string sendAmount;
    cin >> sendAmount;

    try
    {
        cout << ("\nStarting trading dialogue...") << endl;

        string str = sendAmount + "->" + wallet + "->" + recipient;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        cout << (iResult) << endl;

        closesocket(ConnectSocket);

        fstream writeBlock;
        writeBlock.open("D:\\Code\\Blockchain Main\\Blockchain Client\\config.txt", std::ios_base::app);
        writeBlock << ("true\n" + sendAmount + "->" + wallet + "->" + recipient);
        writeBlock.close();
    }
    catch (const std::exception& )
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return;
    }

    //cout << ("Your wallet has:  " + walletValue + " Compute Tokens");
}

static void Help()
{
    cout << ("Possible Actions:\n") << endl;
    cout << ("trade: send tokens to anybody") << endl;
}

static float GetBalance(string walletAddress)
{
    Connect();

    try
    {
        cout << ("\nGetting current balance...") << endl;

        string str = "$GETWALLLETBALANCE##" + walletAddress;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return 0.0f;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        closesocket(ConnectSocket);

        std::ostringstream ss;
        ss << recvbuf;
        string s;
        s = ss.str();
        blockchainlength = stoi(split(s, "##")[1]);
        return stof(split(trim_copy(s), "##")[0]);
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return 0;
    }
}

static void Sync(int whichBlock)
{
    Connect();

    try
    {
        std::ostringstream ww;
        ww << whichBlock;
        string w;
        w = ww.str();
        string str = "$GETBLOCKCHAIN##" + w;

        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        std::ostringstream ss;
        ss << recvbuf;
        string s;
        s = ss.str();
        std::replace(s.begin(), s.end(), (char)"##DIVIDE$LINE##", '\n');
        cout << (s) << endl;

        closesocket(ConnectSocket);

        fstream writeBlock;
        writeBlock.open("D:\\Code\\Blockchain Main\\Blockchain Client\\blockchain\\block" + to_string(whichBlock) + ".txt", std::ios_base::app);
        writeBlock << (s);
        writeBlock.close();
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return;
    }

    int waitTime = 0;
    while (waitTime < 10000000)
    {
        waitTime++;
    }
}

static int GetBlockChainLength()
{
    Connect();

    try
    {
        cout << ("\nGetting blockchain length...") << endl;

        string str = "$GETBLOCKCHAINLENGTH##";
        iResult = send(ConnectSocket, str.c_str(), (int)strlen(str.c_str()), 0);
        if (iResult == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            return 0;
        }
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {}
            //printf("Bytes iResult: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

        closesocket(ConnectSocket);

        std::ostringstream ss;
        ss << recvbuf;
        string s;
        s = ss.str();
        return stoi(trim_copy(s));
    }
    catch (const std::exception&)
    {
        cout << ("Error, Try again later") << endl;

        closesocket(ConnectSocket);
        return 0;
    }
}

// for string delimiter
vector<string> split(string s, string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;

    //  EXAMPLE
    //int main()
    //{
    //    string str = "adsf-+qwret-+nvfkbdsj-+orthdfjgh-+dfjrleih";
    //    string delimiter = "-+";
    //    vector<string> v = split(str, delimiter);
    //
    //    for (auto i : v) cout << i << endl;
    //
    //    return 0;
    //}
}

// trim from start (in place)
static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}
// trim from both ends (in place)
static inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
>>>>>>> parent of ddba084 (Fixed blockchain edited bugs, and added GUI interface for C# build)
}