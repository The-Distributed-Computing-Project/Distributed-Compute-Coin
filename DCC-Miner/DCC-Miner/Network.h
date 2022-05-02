#ifndef network_h
#define network_h

#include <cpr/cpr.h>
#include "Console.h"

class Http
{
	Console console;
public:
	std::string blockVersion = "";
	//string StartHttpWebRequest(string URL, vector<string> args_vals);
	std::string StartHttpWebRequest(std::string URL, std::vector<std::string> args_vals)
	{
		std::string html = "";

		std::string url = URL;
		for (int i = 0; i < args_vals.size(); i++)
		{
			if (i > 0)
				url += "&";
			url += args_vals.at(i);
		}
		if (blockVersion != "")
			url += "&Version=" + blockVersion;

		auto response = cpr::Get(cpr::Url{ url });
		html = response.text;

		console.WriteLine(html, console.Debug());

		return html;
	}
};

int DownloadFile(std::string url, std::string saveAs);
int DownloadFile(std::string url, std::string saveAs, bool printStatus);
std::string UploadFile(std::string url, std::string filePath);
//string GetPeerAddress();

#endif
