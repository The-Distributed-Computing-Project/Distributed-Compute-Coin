

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

	//creates a socket on your machine and connects to the port of the IP address specified
	struct sockaddr_in address;
	int myNetworkSocket = -1;
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip.c_str());
	address.sin_port = htons(port);
	
	myNetworkSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (myNetworkSocket==-1) {
		std::cout << "Socket creation failed on port " << port << std::endl;
		return false;
	}
	
	fcntl(myNetworkSocket, F_SETFL, O_NONBLOCK);
	
	connect(myNetworkSocket, (struct sockaddr *)&address, sizeof(address)); 
	
	//creates a file descriptor set and timeout interval
	fd_set fileDescriptorSet;
	struct timeval timeout;
	
	FD_ZERO(&fileDescriptorSet);
	FD_SET(myNetworkSocket, &fileDescriptorSet);
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	
	//int connectionResponse = select(myNetworkSocket + 1, NULL, &fileDescriptorSet, NULL, &timeout);
	
	const int BUFFERLENGTH = 1024 * 8;
	char buffer[BUFFERLENGTH];
	
	int iResult = recvfrom(myNetworkSocket, buffer, BUFFERLENGTH, 0, (sockaddr*)&address, &address);
	//if (connectionResponse == 1) {
	if (iResult > 0){
		// Read the received data buffer into a string
		std::string textVal = std::string(buffer, buffer + iResult);
		
		// If the returned data contains the segment header, it must be the DCC client port
		try{
			std::string segInfo = SplitString(textVal, "\376")[0];
			int segNumber = std::stoi(SplitString(segInfo, ":")[1]);
			int maxSegments = std::stoi(SplitString(segInfo, ":")[3]);
			std::string content = SplitString(textVal, "\376")[1];
				
			close(myNetworkSocket);
			return true;
		}
		catch(...){
			close(myNetworkSocket);
			return false;
		}
	}
	close(myNetworkSocket);
	return false;
}

int ScanAllPorts(std::string hostNameArg) {
	int port;

	for(port < 65535; port++;){
		//test connection
		if (TestPortConnection(hostNameArg, port)){
			std::cout << "Port " << port << " is open!" << std::endl;
			return port;
		}
		else {
			std::cout << "Port " << port << " is closed." << std::endl;
		}
	}
}

int DownloadFile(std::string url, std::string saveAs, bool printStatus)
{
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		console::NetworkPrint();
		console::WriteLine("Downloading from: \"" + url + "\"");
		
		fp = fopen(saveAs.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		res = curl_easy_perform(curl);
		fclose(fp);
		if (res != CURLE_OK) {
			fprintf(stderr, "\ncurl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}
	return 0;
}

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

//string GetPeerAddress()
//{
//
//}
