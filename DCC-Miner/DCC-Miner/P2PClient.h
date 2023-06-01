#ifndef p2pclient_h
#define p2pclient_h

#include <string>
#include <vector>


extern std::vector<std::string> peerList;


class P2P
{
private:
	SOCKET localSocket;
public:
	//using namespace std;
	//
	//std::string NormalizedIPString(SOCKADDR_IN addr);
	//
	//void TaskRec();
	int MSG_PART = 0;
	std::atomic_bool bool CONNECTED_TO_PEER = false;
	std::atomic<int> messageStatus = 0;
	int messageAttempt = 0;
	std::vector<std::string> CONNECTION_PARTS = {""};

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void ListenerThread(int update_interval);
	int SafeSend(SOCKET s, char* buf, int buflen);
	int P2P::OpenP2PSocket(std::string port);
	void P2P::SenderThread();
	int StartP2P(std::string addr, std::string port, std::string peerPort);
};

#endif
