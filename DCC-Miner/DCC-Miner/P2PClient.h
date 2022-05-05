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
  bool CONNECTED_TO_PEER = false;

  std::string NormalizedIPString(SOCKADDR_IN addr);
  void TaskRec(unsigned int update_interval_millisecs);
  int SafeSend(SOCKET s, char* buf, int buflen);
  int StartP2P(std::string addr, std::string port);
};

#endif
