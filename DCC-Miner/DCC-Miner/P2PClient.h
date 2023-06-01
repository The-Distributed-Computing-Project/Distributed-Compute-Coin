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
	std::atomic_int messageStatus = 0;
	int messageAttempt = 0;
	std::vector<std::string> CONNECTION_PARTS = { "" };
public:
	//using namespace std;
	//
	//std::string NormalizedIPString(SOCKADDR_IN addr);
	//
	//void TaskRec();

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void ListenerThread(int update_interval);
	int SafeSend(SOCKET s, char* buf, int buflen);
	int OpenP2PSocket(std::string port);
	void SenderThread();
	int StartP2P(std::string addr, std::string port, std::string peerPort);
};

#endif
