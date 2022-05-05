
#include <iostream>
#include "json.hpp"
#include <vector>
#include <filesystem>
#include <fstream>
#include <stdio.h>
#include "RequestParser.h"
#include "strops.h"

string BLOCKCHAIN_DIR = "./wwwdata/blockchain";
string PENDING_BLOCKCHAIN_DIR = "./wwwdata/pendingblocks";

std::string RequestParser::Parse(std::string url)
{
	// Create easily accessible map of all name/value pairs.
	// 	ex. {"query", "buy"}
	std::map<std::string, std::string> argValMap;
	std::vector<std::string> args = SplitString(url, "&");
	for (int i = 0; i < args.size(); i++)
	{
		std::vector<std::string> argv = SplitString(args[i], "=");
		argValMap[argv[0]] = argv[1];
	}
	
	
}
