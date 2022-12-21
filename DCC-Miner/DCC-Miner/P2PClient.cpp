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
#include "Network.h"

#pragma comment(lib,"ws2_32.lib")

const int BUFFERLENGTH = 1024;

char buffer[BUFFERLENGTH];

//SOCKET localSocket;
SOCKADDR_IN otherAddr;
int otherSize;
std::atomic_bool stop_thread_1 = false;
//Console console;

std::string P2P::NormalizedIPString(SOCKADDR_IN addr) {
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

//void P2P::TaskRec()
void P2P::TaskRec(unsigned int update_interval_millisecs)
{
	Console console;
	const auto wait_duration = std::chrono::milliseconds(update_interval_millisecs);

	SOCKADDR_IN remoteAddr;
	int remoteAddrLen = sizeof(remoteAddr);

	while (true)
	{
		if (stop_thread_1)
			return;

		console.NetworkPrint();
		console.WriteLine("Checking for requests...");
		int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);
		//console.WriteLine("Checked, parsing " + iResult);

		if (iResult > 0) {
			if (std::string(buffer, buffer + iResult) == "peer connect") {
				console.WriteLine("Received initial connection, awaiting confirmation...", console.greenFGColor, "");
				CONNECTED_TO_PEER = true;
				messageStatus = 1;
			}
			else if (std::string(buffer, buffer + iResult) == "peer success") {
				console.WriteLine("Dual Connection Confirmation", console.greenFGColor, "");
				messageStatus = 2;
			}
			std::cout << "received: " << NormalizedIPString(remoteAddr) << " -> " << std::string(buffer, buffer + iResult) << std::endl;
		}
		else {
			console.NetworkErrorPrint();
			console.WriteLine("Error: Peer closed.");
			break;
		}

		//if (messageStatus==2)
		//	return;

		std::this_thread::sleep_for(wait_duration);
	}
}

int P2P::SafeSend(SOCKET s, char* buf, int buflen)
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

int P2P::StartP2P(std::string addr, std::string port)
{
	Console console;
	Http http;

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

	std::string otherIpPort;
	//std::cout << "Peer IP:PORT combo > "; 
	//std::cin >> otherIpPort;

	console.NetworkPrint();
	console.WriteLine("Asking server for PEER address...");
	std::vector<std::string> args = {
		"query=AskForConnection",
		"ip_port=" + addr + ":" + port,
		"last_tried_ip_port=none"
	};
	std::string httpOut = TrimString(http.StartHttpWebRequest("http://api.achillium.us.to/dcc/p2pconn.php?", args));
	console.NetworkPrint();
	console.WriteLine("HTTP returned: " + httpOut);

	std::string last_tried_ip_port = "";
	for (int t = 0; t < 20; t++)
		//while (true)
	{
		console.NetworkPrint();
		console.WriteLine("Attempt: " + std::to_string(t) + "  |  Asking server for PEER address...");

		std::vector<std::string> args = {
			"query=WaitForConnection",
			"ip_port=" + addr + ":" + port,
			"last_tried_ip_port=" + last_tried_ip_port
		};
		httpOut = TrimString(http.StartHttpWebRequest("http://api.achillium.us.to/dcc/p2pconn.php?", args));

		if (httpOut == "" || httpOut.find("waiting") != std::string::npos || httpOut.find(addr + ":" + port) != std::string::npos)
		{
			console.NetworkPrint();
			console.WriteLine("Failed, waiting 3 sec to look for a new peer...");
			Sleep(3000);
			continue;
		}

		console.NetworkPrint();
		console.WriteLine("HTTP2 returned: " + httpOut);

		otherIpPort = httpOut;


		std::string host = SplitString(otherIpPort, ":")[0];
		int otherPort = stoi(SplitString(otherIpPort, ":")[1]);

		otherAddr.sin_port = htons(otherPort);
		otherAddr.sin_family = AF_INET;
		otherAddr.sin_addr.s_addr = inet_addr(host.c_str());

		otherSize = sizeof(otherAddr);

		// Listen to replies
		const unsigned int update_interval = 50; // update after every 50 milliseconds
		std::thread t1(&P2P::TaskRec, this, update_interval);

		// Begin sending messages, and stop when a reply is received
		for (messageAttempt = 0; messageAttempt < 15; messageAttempt++)
			//while (true)
		{
			std::string msg = messageStatus >= 1 ? "peer success" : "peer connect";
			char* msgConvert = (char*)msg.c_str();
			//int err = SafeSend(localSocket, msgConvert, msg.length());
			//std::cout << err << std::endl;

			console.NetworkPrint();
			console.WriteLine("Attempt: " + std::to_string(messageAttempt) + "  |  Sending: " + msg);

			sendto(localSocket, msg.c_str(), msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
			Sleep(3000);

			if (messageStatus == 2) {
				console.WriteLine("Successful connection!", console.greenFGColor, "");

				if (messageAttempt == 4) {
					stop_thread_1 = true;
					t1.join();
					stop_thread_1 = false;

					closesocket(localSocket);
					WSACleanup();

					console.WriteLine("Closing connection...", console.greenFGColor, "");

					return 0;
				}
			}
		}

		console.WriteLine("Unsuccessful connection, trying other peer", console.redFGColor, "");

		stop_thread_1 = true;
		t1.join();
		stop_thread_1 = false;

		continue;
	}

	//std::cout << "get ip is: " << NormalizedIPString(clientAddr) << std::endl;

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


	//getchar();

	closesocket(localSocket);
	WSACleanup();
}
