
#include "Miner.h"

#if defined(__unix__)
#include <curses.h>
#include <stdio.h>
#define ISKEYDOWN(X) (getch() == X)

#else

#define ISKEYDOWN(X) GetAsyncKeyState(X)

#endif

// Mine a single block with specified data and using the difficulty stored in walletInfo["MineDifficulty"]
int Mine(json currentBlockJson, int blockNum, json& walletInfo)
{
	//walletInfo["targetDifficulty"] = "0000000FFFFFF000000000000000000000000000000000000000000000000000";
	console::MiningPrint();
	console::Write("Mining ");
	console::Write("block " + std::to_string(blockNum), console::cyanFGColor, "");
	if (WalletSettingValues::verbose >= 2) {
		console::Write(" at difficulty ");
		console::Write((std::string)walletInfo["targetDifficulty"], console::cyanFGColor, "");
	}
	console::Write(" :\n");

	std::string seedStr = std::to_string(rand());
	//stdlib_rand_numeric_string(seedStr, 16);
	try
	{
		auto startTime = std::chrono::steady_clock::now();

		std::string runCommand = "podman run -d --network none --rm --name=" + (std::string)(walletInfo["ProgramID"]) + " -v ./wwwdata/programs/" + (std::string)(walletInfo["ProgramID"]) + ":/out/ " + (std::string)(walletInfo["ProgramID"]) + " /bin/bash run.sh --seed " + seedStr;

		if (WalletSettingValues::verbose >= 2) {
			console::ContainerManagerPrint();
			console::Write("Starting program with command: \"");
			console::Write(runCommand);
			console::WriteLine("\"");
		}
		ExecuteAsync(runCommand, false);
		boost::process::child containerProcess = ExecuteAsync("podman wait " + (std::string)(walletInfo["ProgramID"]), false);

		char sha256OutBuffer[65];

		// Get hash from previous block
		std::ifstream blockFile("./wwwdata/blockchain/block" + std::to_string((int)walletInfo["BlockchainLength"]) + ".dccblock");
		std::stringstream blockBuffer;
		blockBuffer << blockFile.rdbuf();
		currentBlockJson["pprev"] = (std::string)(json::parse(blockBuffer.str())["hash"]);

		// Print controls
		console::Write("\nPress ");
		console::Write("P", console::greenFGColor, "");
		console::Write(" to pause, ");
		console::Write("Q", console::redFGColor, "");
		console::Write(" to quit\n");

		//Checks Hash
		unsigned long long int nonce = 0;
		unsigned char hash[32];
		std::string dif = (std::string)(walletInfo["targetDifficulty"]);
		unsigned char* c_difficulty = (unsigned char*)hexstr_to_cstr(dif);

		uint8_t difficultyLen = 65;
		auto hashStart = std::chrono::steady_clock::now();
		unsigned long long hashesPerSecond = 0;
		unsigned long long hashesAtStart = 0;

		// The data we will actually be mining for is a hash of the
		// transactions and header, so we don't need to do calculations on
		// massive amounts of data
		std::string txData; // Only use the `tx` portion of each transaction objects' data
		for (size_t i = 0; i < currentBlockJson["transactions"].size(); i++)
		{
			txData += (std::string)(currentBlockJson["transactions"][i]["tx"].dump());
		}
		std::string fDat = (std::string)currentBlockJson["pprev"] + txData;
		sha256_string((char*)(fDat.c_str()), sha256OutBuffer);
		std::string hData = std::string(sha256OutBuffer);
		char* hDataChars = (char*)hData.c_str();

		char numberstring[] = { '-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'-' ,'0' ,'0' ,'0' ,'0' ,'0', '\0' };
		char databuffer[83];
		strncpy(databuffer, hDataChars, sizeof(databuffer));
		//strncat(databuffer, numberstring, 18);
		// While hash is not less than the target difficulty number
		do
		{
			nonce++;
			addOneToHexStr(numberstring, 17);
			strncpy(databuffer, hDataChars, 82);
			strncat(databuffer, numberstring, sizeof(numberstring) / sizeof(char));
			//addOneToHexStrInRange(databuffer, 65, 17);

			// Hash data
			sha256_full_cstr(databuffer, hash);

			// print status every second
			if ((since(hashStart).count()) >= 1000)
			{
				hashesPerSecond = nonce - hashesAtStart;
				hashStart = std::chrono::steady_clock::now();
				hashesAtStart = nonce;
				//std::cout<<std::endl << databuffer << std::endl;
				cstr_to_hexstr(hash, 32, sha256OutBuffer);
				console::Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + numberstring + " # " + std::string(sha256OutBuffer));
				console::Write("   " + FormatHPS((float)hashesPerSecond) + " ");

				[[unlikely]]
				if (ISKEYDOWN(0x51)) // Q Key for quit loop
					//if (kbhit())
					//	if (GETKEY() == 'q')
							return 2;
				[[unlikely]]
				if (ISKEYDOWN(0x50)) // P Key for pause loop
					//if (kbhit())
					//	if (GETKEY() == 'p')
							return 3;
			}
		} while (!CompareCharNumbers(c_difficulty, hash));

		// Print a final time so the miner can see the result
		cstr_to_hexstr(hash, 32, sha256OutBuffer);
		console::Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + numberstring + " # ");
		console::Write(std::string(sha256OutBuffer), console::greenFGColor, "");

		std::cout << std::endl;

		//// Wait for the Docker program to finish running
		//if (containerProcess.running())
		//	containerProcess.wait();

		// Kill the program if it is still running
		ExecuteAsync("podman stop " + (std::string)(walletInfo["ProgramID"]), false);


		// Convert hash into hexadecimal string
		sha256_string((char*)(hData + numberstring).c_str(), sha256OutBuffer);
		std::string hashStr = std::string(sha256OutBuffer);

		currentBlockJson["hash"] = hashStr;
		currentBlockJson["_version"] = BLOCK_VERSION;
		currentBlockJson["targetDifficulty"] = (std::string)walletInfo["targetDifficulty"];
		currentBlockJson["nonce"] = (std::string)numberstring;
		// Get current unix time in seconds
		uint64_t sec = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		currentBlockJson["time"] = sec;

		// Save new json data to file into finished blockchain folder
		try
		{
			std::ofstream blockFilew("./wwwdata/blockchain/block" + std::to_string(blockNum) + ".dccblock");
			if (blockFilew.is_open())
			{
				blockFilew << currentBlockJson.dump();
				blockFilew.close();
			}
		}
		catch (const std::exception& e)
		{
			if (WalletSettingValues::verbose >= 1)
				std::cerr << e.what() << std::endl;
			return 0;
		}

		// Remove the block from pending
		fs::remove("./wwwdata/pendingblocks/block" + std::to_string(blockNum) + ".dccblock");

		// Create new pending block if there are no more
		if (FileCount("./wwwdata/pendingblocks/") == 0) {
			json blockJson = json();

			blockJson["hash"] = "0000000000000000000000000000000000000000000000000000000000000000";
			blockJson["pprev"] = hashStr;
			blockJson["nonce"] = "";
			blockJson["time"] = "";
			blockJson["targetDifficulty"] = "";
			blockJson["_version"] = BLOCK_VERSION;
			blockJson["transactions"] = json::array();
			blockJson["containerTasks"] = json::array();
			blockJson["id"] = blockNum + 1;

			// Save new json data to file into finished blockchain folder
			try
			{
				std::ofstream blockFilew("./wwwdata/pendingblocks/block" + std::to_string(blockNum + 1) + ".dccblock");
				if (blockFilew.is_open())
				{
					blockFilew << blockJson.dump();
					blockFilew.close();
				}
			}
			catch (const std::exception& e)
			{
				if (WalletSettingValues::verbose >= 1)
					std::cerr << e.what() << std::endl;
				return 0;
			}
		}


		console::MiningPrint();
		console::WriteLine("Mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");

		return 1;
	}
	catch (const std::exception& e)
	{
		//if (WalletSettingValues::verbose == true)
		std::cerr << e.what() << std::endl;
		return 0;
	}
}

// Mine blocks for a pool
int PoolMine(std::string poolURL, json& walletInfo)
{
	console::NetworkPrint();
	console::Write("Use pool ");
	console::WriteLine(poolURL, console::brightCyanFGColor, "");
	unsigned int blockNumber = 0;
	std::string difficulty = "";
	std::string hData = "";
	unsigned long long int nonce = 0;
	unsigned long long int maxNonce = 0;
	while (true)
	{
		// Request the data to mine from the pool, and receive the hash along with a nonce range.
		try
		{
			Http http;
			std::vector<std::string> args = { "query=requestData", "lastNonce=" + std::to_string(maxNonce) };
			std::string html = http.StartHttpWebRequest(poolURL, args);
			std::cout << "\"" << html << "\"" << std::endl;

			if (html.find("ERR") != std::string::npos || html == "")
				throw 0;
			json poolData = json::parse(html);
			blockNumber = (unsigned int)poolData["blockNumber"];
			difficulty = (std::string)poolData["difficulty"];
			hData = (std::string)poolData["hData"];
			nonce = (unsigned long long int)poolData["startNonce"];
			maxNonce = (unsigned long long int)poolData["endNonce"];

			console::NetworkPrint();
			console::Write("new job ", console::magentaFGColor, "");
			console::WriteLine("from " + poolURL + " nonces " + std::to_string(maxNonce - nonce) + " height " + std::to_string(blockNumber));
		}
		catch (const std::exception& e)
		{
			console::NetworkErrorPrint();
			console::WriteLine("Connection error.");
			std::cerr << e.what() << std::endl;
			return 0;
		}

		//walletInfo["targetDifficulty"] = "0000000FFFFFF000000000000000000000000000000000000000000000000000";
		console::MiningPrint();
		console::Write("Mining ");
		console::Write("block " + std::to_string(blockNumber), console::cyanFGColor, "");
		console::Write(" at difficulty ");
		console::Write(difficulty, console::cyanFGColor, "");
		console::Write(" :\n");
		try
		{
			auto startTime = std::chrono::steady_clock::now();

			// The Docker program execution needs to be thought out more, because it would need changes for pool mining.
			//console::ContainerManagerPrint();
			//console::WriteLine("Starting program... ");
			//boost::process::child containerProcess = ExecuteAsync("cargo run --manifest-path ./wwwdata/programs/" + (std::string)(walletInfo["ProgramID"]) + "/Cargo.toml", false);

			char sha256OutBuffer[65];

			unsigned char hash[32];
			unsigned char* c_difficulty = (unsigned char*)hexstr_to_cstr(difficulty);

			uint8_t difficultyLen = 65;
			auto hashStart = std::chrono::steady_clock::now();
			unsigned long long hashesPerSecond = 0;
			unsigned long long hashesAtStart = 0;

			char* hDataChars = (char*)hData.c_str();

			char numberstring[17];
			char databuffer[128];
			strncpy(databuffer, hDataChars, sizeof(databuffer));
			// While hash is not less than the target difficulty number, and nonces are not exhausted
			do
			{
				nonce++;
				sprintf(numberstring, "%x", nonce);
				strncpy(databuffer, hDataChars, 65);
				strncat(databuffer, numberstring, 17);
				sha256_full_cstr(databuffer, hash);
				//std::cout << sizeof(hashesPerSecond) << std::endl;

				if ((since(hashStart).count() / 1000) >= 1)
				{
					hashesPerSecond = nonce - hashesAtStart;
					hashStart = std::chrono::steady_clock::now();
					hashesAtStart = nonce;

					cstr_to_hexstr(hash, 32, sha256OutBuffer);
					console::Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + FormatWithCommas<unsigned long long int>(nonce) + " # " + std::string(sha256OutBuffer));
					console::Write("   " + FormatHPS(hashesPerSecond) + "            ");
					//std::cout << std::endl <<  databuffer << std::endl;
				}
			} while (!CompareCharNumbers(c_difficulty, hash) && nonce <= maxNonce);

			std::cout << std::endl;

			// If the nonce is exhausted and the hash is still wrong, request new job from the server
			if (nonce > maxNonce && !CompareCharNumbers(c_difficulty, hash))
				continue;


			//// Wait for the Docker program to finish running
			//if (containerProcess.running())
			//	containerProcess.wait();


			// Return the nonce to the server if it is correct
			try
			{
				Http http;
				std::vector<std::string> args = { "query=solved", "nonce=" + std::to_string(nonce), "id=" + (std::string)walletInfo["Address"] };
				std::string html = http.StartHttpWebRequest(poolURL, args);

				if (html.find("ERR") != std::string::npos || html == "")
					throw 0;
				//json poolData = json::parse(html);
				console::NetworkPrint();
				console::Write("accepted ", console::greenFGColor, "");
				console::WriteLine("nonces " + (std::string)numberstring);
			}
			catch (const std::exception& e)
			{
				console::NetworkErrorPrint();
				console::WriteLine("Connection error.");
				std::cerr << e.what() << std::endl;
				return 0;
			}


			console::MiningPrint();
			console::WriteLine("Mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");

		}
		catch (const std::exception& e)
		{
			//if (WalletSettingValues::verbose == true)
			std::cerr << e.what() << std::endl;
			return 0;
		}
	}
	return 1;
}


// Calculate the nonce and hash for an existing block at a specific difficulty
int MineAnyBlock(int blockNum, std::string& difficulty)
{
	difficulty = ToLower(difficulty);


	auto startTime = std::chrono::steady_clock::now();

	std::ifstream td("./wwwdata/blockchain/block" + std::to_string(blockNum) + ".dccblock");
	std::stringstream bufferd;
	bufferd << td.rdbuf();
	std::string nextBlockText = bufferd.str();
	json o = json::parse(nextBlockText);

	std::string txData; // Only use the `tx` portion of each transaction objects' data
	for (size_t i = 0; i < o["transactions"].size(); i++)
	{
		txData += (std::string)o["transactions"][i]["tx"].dump();
	}
	std::string currentHash = o["hash"];
	std::string pprev = o["pprev"];

	char sha256OutBuffer[65];

	//Checks Hash
	long nonce = 0;
	//std::string hash = "";
	unsigned char hash[32];
	char* c_difficulty = (char*)difficulty.c_str();
	int difficultyLen = difficulty.length();
	auto hashStart = std::chrono::steady_clock::now();
	int hashesPerSecond = 0;
	int hashesAtStart = 0;

	// The data we will actually be mining for is a hash of the
	// transactions and header, so we don't need to do calculations on
	// massive amounts of data
	std::string fDat = pprev + txData;
	sha256_string((char*)(fDat.c_str()), sha256OutBuffer);
	std::string hData = std::string(sha256OutBuffer);

	//while (!StringStartsWith(hash, difficulty))
	while (!CharStrStartsWith(hash, c_difficulty, difficultyLen))
	{
		if ((since(hashStart).count() / 1000) >= 1)
		{
			hashesPerSecond = nonce - hashesAtStart;
			hashStart = std::chrono::steady_clock::now();
			hashesAtStart = nonce;

			cstr_to_hexstr(hash, 32, sha256OutBuffer);
			console::Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + CommaLargeNumber(nonce) + " # " + std::string(sha256OutBuffer));
			console::Write("   " + FormatHPS((float)hashesPerSecond) + "            ");
		}

		nonce++;
		sha256_full_cstr((char*)(hData + std::to_string(nonce)).c_str(), hash);
	}

	std::cout << std::endl;
	console::MiningPrint();
	console::WriteLine("Debug mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");
	console::MiningPrint();
	cstr_to_hexstr(hash, 32, sha256OutBuffer);
	console::Write("Final value: hash # ");
	console::WriteLine(std::string(sha256OutBuffer), "", console::greenFGColor);
	console::MiningPrint();
	console::Write("Final value: nonce # ");
	console::WriteLine(std::to_string(nonce), "", console::greenFGColor);

	return 0;
}


