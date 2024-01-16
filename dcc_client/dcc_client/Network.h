#ifndef network_h
#define network_h

#include <cpr/cpr.h>
#include <iostream>
#include <string>
#include <vector>
#include "Console.h"
//#include "P2PClient.cpp"
//#include <Urlmon.h>
#include "SettingsConsts.h"
#include <curl/curl.h>
#include "Console.h"

class Http
{
public:
	//string StartHttpWebRequest(string URL, vector<string> args_vals);
	std::string StartHttpWebRequest(std::string URL, std::vector<std::string> args_vals)
	{
		std::string html = "";

		std::string url = URL;
		if (args_vals.size() >= 1)
			url += "?";
		for (int i = 0; i < args_vals.size(); i++)
		{
			if (i > 0)
				url += "&";
			url += args_vals.at(i);
		}
		if (WalletSettingValues::verbose >= 2) {
			console::NetworkPrint();
			console::WriteIndented("Accessing url: \"" + url + "\"\n", "", "", 1);
		}

		auto response = cpr::Get(cpr::Url{ url });
		html = response.text;

		//console::DebugPrint();
		//console::WriteLine(html);

		return html;
	}
};

int DownloadFile(std::string url, std::string saveAs, bool printStatus = false);
std::string UploadFile(std::string url, std::string filePath);
//string GetPeerAddress();


#endif
