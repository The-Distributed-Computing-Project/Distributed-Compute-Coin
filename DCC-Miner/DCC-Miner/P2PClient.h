#ifndef p2pclient_h
#define p2pclient_h

#include <string>
#include <vector>


extern std::vector<std::string> peerList;


class P2P
{
private:
	SOCKET localSocket;
	int MSG_PART = 0;
	std::atomic_bool CONNECTED_TO_PEER = false;
	//int messageStatus = 0;
	std::vector<std::string> CONNECTION_PARTS = { "" };
public:
	//using namespace std;
	//
	//std::string NormalizedIPString(SOCKADDR_IN addr);
	//
	//void TaskRec();

	int messageAttempt = 0;
	std::atomic_int messageStatus = idle;
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

	int reqDat = 0;

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void ListenerThread(int update_interval);
	int SafeSend(SOCKET s, char* buf, int buflen);
	int OpenP2PSocket(int port);
	void SenderThread();
	int StartP2P(std::string addr, std::string port, std::string peerPort);
};

#endif
