#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include "strops.h"
#include "P2PClient.h"

#pragma comment(lib,"ws2_32.lib")

const int BUFFERLENGTH = 1024;

char buffer[BUFFERLENGTH];

SOCKET localSocket;
SOCKADDR_IN otherAddr;
int otherSize;

std::string NormalizedIPString(SOCKADDR_IN addr) {
	char host[16];
	ZeroMemory(host, 16);
	inet_ntop(AF_INET, &addr.sin_addr, host, 16);

	USHORT port = ntohs(addr.sin_port);

	int realLen = 0;
	for (int i = 0; i < 16; i++) {
		if (host[i] == '\00') {
			break;
		}
		realLen++;
	}

	std::string res(host, realLen);
	res += ":" + std::to_string(port);

	return res;
}

void TaskRec() {
	while (true) {
		SOCKADDR_IN remoteAddr;
		int	remoteAddrLen = sizeof(remoteAddr);

		int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);

		if (iResult > 0) {
			std::cout << NormalizedIPString(remoteAddr) << " -> " << std::string(buffer, buffer + iResult) << std::endl;
		}
		else {
			std::cout << "Error: Peer closed." << std::endl;
			break;
		}
	}
}

int StartP2P(std::string addr, std::string port)
{	
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return 0;
	}

	//SOCKADDR_IN serverAddr;
	//serverAddr.sin_port = htons(6668);
	//serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	

	//int serverSize = sizeof(serverAddr);

	localSocket = socket(AF_INET, SOCK_DGRAM, 0);

	SOCKADDR_IN clientAddr;
	clientAddr.sin_port = htons(stoi(port));
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = inet_addr(addr.c_str());

	//if (bind(localSocket, (LPSOCKADDR)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
	//	return 0;
	//}

	int val = 64 * 1024;
	setsockopt(localSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));
	setsockopt(localSocket, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val));	
	//
	//std::string request = "1";
	//std::cout << "Identification number: ";  std::cin >> request;
	//
	//sendto(localSocket, request.c_str(), request.length(), 0, (sockaddr*)&serverAddr, serverSize);

	bool notFound = true;

	std::string endpoint;

	std::string otherIpPort;
	std::cout << "Peer IP:PORT > ";  std::cin >> otherIpPort;

	SOCKADDR_IN testOtherIpPort;
	testOtherIpPort.sin_port = htons(stoi(SplitString(otherIpPort, ":")[1]));
	testOtherIpPort.sin_family = AF_INET;
	testOtherIpPort.sin_addr.s_addr = inet_addr(SplitString(otherIpPort, ":")[0].c_str());
	int testOtherIpPortSize = sizeof(testOtherIpPort);

	sendto(localSocket, otherIpPort.c_str(), otherIpPort.length(), 0, (sockaddr*)&testOtherIpPort, testOtherIpPortSize);
	Sleep(500);

	while (notFound) {
		SOCKADDR_IN remoteAddr;
		int	remoteAddrLen = sizeof(remoteAddr);

		int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);

		if (iResult > 0) {
			endpoint = std::string(buffer, buffer + iResult);

			std::cout << "Peer-to-peer Endpoint: " << endpoint << std::endl;

			notFound = false;
		}
		else {

			std::cout << WSAGetLastError();
		}
	}

	std::string host = endpoint.substr(0, endpoint.find(':'));
	int otherPort = stoi(endpoint.substr(endpoint.find(':') + 1));
	
	otherAddr.sin_port = htons(otherPort);
	otherAddr.sin_family = AF_INET;
	otherAddr.sin_addr.s_addr = inet_addr(host.c_str());

	otherSize = sizeof(otherAddr);

	std::thread t1(TaskRec);

	while (true) {
		std::string msg = "Hello world!";
		sendto(localSocket, msg.c_str(), msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
		Sleep(500);
	}

	getchar();

	closesocket(localSocket);
	WSACleanup();
}