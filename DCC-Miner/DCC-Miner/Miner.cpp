
#include "Miner.h"

Console cons_miner = Console();


// Mine a single block with specified data and using the difficulty stored in walletInfo["MineDifficulty"]
int Mine(json currentBlockJson, int blockNum, json& walletInfo)
{
	//walletInfo["targetDifficulty"] = "0000000FFFFFF000000000000000000000000000000000000000000000000000";
	cons_miner.MiningPrint();
	cons_miner.Write("Mining ");
	cons_miner.Write("block " + std::to_string(blockNum), cons_miner.cyanFGColor, "");
	cons_miner.Write(" at difficulty ");
	cons_miner.Write((std::string)walletInfo["targetDifficulty"], cons_miner.cyanFGColor, "");
	cons_miner.Write(" :\n");
	try
	{
		auto startTime = std::chrono::steady_clock::now();

		cons_miner.RustPrint();
		cons_miner.WriteLine("Starting program... ");
		boost::process::child containerProcess = ExecuteAsync("docker run --network none --rm --name="+(std::string)(walletInfo["ProgramID"])+" -v ./wwwdata/programs/"+(std::string)(walletInfo["ProgramID"])+":/out/ "+(std::string)(walletInfo["ProgramID"])+" /bin/bash run.sh", false);

		char sha256OutBuffer[65];

		// Get hash from previous block
		std::ifstream blockFile("./wwwdata/blockchain/block" + std::to_string((int)walletInfo["BlockchainLength"]) + ".dccblock");
		std::stringstream blockBuffer;
		blockBuffer << blockFile.rdbuf();
		currentBlockJson["lastHash"] = (std::string)(json::parse(blockBuffer.str())["hash"]);

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
		std::string fDat = (std::string)currentBlockJson["lastHash"] + txData;
		sha256_string((char*)(fDat.c_str()), sha256OutBuffer);
		std::string hData = std::string(sha256OutBuffer);
		char* hDataChars = (char*)hData.c_str();

		char numberstring[17];
		char databuffer[128];
		strncpy(databuffer, hDataChars, sizeof(databuffer));
		// While hash is not less than the target difficulty number
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
				cons_miner.Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + FormatWithCommas<unsigned long long int>(nonce) + " # " + std::string(sha256OutBuffer));
				cons_miner.Write("   " + FormatHPS(hashesPerSecond) + "            ");
				//std::cout << std::endl <<  databuffer << std::endl;
			}
		} while (!CompareCharNumbers(c_difficulty, hash));

		std::cout << std::endl;

		// Wait for the rust program to finish running
		if (containerProcess.running())
			containerProcess.wait();


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
			if (constants::debugPrint == true)
				std::cerr << e.what() << std::endl;
			return 0;
		}

		// Remove the block from pending
		fs::remove("./wwwdata/pendingblocks/block" + std::to_string(blockNum) + ".dccblock");

		// Create new pending block if there are no more
		if (FileCount("./wwwdata/pendingblocks/") == 0) {
			json blockJson = json();

			blockJson["hash"] = "0000000000000000000000000000000000000000000000000000000000000000";
			blockJson["lastHash"] = hashStr;
			blockJson["nonce"] = "";
			blockJson["time"] = "";
			blockJson["targetDifficulty"] = "";
			blockJson["_version"] = BLOCK_VERSION;
			blockJson["transactions"] = json::array();
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
				if (constants::debugPrint == true)
					std::cerr << e.what() << std::endl;
				return 0;
			}
		}


		cons_miner.MiningPrint();
		cons_miner.WriteLine("Mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");

		return 1;
	}
	catch (const std::exception& e)
	{
		//if (constants::debugPrint == true)
		std::cerr << e.what() << std::endl;
		return 0;
	}
}

// Mine blocks for a pool
int PoolMine(std::string poolURL, json& walletInfo)
{
	cons_miner.NetworkPrint();
	cons_miner.Write("Use pool ");
	cons_miner.WriteLine(poolURL, cons_miner.brightCyanFGColor, "");
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

			cons_miner.NetworkPrint();
			cons_miner.Write("new job ", cons_miner.magentaFGColor, "");
			cons_miner.WriteLine("from " + poolURL + " nonces " + std::to_string(maxNonce - nonce) + " height " + std::to_string(blockNumber));
		}
		catch (const std::exception& e)
		{
			cons_miner.NetworkErrorPrint();
			cons_miner.WriteLine("Connection error.");
			std::cerr << e.what() << std::endl;
			return 0;
		}

		//walletInfo["targetDifficulty"] = "0000000FFFFFF000000000000000000000000000000000000000000000000000";
		cons_miner.MiningPrint();
		cons_miner.Write("Mining ");
		cons_miner.Write("block " + std::to_string(blockNumber), cons_miner.cyanFGColor, "");
		cons_miner.Write(" at difficulty ");
		cons_miner.Write(difficulty, cons_miner.cyanFGColor, "");
		cons_miner.Write(" :\n");
		try
		{
			auto startTime = std::chrono::steady_clock::now();

			// The rust program execution needs to be thought out more, because it would need changes for pool mining.
			//cons_miner.RustPrint();
			//cons_miner.WriteLine("Starting program... ");
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
					cons_miner.Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + FormatWithCommas<unsigned long long int>(nonce) + " # " + std::string(sha256OutBuffer));
					cons_miner.Write("   " + FormatHPS(hashesPerSecond) + "            ");
					//std::cout << std::endl <<  databuffer << std::endl;
				}
			} while (!CompareCharNumbers(c_difficulty, hash) && nonce <= maxNonce);

			std::cout << std::endl;

			// If the nonce is exhausted and the hash is still wrong, request new job from the server
			if (nonce > maxNonce && !CompareCharNumbers(c_difficulty, hash))
				continue;


			//// Wait for the rust program to finish running
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
				cons_miner.NetworkPrint();
				cons_miner.Write("accepted ", cons_miner.greenFGColor, "");
				cons_miner.WriteLine("nonces " + (std::string)numberstring);
			}
			catch (const std::exception& e)
			{
				cons_miner.NetworkErrorPrint();
				cons_miner.WriteLine("Connection error.");
				std::cerr << e.what() << std::endl;
				return 0;
			}


			cons_miner.MiningPrint();
			cons_miner.WriteLine("Mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");

		}
		catch (const std::exception& e)
		{
			//if (constants::debugPrint == true)
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
	std::string lastHash = o["lastHash"];

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
	std::string fDat = lastHash + txData;
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
			cons_miner.Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + CommaLargeNumber(nonce) + " # " + std::string(sha256OutBuffer));
			cons_miner.Write("   " + FormatHPS(hashesPerSecond) + "            ");
		}

		nonce++;
		sha256_full_cstr((char*)(hData + std::to_string(nonce)).c_str(), hash);
	}

	std::cout << std::endl;
	cons_miner.MiningPrint();
	cons_miner.WriteLine("Debug mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");
	cons_miner.MiningPrint();
	cstr_to_hexstr(hash, 32, sha256OutBuffer);
	cons_miner.Write("Final value: hash # ");
	cons_miner.WriteLine(std::string(sha256OutBuffer), "", cons_miner.greenFGColor);
	cons_miner.MiningPrint();
	cons_miner.Write("Final value: nonce # ");
	cons_miner.WriteLine(std::to_string(nonce), "", cons_miner.greenFGColor);

	return 0;
}


