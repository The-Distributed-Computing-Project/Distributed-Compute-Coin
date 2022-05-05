#ifndef p2pclient_h
#define p2pclient_h

#include <string>

class P2P
{
public:
  //using namespace std;
  //
  //std::string NormalizedIPString(SOCKADDR_IN addr);
  //
  //void TaskRec();
  bool CONNECTED_TO_PEER = false;

  int StartP2P(std::string addr, std::string port);
};

#endif
