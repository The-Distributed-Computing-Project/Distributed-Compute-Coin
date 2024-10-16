


#include "P2PClient.h"

#pragma comment(lib,"ws2_32.lib")

const int BUFFERLENGTH = 1024 * 8;

char buffer[BUFFERLENGTH];

int blockchainLength = 0;
int peerBlockchainLength = 0;

#if defined(_MSC_VER)
SOCKADDR_IN otherAddr;
#endif
std::string otherAddrStr;
int otherSize;
std::atomic_bool stop_thread_1 = false;
std::atomic_bool stop_thread_2 = false;
std::atomic_bool thread_running = false;

std::vector<std::string> peerList;
//P2P p2p;

#if defined(_MSC_VER)
// Get the IP:Port combination from SOCKADDR_IN struct, and return it as a string
std::string P2P::NormalizedIPString(SOCKADDR_IN addr) {
	char peerIP[16];
	ZeroMemory(peerIP, 16);
	inet_ntop(AF_INET, &addr.sin_addr, peerIP, 16);

	USHORT port = ntohs(addr.sin_port);

	int realLen = 0;
	for (int i = 0; i < 16; i++) {
		if (peerIP[i] == '\00') {
			break;
		}
		realLen++;
	}

	std::string res(peerIP, realLen);
	res += ":" + std::to_string(port);

	return res;
}
#endif

// Safely send some data as a string, and split large amounts of data into multiple segments to be sent sequentially.
int P2P::mySendTo(int socket, std::string& s, int len, int redundantFlags, sockaddr* to, int toLen)
{
#if defined(_MSC_VER)
	try
	{

		int total = 0;        // how many bytes we've sent
		int bytesLeft = len; // how many we have left to send
		SSIZE_T n = 0;

		const char* p = s.c_str();

		int segmentCount = 1;
		while (total < len) {
			//std::string segInfo = "seg " + + " of " + + ", " + + " bytes|";
			std::string segInfo = "seg :" + std::to_string(segmentCount) +
				": of :" + std::to_string((int)ceil((float)len / 1000.0f)) +
				": , :" + std::to_string((bytesLeft < 1000) ? bytesLeft : 1000) +
				": bytes|||";

			int segSize = segInfo.size();

			segInfo += (p + total);

			n = sendto(socket,
				segInfo.c_str(),
				(bytesLeft < 1000) ? (bytesLeft + segSize) : (1000 + segSize),
				0,
				to,
				toLen)
				- segSize; // Don't include segment info when counting data, so subtract this
			if (n <= -1) { break; }
			total += n;
			if (constants::debugPrint) {
				std::cout << std::to_string((int)round(100 * ((float)total / (float)len))) << "% sent" << std::endl;
			}
			if (bytesLeft < 1000)
				bytesLeft -= n;
			else
				bytesLeft -= 1000;

			segmentCount++;
		}
		if (constants::debugPrint) {
			std::cout << "Done." << std::endl;
		}

		len = total; // return number actually sent here
		return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

#else
	return 0;
#endif
}

// The function that is run in a thread in order to listen for received data in the background
void P2P::ListenerThread(int update_interval)
{
#if defined(_MSC_VER)
	try
	{
		thread_running = true;
		while (true) {
			Console console;

			SOCKADDR_IN remoteAddr;
			int remoteAddrLen = sizeof(remoteAddr);

			DWORD timeout = 10 * 1000;
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
				if (false) {
					console.NetworkErrorPrint();
					console.WriteLine("Error: Socket Error, trying again...");
				}
				else {
					int iResult = recvfrom(localSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);

					if (constants::debugPrint)
						std::cout << "iResult: " << std::to_string(iResult) << std::endl;

					if (iResult > 0) {

						// Get the IPV4 address:port pair of the received data. If it
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

						if (constants::debugPrint)
							console.WriteLine("received -- " + segInfo, console.yellowFGColor, "");

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
							if (constants::debugPrint) {
								console.DebugPrint();
								console.WriteLine("Received initial connection, awaiting confirmation...", console.greenFGColor, "");
							}
							messageStatus = await_first_success; // Awaiting confirmation status
							messageAttempt = 0;
							differentPeerAttempts = 0;

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
							console.NetworkPrint();
							console.WriteLine("Peer closed.");
							CONNECTED_TO_PEER = false;
							reqDat = -1;
							messageStatus = -1;
							return;
						}
						// If the peer is requesting message received confirmation
						else if (totalMessage == "peer$$$success" && (messageStatus >= 0)) {
							if (constants::debugPrint) {
								console.DebugPrint();
								console.WriteLine("Dual Confirmation", console.greenFGColor, "");
							}
							//messageStatus = await_second_success; // Confirmed message status, continue sending our own 
							messageStatus = idle; // Confirmed message status, continue sending our own 
							// confirm 2 times, then switch to idle state -1
							CONNECTED_TO_PEER = true;
						}
						// If the peer is idling
						else if (totalMessage == "peer$$$idle") {
							if (constants::debugPrint) {
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

							if (constants::debugPrint) {
								console.WriteLine("request " + std::to_string(messageStatus), console.greenFGColor, "");
							}
						}
						// If peer is answering request
						else if (SplitString(totalMessage, "$$$")[0] == "answer") {
							// If peer is giving blockchain height
							if (SplitString(totalMessage, "$$$")[1] == "height") {
								peerBlockchainLength = std::stoi(SplitString(totalMessage, "$$$")[2]);
								messageStatus = await_first_success;
								if (constants::debugPrint) {
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
							}
							// If peer is giving a block's data
							else if (SplitString(totalMessage, "$$$")[1] == "block") {
								messageStatus = await_first_success;
								int num = std::stoi(SplitString(totalMessage, "$$$")[2]);
								std::string blockData = SplitString(totalMessage, "$$$")[3];

								// Make sure this data is actually being requested; we don't want a forced download.
								if (reqDat != num)
									continue;

								// Save block data to file
								try
								{
									if (constants::debugPrint)
										console.WriteLine("\nSaved block: " + std::to_string(num));
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

								if (constants::debugPrint) {
									console.WriteLine("received block: " + std::to_string(num), console.greenFGColor, "");
								}
							}
							messageAttempt = 0;

						}
						if (constants::debugPrint) {
							console.WriteLine("received: " + NormalizedIPString(remoteAddr) + " -> " + totalMessage + "\t status: " + std::to_string(messageStatus));
						}
					}
					else if (WSAGetLastError() != WSAETIMEDOUT && constants::debugPrint) {
						console.NetworkErrorPrint();
						console.WriteLine("Error, Peer closed.");
						CONNECTED_TO_PEER = false;
						reqDat = -1;
						//thread_running = false;
						return;
					}
				}
			}
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
#endif
}

void P2P::InitPeerList() {
	std::string line;
	std::ifstream peerFile("./wwwdata/peerlist.list");
	// If the peer list file does not exist, create it
	if (!peerFile)
	{
		Console console;
		console.ErrorPrint();
		console.WriteLine("Error opening peer file", console.redFGColor, "");

		// Create the peer list file
		std::ofstream peerFileW("./wwwdata/peerlist.list");
		if (peerFileW.is_open())
		{
			peerFileW << "";
			peerFileW.close();
		}
		peerFileW.close();
	}
	else
		while (std::getline(peerFile, line))
			peerList.push_back(line);
	peerFile.close();
}


// The function to open the socket required for the P2P connection
int P2P::OpenP2PSocket(int port)
{
#if defined(_MSC_VER)

	Console console;
	Http http;

	console.NetworkPrint();
	console.WriteLine("Starting P2P Client...");

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
		console.WriteLine("\n!!! Failed to bind socket !!!\n");
		closesocket(localSocket);
		WSACleanup();
		return 0;
	}

	int val = 2048;
	setsockopt(localSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));
	setsockopt(localSocket, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val));

#endif

	return 0;
}

// Function to get random peer credentials from the peerList
void P2P::RandomizePeer() {
	try
	{

		uint16_t randI = rand() % peerList.size();
		peerIP = SplitString(peerList[randI], ":")[0];
		peerPort = stoi(SplitString(peerList[randI], ":")[1]);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

// The function that is run in a thread in order to reply or send data to a peer in the background
void P2P::SenderThread()
{
#if defined(_MSC_VER)

	Console console;
	Http http;

	stop_thread_2 = false;

	RandomizePeer();

	while (true) {
		try
		{

			otherAddr.sin_port = htons(peerPort);
			otherAddr.sin_family = AF_INET;
			otherAddr.sin_addr.s_addr = inet_addr(peerIP.c_str());

			otherSize = sizeof(otherAddr);

			bool noinput = false;


			// If not replying to messages or requesting something, continue
			if (messageStatus == idle)
				continue;

			int messageMaxAttempts = 10;

			// Begin sending messages, and stop when a reply is received, or max tries exceeded
			for (messageAttempt = 0; messageAttempt < messageMaxAttempts; messageAttempt++)
			{
				// Stop sending if the message status switches to idle
				if (messageStatus == idle)
				{
					if (constants::debugPrint)
						console.WriteLine("Send/receive complete", console.greenFGColor, "");
					else
						console.WriteLine();
					reqDat = -1;
					role = -1;
					break;
				}

				// If not in idle state, continue sending messages
				std::string msg = "";

				std::cout << "\r\r";
				console.NetworkPrint();
				console.Write("Send attempt: " + std::to_string(messageAttempt) + "    ");

				// If doing initial connect request
				if (messageStatus == initial_connect_request) {
					msg = "peer$$$connect";
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// If doing disconnect request
				else if (messageStatus == disconnect_request) {
					msg = "peer$$$disconnect";
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// If doing peer confirmation
				else if ((messageStatus == initial_connect_request || messageStatus == await_first_success || messageStatus == await_second_success)) {
					msg = "peer$$$success";
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);

					// After multiple confirmations have been sent, switch back to idle mode
					if (messageAttempt >= 2) {
						messageStatus = idle;
						messageAttempt = 0;
						continue;
					}
				}
				// Else if replying to height request
				else if (messageStatus == replying_height) {
					role = 1;
					msg = "answer$$$height$$$" + std::to_string(blockchainLength);
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// Else if replying to pending block data request
				else if (messageStatus == replying_pendingblock) {
					role = 1;
					// Open and read requested block
					std::ifstream td("./wwwdata/pendingblocks/block" + std::to_string(reqDat) + ".dccblock");
					std::stringstream bufferd;
					bufferd << td.rdbuf();
					std::string blockText = bufferd.str();

					msg = "answer$$$pendingblock$$$" + std::to_string(reqDat) + "$$$" + ReplaceEscapeSymbols(blockText);
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// Else if replying to block data request
				else if (messageStatus == replying_block) {
					role = 1;
					// Open and read requested block
					std::ifstream td("./wwwdata/blockchain/block" + std::to_string(reqDat) + ".dccblock");
					std::stringstream bufferd;
					bufferd << td.rdbuf();
					std::string blockText = bufferd.str();

					msg = "answer$$$block$$$" + std::to_string(reqDat) + "$$$" + ReplaceEscapeSymbols(blockText);
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// Else if replying to peer list request
				else if (messageStatus == replying_peer_list) {
					role = 1;
					std::string totalPeersString = "";
					for (int i = 0; i < peerList.size() && i < 10; i++)
						totalPeersString += peerList[i] + ((i == peerList.size() - 1 || i == 9) ? "" : ",");

					msg = "answer$$$peerlist$$$" + totalPeersString;
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
				}
				// Else if requesting chain height
				else if (messageStatus == requesting_height) {
					msg = "request$$$height";
					role = 0;
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					//// Wait extra 3 seconds
				}
				// Else if requesting pending block data
				else if (messageStatus == requesting_pendingblock) {
					msg = "request$$$pendingblock$$$" + std::to_string(reqDat);
					role = 0;
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					// Wait extra 3 seconds
					//noinput = true;
				}
				// Else if requesting block data
				else if (messageStatus == requesting_block) {
					msg = "request$$$block$$$" + std::to_string(reqDat);
					role = 0;
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					// Wait extra 3 seconds
					//noinput = true;
				}
				// Else if requesting peer list
				else if (messageStatus == requesting_peer_list) {
					msg = "request$$$peerlist";
					role = 0;
					if (constants::debugPrint) {
						console.Write(msg + "\n");
					}
					mySendTo(localSocket, msg, msg.length(), 0, (sockaddr*)&otherAddr, otherSize);
					//// Wait extra 3 seconds
					//noinput = true;
				}

				// Wait 50 milliseconds before sending next message
				Sleep(50);
			}

			if (messageAttempt == messageMaxAttempts) {
				console.NetworkErrorPrint();
				console.Write("Peer Timed Out at ", console.redFGColor, "");
				console.WriteLine(std::to_string(peerPort), console.cyanFGColor, "");
				messageAttempt = 0;

				// If this client is the asker, then try other peers.
				if (role == 0) {

					// Try at least 5 different peers to get answer to request
					if (differentPeerAttempts < 4) {
						console.NetworkPrint();
						console.WriteLine("Finding another peer...");
						RandomizePeer();
						differentPeerAttempts++;
					}
					// If at least 5 have been tried, stop.
					else {
						messageStatus = idle;
						console.NetworkErrorPrint();
						console.WriteLine("!! No peers seem to be online. Please try again later. !!", console.redFGColor, "");
						differentPeerAttempts = 0;
						reqDat = -1;
					}
				}
				// Otherwise, this is the answerer, so stop trying to reply to the original asker.
				else {
					role = -1;
					messageStatus = idle;
					console.NetworkErrorPrint();
					console.WriteLine("Asking peer went offline.", console.redFGColor, "");
					differentPeerAttempts = 0;
					reqDat = -1;
				}
			}
		}
		catch (const std::exception& e)
		{
			console.ErrorPrint();
			console.WriteLine(e.what());
		}
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

#endif
}
