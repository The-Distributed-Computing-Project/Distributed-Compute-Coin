#ifndef p2pclient_h
#define p2pclient_h

#include <string>

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
	bool CONNECTED_TO_PEER = false;
	int messageStatus = 0;
	int messageAttempt = 0;
	std::vector<std::string> CONNECTION_PARTS = {""};

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void TaskRec(unsigned int update_interval_millisecs);
	int SafeSend(SOCKET s, char* buf, int buflen);
	int StartP2P(std::string addr, std::string port);
};

#endif
