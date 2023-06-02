
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <math.h>

#include "strops.h"
#include "P2PClient.h"
#include "Console.h"
#include <boost/process.hpp>
#include <chrono>
#include "Network.h"
#include "FileManip.h"
#include "SettingsConsts.h"

#pragma comment(lib,"ws2_32.lib")

const int BUFFERLENGTH = 1024 * 8;

char buffer[BUFFERLENGTH];

int blockchainLength = 0;
int peerBlockchainLength = 0;

//SOCKET localSocket;
SOCKADDR_IN otherAddr;
std::string otherAddrStr;
int otherSize;
std::atomic_bool stop_thread_1 = false;
std::atomic_bool stop_thread_2 = false;
std::atomic_bool thread_running = false;
//Console console;

std::vector<std::string> peerList;

// Get the IP:Port combination from SOCKADDR_IN struct, and return it as a string
std::string P2P::NormalizedIPString(SOCKADDR_IN addr) {
	char otherIP[16];
	ZeroMemory(otherIP, 16);
	inet_ntop(AF_INET, &addr.sin_addr, otherIP, 16);

	USHORT port = ntohs(addr.sin_port);

	int realLen = 0;
	for (int i = 0; i < 16; i++) {
		if (otherIP[i] == '\00') {
			break;
		}
		realLen++;
	}

	std::string res(otherIP, realLen);
	res += ":" + std::to_string(port);

	return res;
}

// Safely send some data as a string, and split large amounts of data into multiple segments to be sent sequentially.
int mySendTo(int socket, std::string& s, int len, int redundantFlags, sockaddr* to, int toLen)
{
	int total = 0;        // how many bytes we've sent
	int bytesleft = len; // how many we have left to send
	SSIZE_T n;

	const char* p = s.c_str();

	int segmentCount = 1;
	while (total < len) {
		//std::string segInfo = "seg " + + " of " + + ", " + + " bytes|";
		std::string segInfo = "seg :" + std::to_string(segmentCount) + ": of :" + std::to_string((int)ceil((float)len / 1000.0f)) + ": , :" + std::to_string((bytesleft < 1000) ? bytesleft : 1000) + ": bytes|||";

		int segSize = segInfo.size();

		segInfo += (p + total);

		n = sendto(socket,
			segInfo.c_str(),
			(bytesleft < 1000) ? (bytesleft + segSize) : (1000 + segSize),
			0,
			to,
			toLen)
			- segSize; // Don't include segment info when counting data, so subtract this
		if (n == -1) { break; }
		total += n;
		if (constants::debugPrint == true) {
			std::cout << std::to_string((int)round(100 * ((float)total / (float)len))) << "% sent" << std::endl;
		}
		if (bytesleft < 1000)
			bytesleft -= n;
		else
			bytesleft -= 1000;

		segmentCount++;
	}
	if (constants::debugPrint == true) {
		std::cout << "Done." << std::endl;
	}

	len = total; // return number actually sent here

	return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

// The function that is run in a thread in order to listen for received data in the background
void P2P::ListenerThread(int update_interval)
{
	thread_running = true;
	while (true) {
		Console console;

		SOCKADDR_IN remoteAddr;
		int remoteAddrLen = sizeof(remoteAddr);

		DWORD timeout = 1 * 1000;
		setsockopt(localSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		if (stop_thread_1) {
			thread_running = false;
			return;
		}

		bool pendingReceiveData = false;
		int currentPendingSegment = 0;
		std::string totalMessage = "";

		while (!stop_thread_1)
		{
			if (stop_thread_1) {
				thread_running = false;
				break;
			}
			//int t = select(-1, &stReadFDS, 0, 0, &stTimeOut);
			if (false) {
				//if (t == SOCKET_ERROR) {
				console.NetworkErrorPrint();
				console.WriteLine("Error: Socket Error, trying again...");
				//break;
			}
			else {
				//std::cout << "WAITING TO RECEIVE" <<std::endl;
				int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);
				//console.WriteLine("Checked, parsing " + iResult);
				std::cout << "iResult: " << std::to_string(iResult) << std::endl;
				if (iResult > 0) {

					// Get the IPV4 address:port of the received data. If it
					// matches the expected one, continue. If it does not, then
					// stop. If the current one is blank or has disconnected,
					// set this one as the current connection and continue.
					std::string fromIPString = NormalizedIPString(remoteAddr);

					// If not currently connected, accept this connection.
					if (otherAddrStr == "")
						otherAddrStr = fromIPString;

					// If connected but different, ignore.
					else if (SplitString(fromIPString, ":")[0] != SplitString(otherAddrStr, ":")[0])
						continue;

					// Read the received data buffer into a string
					std::string textVal = std::string(buffer, buffer + iResult);

					// Get the segment information from the received data
					std::string segInfo = SplitString(textVal, "|||")[0];
					int segNumber = std::stoi(SplitString(segInfo, ":")[1]);
					int maxSegments = std::stoi(SplitString(segInfo, ":")[3]);
					std::string content = SplitString(textVal, "|||")[1];

					console.WriteLine(segInfo, console.yellowFGColor, "");

					// If we are currently still waiting for more data to be received
					if (pendingReceiveData) {
						totalMessage += content;
						// If the current segment number is less than the last one, 
						// this must be different data than we were receiving before,
						// so cancel.
						if (currentPendingSegment > segNumber) {
							currentPendingSegment = 0;
							pendingReceiveData = false;
							totalMessage = "";
							continue;
						}
						// Else if the maximum number of segments was reached, stop
						// Pending receiving data
						else if (maxSegments == segNumber) {
							currentPendingSegment = 0;
							pendingReceiveData = false;
						}
						// Else if the maximum number of segments was NOT reached,
						// continue receiving pending data
						else if (maxSegments > segNumber && segNumber == 1) {
							currentPendingSegment = segNumber;
							continue;
						}
					}
					// Else if the maximum number of segments is greater than
					// the current one, and the current one is 1, that means
					// this is the first and this needs to wait for more data
					// to arrive.
					else if (maxSegments > segNumber && segNumber == 1) {
						currentPendingSegment = segNumber;
						pendingReceiveData = true;
						totalMessage = content; // Clear total message string and overwrite with current new data
						continue;
					}
					// Else, this is a single segment message, and so the
					// totalMessage` variable can be set to the content
					else
						totalMessage = content;

					// If the peer is requesting to connect
					if (totalMessage == "peer$$$connect") {
						if (constants::debugPrint == true) {
							console.DebugPrint();
							console.WriteLine("Received initial connection, awaiting confirmation...", console.greenFGColor, "");
						}
						messageStatus = await_first_success; // Awaiting confirmation status
						messageAttempt = 0;

						CONNECTED_TO_PEER = true;

						// Add item to peer list, and save to file
						bool alreadyInList = false;
						for (int y = 0; y < peerList.size(); y++) {
							if (otherAddrStr == peerList[y]) {
								alreadyInList = true;
								break;
							}
						}
						if (alreadyInList == false) {
							peerList.push_back(otherAddrStr);
							std::string totalList = "";
							for (int y = 0; y < peerList.size(); y++)
								totalList += peerList[y] + "\n";
							std::ofstream peerFileW("./wwwdata/peerlist.list");
							if (peerFileW.is_open())
							{
								peerFileW << totalList;
								peerFileW.close();
							}
							peerFileW.close();
						}
					}
					// If the peer is ending the connection
					else if (totalMessage == "peer$$$disconnect") {
						console.NetworkErrorPrint();
						console.WriteLine("Peer closed.");
						//CONNECTED_TO_PEER = false;
						thread_running = false;
						messageStatus = disconnect_request;
						return;
					}
					// If the peer is requesting message received confirmation
					else if (totalMessage == "peer$$$success" && (messageStatus >= 0)) {
						if (constants::debugPrint == true) {
							console.DebugPrint();
							console.WriteLine("Dual Confirmation", console.greenFGColor, "");
						}
						messageStatus = await_second_success; // Confirmed message status, continue sending our own 
						// confirm 2 times, then switch to idle state -1
						CONNECTED_TO_PEER = true;
					}
					// If the peer is idling
					else if (totalMessage == "peer$$$idle") {
						if (constants::debugPrint == true) {
							console.DebugPrint();
							console.WriteLine("idle...", console.yellowFGColor, "");
						}
					}
					// If peer is requesting data
					else if (SplitString(totalMessage, "$$$")[0] == "request") {
						// If peer is asking for blockchain height
						if (SplitString(totalMessage, "$$$")[1] == "height")
							messageStatus = replying_height;
						// If peer is asking for a pending block's data
						else if (SplitString(totalMessage, "$$$")[1] == "pendingblock") {
							messageStatus = replying_pendingblock;
							reqDat = std::stoi(SplitString(totalMessage, "$$$")[2]);
						}
						// If peer is asking for a block's data
						else if (SplitString(totalMessage, "$$$")[1] == "block") {
							messageStatus = replying_block;
							reqDat = std::stoi(SplitString(totalMessage, "$$$")[2]);
						}
						// If peer is asking for this peer's peerList
						else if (SplitString(totalMessage, "$$$")[1] == "peerlist")
							messageStatus = replying_peer_list;

						if (constants::debugPrint == true) {
							console.WriteLine("request " + std::to_string(messageStatus), console.greenFGColor, "");
						}
					}
					// If peer is answering request
					else if (SplitString(totalMessage, "$$$")[0] == "answer") {
						// If peer is giving blockchain height
						if (SplitString(totalMessage, "$$$")[1] == "height") {
							peerBlockchainLength = std::stoi(SplitString(totalMessage, "$$$")[2]);
							messageStatus = await_first_success;
							if (constants::debugPrint == true) {
								console.WriteLine("answer height: " + std::to_string(peerBlockchainLength), console.greenFGColor, "");
							}
						}
						// If peer is giving peer list
						else if (SplitString(totalMessage, "$$$")[1] == "peerlist") {
							std::vector<std::string> receivedPeers = SplitString(SplitString(totalMessage, "$$$")[2], ",");
							// Iterate all received peers, and only add them to our list if it is not already on it
							for (int x = 0; x < receivedPeers.size(); x++) {
								bool wasFound = false;
								for (int y = 0; y < peerList.size(); y++) {
									if (receivedPeers[x] == peerList[y]) {
										wasFound = true;
										break;
									}
								}
								if (wasFound == true)
									peerList.push_back(receivedPeers[x]);
							}
							messageStatus = await_first_success;
							//console.WriteLine("answer peerlist: " + std::to_string(peerBlockchainLength), console.greenFGColor, "");
						}
						// If peer is giving a block's data
						else if (SplitString(totalMessage, "$$$")[1] == "block") {
							messageStatus = await_first_success;
							int num = std::stoi(SplitString(totalMessage, "$$$")[2]);
							std::string blockData = SplitString(totalMessage, "$$$")[3];

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

							if (constants::debugPrint == true) {
								console.WriteLine("answer block: " + std::to_string(num), console.greenFGColor, "");
							}
						}
						messageAttempt = 0;

					}
					if (constants::debugPrint == true) {
						console.WriteLine("received: " + NormalizedIPString(remoteAddr) + " -> " + totalMessage + "\t status: " + std::to_string(messageStatus));
					}
				}
				else if(WSAGetLastError() != WSAETIMEDOUT) {
					console.NetworkErrorPrint();
					console.WriteLine("Error: Peer closed.");
					CONNECTED_TO_PEER = false;
					thread_running = false;
					return;
				}
			}
		}
	}
}


// The function to open the socket required for the P2P connection
int P2P::OpenP2PSocket(int port)
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

	stop_thread_1 = false;
	stop_thread_2 = false;

	localSocket = socket(AF_INET, SOCK_DGRAM, 0);

	SOCKADDR_IN clientAddr;
	clientAddr.sin_port = htons(port);
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket
	if (bind(localSocket, (LPSOCKADDR)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
	{
		console.ErrorPrint();
		console.WriteLine("Failed to bind socket");
		closesocket(localSocket);
		WSACleanup();
		return 0;
	}

	int val = 2048;
	setsockopt(localSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));
	setsockopt(localSocket, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val));

	//closesocket(localSocket);
	//WSACleanup();

	return 0;
}

// The function that is run in a thread in order to reply or send data to a peer in the background
void P2P::SenderThread()
{
	Console console;
	Http http;

	stop_thread_2 = false;

	// Start requesting possible peers to connect to, from the handling server
	//for (int t = 0; t < 20; t++)
	while (true)
	{
	//	// If not connected to a peer, continue
	//	if (!CONNECTED_TO_PEER)
	//		continue;
	
		std::string otherIP = SplitString(peerList[0], ":")[0];
		int otherPort = stoi(SplitString(peerList[0], ":")[1]);

		otherAddr.sin_port = htons(otherPort);
		otherAddr.sin_family = AF_INET;
		otherAddr.sin_addr.s_addr = inet_addr(otherIP.c_str());

		otherSize = sizeof(otherAddr);

		bool noinput = false;


		// If not replying to messages or requesting something, continue
		if (messageStatus == idle)
			continue;

		// Begin sending messages, and stop when a reply is received
		for (messageAttempt = 0; messageAttempt < 10; messageAttempt++)
			//while (true)
		{
			// If not in idle state, continue sending messages
			if (messageStatus != idle && !(noinput && (messageStatus == requesting_block || messageStatus == requesting_pendingblock))) {
				std::string msg = "";

				std::cout << "\r";
				console.NetworkPrint();
				if (constants::debugPrint == true)
					console.Write("Send attempt : " + std::to_string(messageAttempt) + "  " + otherIP + ":" + std::to_string(otherPort) + "    ");
				else
					console.Write("Send attempt: " + std::to_string(messageAttempt) + "    ");

				// If doing initial connect request
				if (messageStatus == initial_connect_request) {
					msg = "peer$$$connect";
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// If doing disconnect request
				else if (messageStatus == disconnect_request) {
					msg = "peer$$$disconnect";
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// If doing peer confirmation
				else if ((messageStatus == initial_connect_request || messageStatus == await_first_success || messageStatus == await_second_success)) {
					msg = "peer$$$success";
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);

					//console.WriteLine("Confirming");

					// After multiple confirmations have been sent, switch back to idle mode
					if (messageAttempt >= 2) {
						messageStatus = idle;
						messageAttempt = 0;
						continue;
					}
				}
				// Else if replying to height request
				else if (messageStatus == replying_height) {
					msg = "answer$$$height$$$" + std::to_string(blockchainLength);
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// Else if replying to pending block data request
				else if (messageStatus == replying_pendingblock) {
					// Open and read requested block
					std::ifstream td("./wwwdata/pendingblocks/block" + std::to_string(reqDat) + ".dccblock");
					std::stringstream bufferd;
					bufferd << td.rdbuf();
					std::string blockText = bufferd.str();

					msg = "answer$$$pendingblock$$$" + std::to_string(reqDat) + "$$$" + ReplaceEscapeSymbols(blockText);
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					Sleep(3000);
				}
				// Else if replying to block data request
				else if (messageStatus == replying_block) {
					// Open and read requested block
					std::ifstream td("./wwwdata/blockchain/block" + std::to_string(reqDat) + ".dccblock");
					std::stringstream bufferd;
					bufferd << td.rdbuf();
					std::string blockText = bufferd.str();

					msg = "answer$$$block$$$" + std::to_string(reqDat) + "$$$" + ReplaceEscapeSymbols(blockText);
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					Sleep(3000);
				}
				// Else if replying to peer list request
				else if (messageStatus == replying_peer_list) {
					std::string totalPeersString = "";
					for (int i = 0; i < peerList.size() && i < 10; i++)
						totalPeersString += peerList[i] + ((i == peerList.size() - 1 || i == 9) ? "" : ",");

					msg = "answer$$$peerlist$$$" + totalPeersString;
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					Sleep(3000);
				}
				// Else if requesting chain height
				else if (messageStatus == requesting_height) {
					msg = "request$$$height";
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					//// Wait extra 3 seconds
					//Sleep(3000);
				}
				// Else if requesting pending block data
				else if (messageStatus == requesting_pendingblock) {
					msg = "request$$$pendingblock$$$" + std::to_string(reqDat);
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					// Wait extra 3 seconds
					Sleep(3000);
					//noinput = true;
				}
				// Else if requesting block data
				else if (messageStatus == requesting_block) {
					msg = "request$$$block$$$" + std::to_string(reqDat);
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					// Wait extra 3 seconds
					Sleep(3000);
					//noinput = true;
				}
				// Else if requesting peer list
				else if (messageStatus == requesting_peer_list) {
					msg = "request$$$peerlist";
					if (constants::debugPrint == true) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					//// Wait extra 3 seconds
					//Sleep(3000);
					//noinput = true;
				}
				// Wait 3 seconds before sending next message
				Sleep(3000);
			}
			// Else if in idle state, request input (temporary) and execute that input
			else if (!noinput) {
				// Request console input
				std::string inputCmd = "";
				console.Write("\n\nP2P Shell $  ");
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
					messageStatus = requesting_height;
					messageAttempt = 0;
				}
				// If user inputted `syncblock` command, and an argument
				else if (cmdArgs[0] == "syncblock" && cmdArgs.size() >= 2) {
					reqDat = std::stoi(cmdArgs[1]); // Block number is the first argument
					messageStatus = requesting_block;
					messageAttempt = 0;
					Sleep(1000);
				}
				// If user inputted `noinput` command, stop requesting console input so the main thread is free to reply
				else if (cmdArgs[0] == "noinput") {
					noinput = true;
				}
				// If user inputted `syncpeers` command, request a list of known peers from the connection
				else if (cmdArgs[0] == "syncpeers") {
					messageStatus = requesting_peer_list;
					messageAttempt = 0;
				}
				// If user inputted `exit` command, close the connection and exit the P2P shell
				else if (cmdArgs[0] == "exit") {
					messageStatus = disconnect_request; // Return disconnect request, so the other peer thread knows to shut down
					messageAttempt = 0;
					/*stop_thread_1 = true;
					t1.join();
					//stop_thread_1 = false;

					closesocket(localSocket);
					WSACleanup();

					return 0;*/
				}
				else {
					messageStatus = idle;
					console.ErrorPrint();
					console.WriteLine("Command not found: \"" + inputCmd + "\"");
					//std::cout << "Command not found: \"" + inputCmd + "\"\n";
				}
			}
			// If in listen mode, but the peer has disconnected, exit listen mode and close connection
			else if (CONNECTED_TO_PEER == false) {
				//stop_thread_1 = true;
				//t1.join();
				//stop_thread_1 = false;

				closesocket(localSocket);
				WSACleanup();
			}
			// Otherwise, we are in listen mode and still connected
			else {
				messageAttempt = 0;
				console.WriteLine("listener status: " + std::to_string(thread_running));
				Sleep(100);
			}
		}

		console.NetworkErrorPrint();
		console.WriteLine("Peer Timed out", console.redFGColor, "");

		messageStatus = idle;
		messageAttempt = 0;

		// Stop the current listening thread and continue
		//stop_thread_1 = true;
		//t1.join();
		//t1.detach();
		//stop_thread_1 = false;

		//closesocket(localSocket);
		//WSACleanup();

		continue;
	}

	closesocket(localSocket);
	WSACleanup();
}

// The function to start the P2P node, get IP address, and try to connect to another peer.
// TODO: Break this function into multiple parts, for starting the connection and winsock,
//       sending and handling messages and client states, and safely closing the connection
int P2P::StartP2P(std::string addr, std::string port, std::string peerPort)
{
	//	Console console;
	//	Http http;
	//
	//	console.NetworkPrint();
	//	console.WriteLine("Starting P2P Client");
	//
	//	// Start winsock
	//	WSADATA wsaData;
	//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
	//		return 0;
	//	}
	//
	//	blockchainLength = FileCount("./wwwdata/blockchain/");
	//
	//	stop_thread_1 = false;
	//
	//	//SOCKADDR_IN serverAddr;
	//	//serverAddr.sin_port = htons(6668);
	//	//serverAddr.sin_family = AF_INET;
	//	//serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	//
	//	//int serverSize = sizeof(serverAddr);
	//
	//	localSocket = socket(AF_INET, SOCK_DGRAM, 0);
	//
	//	SOCKADDR_IN clientAddr;
	//	clientAddr.sin_port = htons(stoi(port));
	//	clientAddr.sin_family = AF_INET;
	//	clientAddr.sin_addr.s_addr = INADDR_ANY;
	//
	//	//		address.sin_family = AF_INET;
	//	//		address.sin_addr.s_addr = INADDR_ANY;
	//	//		address.sin_port = htons(port);
	//
	//	// Bind the socket
	//	if (bind(localSocket, (LPSOCKADDR)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
	//	{
	//		console.ErrorPrint();
	//		console.WriteLine("Failed to bind socket");
	//		closesocket(localSocket);
	//		WSACleanup();
	//		return 0;
	//	}
	//
	//	int val = 2048;
	//	setsockopt(localSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));
	//	setsockopt(localSocket, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val));
	//	//
	//	//std::string request = "1";
	//	//std::cout << "Identification number: ";  std::cin >> request;
	//	//
	//	//sendto(localSocket, request.c_str(), request.length(), 0, (sockaddr*)&serverAddr, serverSize);
	//
	//	bool notFound = true;
	//
	//	std::string otherIpPort = peerPort;
	//	//std::cout << "Peer IP:PORT combo > "; 
	//	//std::cin >> otherIpPort;
	//
	//	//console.NetworkPrint();
	//	//console.WriteLine("Asking server for PEER address...");
	//	std::vector<std::string> args = {
	//		"query=AskForConnection",
	//		"ip_port=" + addr + ":" + port,
	//		"last_tried_ip_port=none"
	//	};
	//	std::string httpOut = TrimString(http.StartHttpWebRequest("http://api.achillium.us.to/dcc/p2pconn.php?", args));
	//	//console.NetworkPrint();
	//	//console.WriteLine("HTTP returned: " + httpOut);
	//
	//	std::string last_tried_ip_port = "";
	//
	//	// Start requesting possible peers to connect to, from the handling server
	//	for (int t = 0; t < 20; t++)
	//		//while (true)
	//	{
	//		std::cout << "\r";
	//		console.NetworkPrint();
	//		console.WriteLine("Attempt: " + std::to_string(t) + "  |  Asking server for PEER address...    ");
	//
	//		std::vector<std::string> args = {
	//			"query=WaitForConnection",
	//			"ip_port=" + addr + ":" + port,
	//			"last_tried_ip_port=" + last_tried_ip_port
	//		};
	//		std::string httpOut = TrimString(http.StartHttpWebRequest("http://api.achillium.us.to/dcc/p2pconn.php?", args));
	//		//otherAddrStr = last_tried_ip_port;
	//
	//		// If the request fails or no peers are found, try again after 3 seconds
	//		if (httpOut == "" || httpOut.find("waiting") != std::string::npos || httpOut.find(addr + ":" + port) != std::string::npos)
	//		{
	//			if (constants::debugPrint == true) {
	//				console.NetworkPrint();
	//				console.WriteLine("No peers found, waiting 3 sec to ask again...");
	//			}
	//			Sleep(3000); // Wait 3 seconds until next request
	//			continue;
	//		}
	//
	//		if (constants::debugPrint == true) {
	//			console.NetworkPrint();
	//			console.WriteLine("Server returned: " + httpOut);
	//		}
	//
	//		otherIpPort = httpOut;
	//		last_tried_ip_port = otherIpPort;
	//		otherAddrStr = otherIpPort;
	//
	//		// Add item to peer list, and save to file
	//		bool alreadyInList = false;
	//		for (int y = 0; y < peerList.size(); y++) {
	//			if (otherAddrStr == peerList[y]) {
	//				alreadyInList = true;
	//				break;
	//			}
	//		}
	//		if (alreadyInList == false) {
	//			peerList.push_back(otherAddrStr);
	//			std::string totalList = "";
	//			for (int y = 0; y < peerList.size(); y++)
	//				totalList += peerList[y] + "\n";
	//			std::ofstream peerFileW("./wwwdata/peerlist.list", std::ios::out | std::ios::trunc);
	//			if (peerFileW.is_open())
	//			{
	//				peerFileW << totalList;
	//				peerFileW.close();
	//			}
	//			peerFileW.close();
	//		}
	//
	//
	//		std::string otherIP = SplitString(otherIpPort, ":")[0];
	//		int otherPort = stoi(SplitString(otherIpPort, ":")[1]);
	//
	//		otherAddr.sin_port = htons(otherPort);
	//		otherAddr.sin_family = AF_INET;
	//		otherAddr.sin_addr.s_addr = inet_addr(otherIP.c_str());
	//
	//		otherSize = sizeof(otherAddr);
	//
	//		// Listen to replies
	//		const unsigned int update_interval = 500; // update after every 500 milliseconds
	//		std::thread t1(&P2P::ListenerThread, this, update_interval);
	//
	//		bool noinput = false;
	//
	//		// Only receive if in the idle state.
	//		while (messageStatus == idle) {}
	//
	//		// Begin sending messages, and stop when a reply is received
	//		for (messageAttempt = 0; messageAttempt < 10; messageAttempt++)
	//			//while (true)
	//		{
	//			// If not in idle state, continue sending messages
	//			if (messageStatus != idle && !(noinput && messageStatus == requesting_block)) {
	//				std::string msg = "";
	//				//int err = SafeSend(localSocket, msgConvert, msg.length());
	//				//std::cout << err << std::endl;
	//
	//				std::cout << "\r";
	//				console.NetworkPrint();
	//				if (constants::debugPrint == true)
	//					console.Write("Send attempt : " + std::to_string(messageAttempt) + "    ");
	//				else
	//					console.Write("Send attempt: " + std::to_string(messageAttempt) + "    ");
	//
	//				// If doing initial connect request
	//				if (messageStatus == initial_connect_request) {
	//					msg = "peer$$$connect";
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//				}
	//				// If doing disconnect request
	//				if (messageStatus == disconnect_request) {
	//					msg = "peer$$$disconnect";
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//				}
	//				// If doing peer confirmation
	//				else if ((messageStatus == initial_connect_request || messageStatus == await_first_success || messageStatus == await_second_success)) {
	//					msg = "peer$$$success";
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//
	//					//console.WriteLine("Confirming");
	//
	//					// After multiple confirmations have been sent, switch back to idle mode
	//					if (messageAttempt >= 2) {
	//						messageStatus = idle;
	//						messageAttempt = 0;
	//						continue;
	//					}
	//				}
	//				// Else if replying to height request
	//				else if (messageStatus == replying_height) {
	//					msg = "answer$$$height$$$" + std::to_string(blockchainLength);
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//				}
	//				// Else if replying to block data request
	//				else if (messageStatus == replying_block) {
	//					// Open and read requested block
	//					std::ifstream td("./wwwdata/blockchain/block" + std::to_string(reqDat) + ".dccblock");
	//					std::stringstream bufferd;
	//					bufferd << td.rdbuf();
	//					std::string blockText = bufferd.str();
	//
	//					msg = "answer$$$block$$$" + std::to_string(reqDat) + "$$$" + ReplaceEscapeSymbols(blockText);
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//					Sleep(3000);
	//				}
	//				// Else if replying to peer list request
	//				else if (messageStatus == replying_peer_list) {
	//					std::string totalPeersString = "";
	//					for (int i = 0; i < peerList.size() && i < 10; i++)
	//						totalPeersString += peerList[i] + ((i == peerList.size() - 1 || i == 9) ? "" : ",");
	//
	//					msg = "answer$$$peerlist$$$" + totalPeersString;
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//					Sleep(3000);
	//				}
	//				// Else if requesting chain height
	//				else if (messageStatus == requesting_height) {
	//					msg = "request$$$height";
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//					//// Wait extra 3 seconds
	//					//Sleep(3000);
	//				}
	//				// Else if requesting block data
	//				else if (messageStatus == requesting_block) {
	//					msg = "request$$$block$$$" + std::to_string(reqDat);
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//					// Wait extra 3 seconds
	//					Sleep(3000);
	//					//noinput = true;
	//				}
	//				// Else if requesting peer list
	//				else if (messageStatus == requesting_peer_list) {
	//					msg = "request$$$peerlist";
	//					if (constants::debugPrint == true) {
	//						console.Write(msg + "\n");
	//					}
	//					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
	//					//// Wait extra 3 seconds
	//					//Sleep(3000);
	//					//noinput = true;
	//				}
	//				// Wait 3 seconds before sending next message
	//				Sleep(3000);
	//			}
	//			// Else if in idle state, request input (temporary) and execute that input
	//			else if (!noinput) {
	//				// Request console input
	//				std::string inputCmd = "";
	//				console.Write("\n\nP2P Shell $  ");
	//				//std::cout << "Network Input*>  ";
	//				std::getline(std::cin, inputCmd);
	//
	//				// Reset attempts to 0, since we are currently not sending messages
	//				messageAttempt = 0;
	//
	//				// If the inputted command is blank or too small to be a command, try again.
	//				if (inputCmd.length() == 0)
	//					continue;
	//
	//				std::vector<std::string> cmdArgs = SplitString(inputCmd, " ");
	//
	//				// If user inputted `height` command
	//				if (cmdArgs[0] == "height") {
	//					messageStatus = requesting_height;
	//					messageAttempt = 0;
	//				}
	//				// If user inputted `syncblock` command, and an argument
	//				else if (cmdArgs[0] == "syncblock" && cmdArgs.size() >= 2) {
	//					reqDat = std::stoi(cmdArgs[1]); // Block number is the first argument
	//					messageStatus = requesting_block;
	//					messageAttempt = 0;
	//					Sleep(1000);
	//				}
	//				// If user inputted `noinput` command, stop requesting console input so the main thread is free to reply
	//				else if (cmdArgs[0] == "noinput") {
	//					noinput = true;
	//				}
	//				// If user inputted `syncpeers` command, request a list of known peers from the connection
	//				else if (cmdArgs[0] == "syncpeers") {
	//					messageStatus = requesting_peer_list;
	//					messageAttempt = 0;
	//				}
	//				// If user inputted `exit` command, close the connection and exit the P2P shell
	//				else if (cmdArgs[0] == "exit") {
	//					messageStatus = disconnect_request;
	//					messageAttempt = 0;
	//					/*stop_thread_1 = true;
	//					t1.join();
	//					stop_thread_1 = false;
	//
	//					closesocket(localSocket);
	//					WSACleanup();
	//
	//					return 0;*/
	//				}
	//				else {
	//					messageStatus = idle;
	//					console.ErrorPrint();
	//					console.WriteLine("Command not found: \"" + inputCmd + "\"");
	//					//std::cout << "Command not found: \"" + inputCmd + "\"\n";
	//				}
	//			}
	//			// If in listen mode, but the peer has disconnected, exit listen mode and close connection
	//			else if(CONNECTED_TO_PEER == false) {
	//				stop_thread_1 = true;
	//				t1.join();
	//				stop_thread_1 = false;
	//
	//				closesocket(localSocket);
	//				WSACleanup();
	//
	//				return 0;
	//			}
	//			// Otherwise, we are in listen mode and still connected
	//			else {
	//				messageAttempt = 0;
	//				console.WriteLine("listener status: " + std::to_string(thread_running));
	//				Sleep(100);
	//			}
	//		}
	//
	//		console.NetworkErrorPrint();
	//		console.WriteLine("Peer Timed out", console.redFGColor, "");
	//
	//		messageStatus = initial_connect_request;
	//		messageAttempt = 0;
	//
	//		// Stop the current listening thread and continue
	//		stop_thread_1 = true;
	//		//t1.join();
	//		t1.detach();
	//		stop_thread_1 = false;
	//
	//		closesocket(localSocket);
	//		WSACleanup();
	//
	//		return 0;
	//
	//		continue;
	//	}
	//
	//	closesocket(localSocket);
	//	WSACleanup();
	//
	return 0;
}
