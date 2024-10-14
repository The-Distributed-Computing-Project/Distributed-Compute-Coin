

#include "Network.h"

//using namespace std;

//std::string Http::StartHttpWebRequest(std::string URL, vector<std::string> args_vals)
//{
//	std::string html = "";
//
//	std::string url = URL;
//	for (int i = 0; i < args_vals.size(); i++)
//	{
//		if (i > 0)
//			url += "&";
//		url += args_vals.at(i);
//	}
//	if (blockVersion != "")
//		url += "&Version=" + blockVersion;
//
//	auto response = cpr::Get(cpr::Url{ url });
//	html = response.text;
//
//	Console().WriteLine(html, Console().Debug());
//
//	return html;
//}

bool TestPortConnection(std::string ip, int port) {

#if WINDOWS

	//creates a socket on your machine and connects to the port of the IP address specified
	SOCKADDR_IN connAddr;
	int connAddrLen = sizeof(connAddr);
	SOCKET myNetworkSocket;
	
	connAddr.sin_family = AF_INET;
	connAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	connAddr.sin_port = htons(port);

	myNetworkSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*DWORD timeout = 2 * 1000;
	setsockopt(myNetworkSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));*/

	if (myNetworkSocket == INVALID_SOCKET) {
		std::cout << WSAGetLastError() << std::endl;
		return false;
	}

	u_long mode = 1;
	int iResult = ioctlsocket(myNetworkSocket, FIONBIO, &mode);

	if (iResult == SOCKET_ERROR) {
		std::cout << WSAGetLastError() << std::endl;
		return false;
	}
	else
	{

		iResult = connect(myNetworkSocket, (SOCKADDR*)&connAddr, sizeof(connAddr));
		if (iResult != WSAEWOULDBLOCK)
		{
			const int BUFFERLENGTH = 1024 * 8;
			char buffer[BUFFERLENGTH];

			const char* testBuf = "ping";
			sendto(myNetworkSocket, testBuf, 5, 0, (sockaddr*)&connAddr, connAddrLen);
			Sleep(500);
			iResult = recvfrom(myNetworkSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&connAddr, &connAddrLen);
			//if (connectionResponse == 1) {
			if (iResult > 0) {
				// Read the received data buffer into a string
				std::string textVal = std::string(buffer, buffer + iResult);
				return true;
				// If the returned data contains the segment header, it must be the DCC client port
				try {
					std::string segInfo = SplitString(textVal, "&")[0];
					int segNumber = std::stoi(SplitString(segInfo, ":")[1]);
					int maxSegments = std::stoi(SplitString(segInfo, ":")[3]);
					//std::string content = SplitString(textVal, "&")[1];

					closesocket(myNetworkSocket);
					return true;
				}
				catch (...) {
					closesocket(myNetworkSocket);
					return false;
				}
			}
			else
			{
				std::cout << "\tnodata\n";
			}
		}
	}
	
	/*if (bind(myNetworkSocket, (LPSOCKADDR)&connAddr, sizeof(connAddr)) == SOCKET_ERROR)
	{
		console::ErrorPrint();
		console::WriteLine("\n!!! Failed to bind socket !!!\n");
		closesocket(myNetworkSocket);
		return 0;
	}*/

	closesocket(myNetworkSocket);
#endif
	return false;
}

int ScanAllPorts(std::string hostNameArg) {
	int port = 0;

	while(port < 65535){
		//test connection
		if (TestPortConnection(hostNameArg, port)){
			std::cout << "Port " << port << " is open!" << std::endl;
			return port;
		}
		else {
			std::cout << "Port " << port << " is closed." << std::endl;
		}
		port++;
	}
	std::cout << "No ports open!\n";
	return 0;
}

int DownloadFile(std::string url, std::string saveAs, bool printStatus)
{
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		//console::NetworkPrint();
		console::Write(" (Downloading from: \"" + url + "\") ");
		
		fp = fopen(saveAs.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 120);
		res = curl_easy_perform(curl);
		fclose(fp);
		if (res != CURLE_OK) {
			//fprintf(stderr, "\ncurl_easy_perform() failed: %s\n",
			//	curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			throw curl_easy_strerror(res);

		}
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
	return 0;
}

size_t general_write_data(void* ptr, size_t size, size_t nmemb, void* str) {
	std::string* s = static_cast<std::string*>(str);
	std::copy((char*)ptr, (char*)ptr + (size + nmemb), std::back_inserter(*s));
	return size * nmemb;
}

std::string DownloadFileAsString(std::string url, bool printStatus)
{
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	std::string result = "";
	if (curl)
	{
		console::Write(" (Downloading from: \"" + url + "\") ");
		
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, general_write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			//fprintf(stderr, "\ncurl_easy_perform() failed: %s\n",
			//	curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			curl_global_cleanup();
			throw curl_easy_strerror(res);
		}
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
	result.pop_back();
	return result;
}

#if WINDOWS
std::string UploadFile(std::string url, std::string filePath)
{
	cpr::Response r = cpr::Post(cpr::Url{ url },
		cpr::Multipart{ {"file", cpr::File{filePath}} });
	//std::cout << r.text << std::endl;

	return r.text;

	//CURL* curl;
	//CURLcode res;

	//curl_httppost* post = NULL;
	//curl_httppost* last = NULL;
	///*HttpPost* post = NULL;
	//HttpPost* last = NULL;*/

	//curl = curl_easy_init();
	//if (curl)
	//{
	//	curl_formadd(&post, &last,
	//		CURLFORM_COPYNAME, "filename",
	//		CURLFORM_COPYCONTENTS, "programOutData",
	//		CURLFORM_END);
	//	curl_formadd(&post, &last,
	//		CURLFORM_COPYNAME, "file",
	//		CURLFORM_FILE, filePath,
	//		CURLFORM_END);

	//	curl_easy_setopt(curl, CURLOPT_URL, url);
	//	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

	//	res = curl_easy_perform(curl);
	//	if (res)
	//	{
	//		return 0;
	//	}

	//	curl_formfree(post);
	//}
	//else
	//{
	//	return 0;
	//}

	//curl_easy_cleanup(curl);
}
#endif
//string GetPeerAddress()
//{
//
//}
