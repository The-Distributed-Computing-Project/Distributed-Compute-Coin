#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include "strops.h"
#include "P2PClient.h"
#include "Console.h"
#include <boost/process.hpp>
#include <chrono>

#pragma comment(lib,"ws2_32.lib")

const int BUFFERLENGTH = 1024;

char buffer[BUFFERLENGTH];

SOCKET localSocket;
SOCKADDR_IN otherAddr;
int otherSize;
//Console console;

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

void TaskRec(unsigned int update_interval_millisecs) {
	Console console;
	while (true)
	{
		//const auto wait_duration = std::chrono::milliseconds(update_interval_millisecs);

		SOCKADDR_IN remoteAddr;
		int	remoteAddrLen = sizeof(remoteAddr);

		console.NetworkPrint();
		console.WriteLine("Checking for requests...");
		int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);
		//console.WriteLine("Checked, parsing " + iResult);

		if (iResult > 0) {
			console.NetworkPrint();
			console.WriteLine(NormalizedIPString(remoteAddr) + " -> " + std::string(buffer, buffer + iResult));
			std::cout << NormalizedIPString(remoteAddr) << " -> " << std::string(buffer, buffer + iResult) << std::endl;
		}
		else {
			console.NetworkErrorPrint();
			console.WriteLine("Error: Peer closed.");
			//std::cout << "Error: Peer closed." << std::endl;
			//break;
		}

		//std::this_thread::sleep_for(wait_duration);
	}
}

int SafeSend(SOCKET s, char* buf, int buflen)
{
	int sendlen = 0;
	int totalsend = 0;
	int remaining = buflen;

	while (sendlen != buflen)
	{
		sendlen = send(s, &buf[totalsend], remaining, 0);

		if (sendlen == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}

		totalsend = totalsend + sendlen;
		remaining = sendlen - totalsend;
	}
	return 0;
}

int StartP2P(std::string addr, std::string port)
{	
	Console console;

	console.NetworkPrint();
	console.WriteLine("Starting P2P Client");

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
	clientAddr.sin_addr.s_addr = INADDR_ANY;

	//		address.sin_family = AF_INET;
	//		address.sin_addr.s_addr = INADDR_ANY;
	//		address.sin_port = htons(port);

	if (bind(localSocket, (LPSOCKADDR)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
	{
		console.ErrorPrint();
		console.WriteLine("Failed to bind socket");
		closesocket(localSocket);
		WSACleanup();
		return 0;
	}

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
	std::cout << "Peer IP:PORT combo > "; 
	std::cin >> otherIpPort;

	std::cout << "get ip is: " << NormalizedIPString(clientAddr) << std::endl;

	//SOCKADDR_IN testOtherIpPort;
	//testOtherIpPort.sin_port = htons(stoi(SplitString(otherIpPort, ":")[1]));
	//testOtherIpPort.sin_family = AF_INET;
	//testOtherIpPort.sin_addr.s_addr = inet_addr(SplitString(otherIpPort, ":")[0].c_str());
	//int testOtherIpPortSize = sizeof(testOtherIpPort);

	/*if(SplitString(otherIpPort, ":").size()>=3)
	{
		console.NetworkPrint();
		console.WriteLine("Attempting to contact " + otherIpPort);
		sendto(localSocket, otherIpPort.c_str(), otherIpPort.length(), 0, (sockaddr*)&testOtherIpPort, testOtherIpPortSize);
		Sleep(500);
		console.NetworkPrint();
		console.WriteLine("Sent, waiting for response...");
	}*/

	//while (notFound) {
	//	SOCKADDR_IN remoteAddr;
	//	int	remoteAddrLen = sizeof(remoteAddr);

	//	int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);

	//	if (iResult > 0) {
	//		endpoint = std::string(buffer, buffer + iResult);

	//		//std::cout << "Peer-to-peer Endpoint: " << endpoint << std::endl;
	//		console.NetworkPrint();
	//		console.WriteLine("Found Peer-to-peer Endpoint: " + endpoint);

	//		notFound = false;
	//	}
	//	else {

	//		console.NetworkErrorPrint();
	//		std::cout << WSAGetLastError();
	//		console.WriteLine();
	//	}
	//}
	endpoint = otherIpPort;

	std::string host = SplitString(otherIpPort, ":")[0];
	int otherPort = stoi(SplitString(otherIpPort, ":")[1]);
	
	otherAddr.sin_port = htons(otherPort);
	otherAddr.sin_family = AF_INET;
	otherAddr.sin_addr.s_addr = inet_addr(host.c_str());

	otherSize = sizeof(otherAddr);

	const unsigned int update_interval = 50; // update after every 50 milliseconds
	std::thread t1(TaskRec, update_interval);

	while (true) {
		std::string msg = "Hello world!";
		char* msgConvert = (char*)msg.c_str();
		//int err = SafeSend(localSocket, msgConvert, msg.length());
		//std::cout << err << std::endl;
		
		sendto(localSocket, msg.c_str(), msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
		Sleep(1000);

		console.NetworkPrint();
		console.WriteLine("Sending: " + msg);
	}

	getchar();

	closesocket(localSocket);
	WSACleanup();
}
