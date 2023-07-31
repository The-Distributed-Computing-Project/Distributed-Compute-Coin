#ifndef p2pclient_h
#define p2pclient_h

#include <string>
#include <vector>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <math.h>

#include "strops.h"
#include "Console.h"
#include <boost/process.hpp>
#include <chrono>
#include "Network.h"
#include "FileManip.h"
#include "SettingsConsts.h"


extern std::vector<std::string> peerList;

//extern P2P p2p;


class P2P
{
private:
	SOCKET localSocket;
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

	uint8_t role = -1; //   -1 == offline,  0 == requester,  1 == answerer

	std::atomic_int messageStatus = -1;
	enum MsgStatus {
		idle = -1,
		initial_connect_request = 0,
		disconnect_request = 9,
		await_first_success = 1,
		await_second_success = 2,
		replying_height = 3,
		replying_block = 4,
		replying_pendingblock = 11,
		requesting_height = 5,
		requesting_block = 6,
		requesting_pendingblock = 10,
		requesting_peer_list = 7,
		replying_peer_list = 8,
	};

	int reqDat = -1;

	std::string peerIP;
	int peerPort;

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void ListenerThread(int update_interval);
	void RandomizePeer();
	//int mySendTo(int socket, std::string& s, int len, int redundantFlags, sockaddr* to, int toLen);
	int OpenP2PSocket(int port);
	void SenderThread();
	int mySendTo(int socket, std::string& s, int len, int redundantFlags, sockaddr* to, int toLen);
};

#endif
