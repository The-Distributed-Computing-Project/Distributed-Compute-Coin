#pragma once

#include <string>
#include <vector>

#if WINDOWS
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#endif

#include <iostream>
#include <thread>
#include <math.h>

#include "json.hpp"
#include "strops.h"
#include "Console.h"
#include <boost/process.hpp>
#include <chrono>
#include "Network.h"
#include "FileManip.h"
#include "SettingsConsts.h"
#include "crypto.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS


extern std::vector<std::string> peerList;

using json = nlohmann::json;

//extern P2P p2p;


class P2P
{
private:
#if defined(_MSC_VER)
	SOCKET localSocket;
#endif
	int MSG_PART = 0;
	//int messageStatus = 0;
	std::vector<std::string> CONNECTION_PARTS = { "" };
public:
	//using namespace std;
	//
	//std::string NormalizedIPString(SOCKADDR_IN addr);
	//
	//void TaskRec();
	std::atomic_bool CONNECTED_TO_PEER = false;

	int messageAttempt = 0;
	int differentPeerAttempts = 0;

	std::vector<std::string> peerList;
	int peerListID = 0;

	int role = -1; //   -1 == offline,  0 == requester,  1 == answerer

	std::atomic_int messageStatus = -1;
	enum MsgStatus {
		idle = -1,
		initial_connect_request = 0,
		disconnect_request = 9,

		await_first_success = 1,
		await_second_success = 2,

		requesting_height = 5,
		replying_height = 3,

		requesting_block = 6,
		replying_block = 4,

		requesting_peer_list = 7,
		replying_peer_list = 8,

		requesting_pendingblock = 10,
		replying_pendingblock = 11,

		requesting_transaction_process = 12,
		replying_transaction_process = 13,

		requesting_deluge_file = 14,
		replying_deluge_file = 15,

		requesting_deluge_chunk = 16,
		replying_deluge_chunk = 17,
	};

	int reqDat = -1;
	std::string extraData = "";

	std::string peerIP;
	int peerPort;

#if defined(_MSC_VER)
	std::string NormalizedIPString(SOCKADDR_IN addr);
#endif
	bool isAwaiting();
	void ListenerThread(int update_interval);
	void RandomizePeer();
	//int mySendTo(int socket, std::string& s, int len, int redundantFlags, sockaddr* to, int toLen);
	int OpenP2PSocket(int port);
	void SenderThread();
	int mySendTo(int socket, std::string& s, int len, int redundantFlags, sockaddr* to, int toLen);
	void InitPeerList();
	void SavePeerList();
};

bool VerifyTransaction(json& tx, uint32_t id = 0, bool thorough = false);

