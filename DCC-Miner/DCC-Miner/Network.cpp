
#include <iostream>
#include <string>
#include <vector>
#include "Console.h"
//#include "P2PClient.cpp"
#include <Urlmon.h>
#include <curl/curl.h>

#include "Network.h"
#include "Console.h"

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

int DownloadFile(std::string url, std::string saveAs)
{
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		fp = fopen(saveAs.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	return 0;
}
int DownloadFile(std::string url, std::string saveAs, bool printStatus)
{
	Console console;
	
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		if (printStatus)
		{
			console.Network();
			console.Write("Downloading from: \"" + url + "\" ...\r");
		}
		fp = fopen(saveAs.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
		if (printStatus)
		{
			console.Network();
			console.Write("Downloading from: \"" + url + "\" Done.\r");
		}
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
