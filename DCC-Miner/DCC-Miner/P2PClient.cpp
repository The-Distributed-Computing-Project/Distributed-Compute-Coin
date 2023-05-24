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
#include "FileManip.h"

#pragma comment(lib,"ws2_32.lib")

const int BUFFERLENGTH = 1024 * 8;

char buffer[BUFFERLENGTH];

int reqDat = 0;
int blockchainLength = 0;
int peerBlockchainLength = 0;

//SOCKET localSocket;
SOCKADDR_IN otherAddr;
int otherSize;
std::atomic_bool stop_thread_1 = false;
std::atomic_bool thread_running = false;
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

// Send full message safely
int mySendTo(int socket, std::string& s, int length, int flags, sockaddr* to, int toLen) {
	const char* p = s.c_str();
	size_t len = s.length();
	size_t n;
	while (len > 0 && (n = sendto(socket, p, length, flags, to, toLen)) > 0) {
		// successfully sent some (possibly all) of the message
		// if it was partially successful, advance down the string
		// to the bit which didn't get sent, and try again
		std::cout << "sent " << std::to_string(n) << " of " << std::to_string(length) << std::endl;
		len -= n;
		p += n;
		n = 1;
	}
	if (n == 0) {
		// a send call failed to make any progress through the data
		// or perhaps len itself was 0 to start with.
		std::cout << "Send failed" << std::endl;
	}
	if (n < 0) {
		// look at errno to determine what went wrong
		// some values like EAGAIN and EINTR may be worth a 2nd attempt
		std::cout << "Send failed, errno: " << std::to_string(n) << std::endl;
	}
	return n;
}

// Receive full message safely
int myReceiveFrom(int socket, char* buffer, int len, int flags, sockaddr* from, int* fromLen) {
	int n = recvfrom(socket, buffer, len, 0, from, fromLen);
	//while (len > 0 && (n = send(socket, p, len, 0)) > 0) {
	//	// successfully sent some (possibly all) of the message
	//	// if it was partially successful, advance down the string
	//	// to the bit which didn't get sent, and try again
	//	len -= n;
	//	p += n;
	//	n = 1;
	//}
	//if (n == 0) {
	//	// a send call failed to make any progress through the data
	//	// or perhaps len itself was 0 to start with.
	//}
	//if (n < 0) {
	//	// look at errno to determine what went wrong
	//	// some values like EAGAIN and EINTR may be worth a 2nd attempt
	//}
	return n;
}

//void P2P::TaskRec()
void P2P::TaskRec(int update_interval)
{
	thread_running = true;
	while (true) {
		Console console;
		//const auto wait_duration = std::chrono::milliseconds(update_interval);

		SOCKADDR_IN remoteAddr;
		int remoteAddrLen = sizeof(remoteAddr);


		struct timeval stTimeOut;
		fd_set stReadFDS;
		FD_ZERO(&stReadFDS);
		stTimeOut.tv_sec = 3; // 20 second timeout
		stTimeOut.tv_usec = 0;
		FD_SET(localSocket, &stReadFDS);

		if (stop_thread_1) {
			thread_running = false;
			return;
		}

		while (!stop_thread_1)
		{
			//std::cerr << "\nstopthread status:" << (stop_thread_1 ? "true" : "false") << std::endl;
			if (stop_thread_1) {
				thread_running = false;
				return;
			}
			/*if (messageAttempt == 4)
				return;*/

				//console.NetworkPrint();
				//console.WriteLine("Checking for requests...");
			int t = select(-1, &stReadFDS, 0, 0, &stTimeOut);
			std::cerr << ("monitoring... ") << std::to_string(messageStatus) << std::endl;
			if (false) {
			//if (t == SOCKET_ERROR) {
				console.NetworkErrorPrint();
				console.WriteLine("Error: Socket Error, trying again...");
				break;
			}
			else {
				int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);
				//console.WriteLine("Checked, parsing " + iResult);
				//std::cout << "iResult: " << std::to_string(iResult) << std::endl;
				if (iResult > 0) {
					std::string textVal = std::string(buffer, buffer + iResult);
					// If the peer is requesting to connect
					if (textVal == "peer$$$connect") {
						console.DebugPrint();
						console.WriteLine("Received initial connection, awaiting confirmation...", console.greenFGColor, "");
						messageStatus = 1; // Awaiting confirmation status
						messageAttempt = 0;
					}
					// If the peer is requesting message received confirmation
					else if (textVal == "peer$$$success" && (messageStatus >= 0)) {
						console.DebugPrint();
						console.WriteLine("Dual Confirmation", console.greenFGColor, "");
						messageStatus = 2; // Confirmed message status, continue sending our own 
										   // confirmation for 4 times, then switch to idle state -1
						CONNECTED_TO_PEER = true;
					}
					// If the peer is idling
					else if (textVal == "peer$$$idle") {
						console.DebugPrint();
						console.WriteLine("idle...", console.yellowFGColor, "");
						//messageStatus = -1;
					}
					// If peer is requesting data
					else if (SplitString(textVal, "$$$")[0] == "request") {
						// If peer is asking for blockchain height
						if (SplitString(textVal, "$$$")[1] == "height")
							messageStatus = 3;
						// If peer is asking for a block's data
						else if (SplitString(textVal, "$$$")[1] == "block") {
							messageStatus = 4;
							reqDat = std::stoi(SplitString(textVal, "$$$")[2]);
						}

						console.WriteLine("request " + std::to_string(messageStatus), console.greenFGColor, "");
					}
					// If peer is answering request
					else if (SplitString(textVal, "$$$")[0] == "answer") {
						// If peer is giving blockchain height
						if (SplitString(textVal, "$$$")[1] == "height") {
							peerBlockchainLength = std::stoi(SplitString(textVal, "$$$")[2]);
							messageStatus = 1;
							console.WriteLine("answer height: " + std::to_string(peerBlockchainLength), console.greenFGColor, "");
						}
						// If peer is giving a block's data
						else if (SplitString(textVal, "$$$")[1] == "block") {
							messageStatus = 1;
							int num = std::stoi(SplitString(textVal, "$$$")[2]);
							std::string blockData = SplitString(textVal, "$$$")[3];

							// Save block data to file
							try
							{
								std::ofstream blockFile("./wwwdata/blockchain/block" + std::to_string(num) + ".dccblock");
								if (blockFile.is_open())
								{
									blockFile << blockData;
									blockFile.close();
								}
							}
							catch (const std::exception& e)
							{
								std::cerr << e.what() << std::endl;
							}

							console.WriteLine("answer block: " + std::to_string(num), console.greenFGColor, "");
						}
						messageAttempt = 0;

					}
					console.WriteLine("received: " + NormalizedIPString(remoteAddr) + " -> " + std::string(buffer, buffer + iResult) + "\t status: " + std::to_string(messageStatus));
				}
				else {
					console.NetworkErrorPrint();
					console.WriteLine("Error: Peer closed.");
					CONNECTED_TO_PEER = false;
					thread_running = false;
					return;
				}

				//if (messageStatus==2)
				//	return;

				//std::this_thread::sleep_for(wait_duration);
			}
		}
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

int P2P::StartP2P(std::string addr, std::string port, std::string peerPort)
{
	Console console;
	Http http;

	console.NetworkPrint();
	console.WriteLine("Starting P2P Client");

	// Start winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return 0;
	}

	blockchainLength = FileCount("./wwwdata/blockchain/");

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

	// Bind the socket
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

	std::string otherIpPort = peerPort;
	//std::cout << "Peer IP:PORT combo > "; 
	//std::cin >> otherIpPort;

	//console.NetworkPrint();
	//console.WriteLine("Asking server for PEER address...");
	std::vector<std::string> args = {
		"query=AskForConnection",
		"ip_port=" + addr + ":" + port,
		"last_tried_ip_port=none"
	};
	std::string httpOut = TrimString(http.StartHttpWebRequest("http://api.achillium.us.to/dcc/p2pconn.php?", args));
	//console.NetworkPrint();
	//console.WriteLine("HTTP returned: " + httpOut);

	std::string last_tried_ip_port = "";

	// Start requesting peers to connect to from the handling server
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
		std::string httpOut = TrimString(http.StartHttpWebRequest("http://api.achillium.us.to/dcc/p2pconn.php?", args));

		// If the request fails or no peers are found, try again after 3 seconds
		if (httpOut == "" || httpOut.find("waiting") != std::string::npos || httpOut.find(addr + ":" + port) != std::string::npos)
		{
			console.NetworkPrint();
			console.WriteLine("Failed, waiting 3 sec to ask for a peer...");
			Sleep(3000); // Wait 3 seconds until next request
			continue;
		}

		console.NetworkPrint();
		console.WriteLine("HTTP2 returned: " + httpOut);

		otherIpPort = httpOut;
		last_tried_ip_port = otherIpPort;


		std::string host = SplitString(otherIpPort, ":")[0];
		int otherPort = stoi(SplitString(otherIpPort, ":")[1]);

		otherAddr.sin_port = htons(otherPort);
		otherAddr.sin_family = AF_INET;
		otherAddr.sin_addr.s_addr = inet_addr(host.c_str());

		otherSize = sizeof(otherAddr);

		// Listen to replies
		const unsigned int update_interval = 500; // update after every 500 milliseconds
		std::thread t1(&P2P::TaskRec, this, update_interval);

		bool noinput = false;

		// Begin sending messages, and stop when a reply is received
		for (messageAttempt = 0; messageAttempt < 8; messageAttempt++)
			//while (true)
		{
			// If not in idle state, continue sending messages
			if (messageStatus != -1 && !(noinput && messageStatus == 6)) {
				std::string msg = "";
				//int err = SafeSend(localSocket, msgConvert, msg.length());
				//std::cout << err << std::endl;

				console.NetworkPrint();
				console.Write("Send attempt: " + std::to_string(messageAttempt) + "  :  ");

				// If doing initial connect request
				if (messageStatus == 0) {
					msg = "peer$$$connect";
					console.Write(msg + "\n");
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// If doing peer confirmation
				else if ((messageStatus == 0 || messageStatus == 1 || messageStatus == 2)) {
					msg = "peer$$$success";
					console.Write(msg + "\n");
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);

					//console.WriteLine("Confirming");

					if (messageAttempt >= 4) {
						// After multiple confirmations have been sent, switch back to idle mode
						messageStatus = -1;
						messageAttempt = 0;
						//noinput = false;
						continue;
						//console.WriteLine("stopping thread...", console.yellowFGColor, "");
						//stop_thread_1 = true;
						//Sleep(8000);
						//t1.join();
						//stop_thread_1 = false;
						//console.WriteLine("stopped", console.yellowFGColor, "");

						//closesocket(localSocket);
						//WSACleanup();

						//console.WriteLine("Closing connection...", console.greenFGColor, "");

						//return 0;
					}
				}
				// Else if replying to height request
				else if (messageStatus == 3) {
					msg = "answer$$$height$$$" + std::to_string(blockchainLength);
					console.Write(msg + "\n");
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// Else if replying to block data request
				else if (messageStatus == 4) {
					// Open and read requested block
					std::ifstream td("./wwwdata/blockchain/block" + std::to_string(reqDat) + ".dccblock");
					std::stringstream bufferd;
					bufferd << td.rdbuf();
					std::string blockText = bufferd.str();
					std::string testSend = "{\"Version\":\"v0.01alpha-coin\",\"hash\":\"00000622bf7f189dccdb4701bb146e37b17a24c4b019a1c503b85e65b38d32c2\",\"lastHash\":\"00000c1999d1e108ed9205705eb98081635e11e81ec6356729e55a4e57a18663\",\"nonce\":\"47161\",\"time\":1671746241,\"transactionTimes\":[1671746235, 1671746238],\"transactions\":[\"1.000000->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735|cgbb0zbqtm/sTmB9OJPMI2dlWjcHpf21x+TBqCDRiz6DyoBCfmQnSNA9g/iiJo0ivibnfvRCD4AxbFSmsOKX2gLjwR1Ysgt65I7mIIcdc0+chUqDnu0a+X8LbSKYD6yCSr4rSD4955nU3s930DjVOgVmKxh7K6+2BJ2nx5GZic9owDDCYDbBHK01pYCBSEfnaIA1XOXeGWMtadMZAfW7as9k6ZXSeGpflVN3JdI3Fh107Z1wby6I94gmJt5Gw9sTTA6MCoB/K2GUmhQZ14N6f6VYJ3BxURJB+iiLxGtBINjReGqZFgwb8dtpX9RfOwQRGJC8rvv+Tjzk1qszaXseOw==|-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEApXRtTjPRE4XRamo84MIP4rjVqUnW91OZ/D7K5qXoLTyO9IOv1zui\nervc3Usp3uyDHpKnZdy2jb0czi6qC5nbaEFh6OlSGJmpa0MN++zxo4YkGwKwRf3N\n6SG8r0bOhipGyVOLOyh+q1oCBY9HqrFhZVgwtaunRurL2GLzG9o8hOr1+UIMoyFh\nzItKMOC26XxiCy2w8B9KOreJS/4Hucg94WsjweyiKFUe2+VUjgsw4Y0Mxg8lrCAP\nsZg0GkYLFsD69WtfHE+H2Dn+xeV+25ZU1fdMj4n4xY15NHg8s/3XTyDvCLOPjn6e\ni8D7LcG3jbyKkhZGq4v30kDiA6O/9d7T5QIBAw==\n-----END RSA PUBLIC KEY-----\n\",\"1.000000->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735|bsTog6pluw/85hWJ3JJQ2jNr/u+Gf5U32RCNFFNUx446Jvw0FqKg/Wh7cXIQTJ2DvVjAH0SiNt+ENgKTuLSTKvwP772cRPKVRBGy2rYE268+dpoIiw7dRv86lnP9JjQaOXkUgx2xQK+JnYHPDEPF+VBG8lLm19UM1YWl9v+Jb9ep//kfLJXzXVYp+rmzyCNb4hO6ehgF/NnuA1hgdwqbmRTLYPOHhdy6P5L8LW2v4ANUCGNCNFB+ajxb1G6yJdodX4OuqcC1Z/LQrjbSxCF2kXtv8p1RA3F9qG4WtMk62vi5otUDWo1W5aExiwOg79hLOTqBbSta9ZWOXR05lm/n1Qw==|-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEApXR";
					//std::string testSend = "{\"Version\":\"v0.01alpha-coin\",\"hash\":\"00000622bf7f189dccdb4701bb146e37b17a24c4b019a1c503b85e65b38d32c2\",\"lastHash\":\"00000c1999d1e108ed9205705eb98081635e11e81ec6356729e55a4e57a18663\",\"nonce\":\"47161\",\"time\":1671746241,\"transactionTimes\":[1671746235, 1671746238],\"transactions\":[\"1.000000->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735|cgbb0zbqtm/sTmB9OJPMI2dlWjcHpf21x+TBqCDRiz6DyoBCfmQnSNA9g/iiJo0ivibnfvRCD4AxbFSmsOKX2gLjwR1Ysgt65I7mIIcdc0+chUqDnu0a+X8LbSKYD6yCSr4rSD4955nU3s930DjVOgVmKxh7K6+2BJ2nx5GZic9owDDCYDbBHK01pYCBSEfnaIA1XOXeGWMtadMZAfW7as9k6ZXSeGpflVN3JdI3Fh107Z1wby6I94gmJt5Gw9sTTA6MCoB/K2GUmhQZ14N6f6VYJ3BxURJB+iiLxGtBINjReGqZFgwb8dtpX9RfOwQRGJC8rvv+Tjzk1qszaXseOw==|-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEApXRtTjPRE4XRamo84MIP4rjVqUnW91OZ/D7K5qXoLTyO9IOv1zui\nervc3Usp3uyDHpKnZdy2jb0czi6qC5nbaEFh6OlSGJmpa0MN++zxo4YkGwKwRf3N\n6SG8r0bOhipGyVOLOyh+q1oCBY9HqrFhZVgwtaunRurL2GLzG9o8hOr1+UIMoyFh\nzItKMOC26XxiCy2w8B9KOreJS/4Hucg94WsjweyiKFUe2+VUjgsw4Y0Mxg8lrCAP\nsZg0GkYLFsD69WtfHE+H2Dn+xeV+25ZU1fdMj4n4xY15NHg8s/3XTyDvCLOPjn6e\ni8D7LcG3jbyKkhZGq4v30kDiA6O/9d7T5QIBAw==\n-----END RSA PUBLIC KEY-----\n\",\"1.000000->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735->3bc5832b5c8939549526b843337267b25f67393142015fe3aa7294bbd125a735|bsTog6pluw/85hWJ3JJQ2jNr/u+Gf5U32RCNFNUx446Jvw0FqKg/Wh7cXIQTJ2DvVjAH0SiNt+ENgKTuLSTKvwP772cRPKVRBGy2rYE268+dpoIiw7dRv86lnP9JjQaOXkUgx2xQK+JnYHPDEPF+VBG8lLm19UM1YWl9v+Jb9ep//kfLJXzXVYp+rmzyCNb4hO6ehgF/NnuA1hgdwqbmRTLYPOHhdy6P5L8LW2v4ANUCGNCNFB+ajxb1G6yJdodX4OuqcC1Z/LQrjbSxCF2kXtv8p1RA3F9qG4WtMk62vi5otUDWo1W5aExiwOg79hLOTqBbSta9ZWOXR05lm/n1Qw==|-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEApXRtTjPRE4XRamo84MIP4rjVqUnW91OZ/D7K5qXoLTyO9IOv1zui\nervc3Usp3uyDHpKnZdy2jb0czi6qC5nbaEFh6OlSGJmpa0MN++zxo4YkGwKwRf3N\n6SG8r0bOhipGyVOLOyh+q1oCBY9HqrFhZVgwtaunRurL2GLzG9o8hOr1+UIMoyFh\nzItKMOC26XxiCy2w8B9KOreJS/4Hucg94WsjweyiKFUe2+VUjgsw4Y0Mxg8lrCAP\nsZg0GkYLFsD69WtfHE+H2Dn+xeV+25ZU1fdMj4n4xY15NHg8s/3XTyDvCLOPjn6e\ni8D7LcG3jbyKkhZGq4v30kDiA6O/9d7T5QIBAw==\n-----END RSA PUBLIC KEY-----\n\"];

					msg = "answer$$$block$$$" + std::to_string(reqDat) + "$$$" + ReplaceEscapeSymbols(testSend);
					console.Write(msg + "\n");
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					Sleep(3000);
				}
				// Else if requesting chain height
				else if (messageStatus == 5) {
					msg = "request$$$height";
					console.Write(msg + "\n");
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					//// Wait extra 3 seconds
					//Sleep(3000);
				}
				// Else if requesting block data
				else if (messageStatus == 6) {
					msg = "request$$$block$$$" + std::to_string(reqDat);
					console.Write(msg + "\n");
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					// Wait extra 3 seconds
					Sleep(3000);
					noinput = true;
				}
				// Wait 3 seconds before sending next message
				Sleep(3000);
			}
			// Else if in idle state, request input (temporary) and execute that input
			else if (!noinput) {
				// Request console input
				std::string inputCmd = "";
				console.Write("P2P Shell $  ");
				//std::cout << "Network Input*>  ";
				std::getline(std::cin, inputCmd);

				// Reset attempts to 0, since we are currently not sending messages
				messageAttempt = 0;

				// If the inputted command is blank or too small to be a command, try again.
				if (inputCmd.length() == 0)
					continue;

				std::vector<std::string> cmdArgs = SplitString(inputCmd, " ");

				// If user inputted `height` command
				if (cmdArgs[0] == "height") {
					messageStatus = 5;
					messageAttempt = 0;
				}
				// If user inputted `syncblock` command, and an argument
				else if (cmdArgs[0] == "syncblock" && cmdArgs.size() >= 2) {
					reqDat = std::stoi(cmdArgs[1]); // Block number is the first argument
					messageStatus = 6;
					messageAttempt = 0;
					Sleep(1000);
				}
				// If user inputted `noinput` command, stop requesting console input so the main thread is free to reply
				else if (cmdArgs[0] == "noinput") {
					noinput = true;
				}
				else {
					messageStatus = -1;
					console.ErrorPrint();
					console.WriteLine("Command not found: \"" + inputCmd + "\"");
					//std::cout << "Command not found: \"" + inputCmd + "\"\n";
				}
			}
			else {
				messageAttempt = 0;
				console.WriteLine("listener status: " + std::to_string(thread_running));
			}
		}

		console.WriteLine("Unsuccessful connection, trying other peer", console.redFGColor, "");

		// Stop the current listening thread and continue
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
