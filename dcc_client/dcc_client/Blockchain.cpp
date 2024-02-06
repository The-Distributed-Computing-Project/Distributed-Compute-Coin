#pragma once


#include "Blockchain.h"



//
//int GetProgram();
//float GetProgramLifeLeft(std::string& programID);
//json UpgradeBlock(json& b, std::string toVersion);



std::string programID;
json programConfig;

//P2P p2p;

// Sync a single pending block from a peer
int SyncPending(P2P& p2p, int whichBlock)
{
	if (fs::exists("./wwwdata/pendingblocks/block" + std::to_string(whichBlock) + ".dccblock"))
		return 1;

	p2p.messageStatus = p2p.requesting_pendingblock;
	p2p.messageAttempt = 0;
	p2p.reqDat = whichBlock;

	while (p2p.isAwaiting()) {}

	return 1;
}

// Sync a single solid block from a peer
int SyncBlock(P2P& p2p, int whichBlock, bool force)
{
	if (fs::exists("./wwwdata/blockchain/block" + std::to_string(whichBlock) + ".dccblock") && !force)
		return 1;

	p2p.messageStatus = p2p.requesting_block;
	p2p.messageAttempt = 0;
	p2p.reqDat = whichBlock;

	while (p2p.isAwaiting()) {}

	return 1;
}

// Sync the entire blockchain
int Sync(P2P& p2p, json& walletInfo)
{
	try
	{
		for (int i = 1; i < walletInfo["BlockchainLength"]; i++)
			if (!fs::exists("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock"))
				SyncBlock(p2p, i);
		//GetProgram(walletInfo);
		return 1;
	}
	catch (const std::exception& e)
	{
		ERRORMSG("Failed to sync chain\n" << e.what());
		return 0;
	}
}

// Read the configuration file of the assigned program, and return the JSON data
json ReadProgramConfig()
{
	std::ifstream t("./wwwdata/programs/" + programID + ".cfg");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string content = buffer.str();
	return json::parse(content);
}

// Write the JSON data for the assigned program to file
int WriteProgramConfig()
{
	try
	{
		std::ofstream configFile("./wwwdata/programs/" + programID + ".cfg");
		if (configFile.is_open())
		{
			configFile << programConfig.dump();
			configFile.close();
		}
		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

// Make sure a rust program is assigned. If one is not, or it's life is 0, then download a new one     // TODO: Change to download from peers instead of server
int GetProgram(json& walletInfo)
{
	p2p.messageStatus = p2p.requesting_deluge_file;
	p2p.messageAttempt = 0;
	//p2p.reqDat = whichBlock;

	while (p2p.isAwaiting()) {}

	
	float life = 0;
	for (auto item : fs::directory_iterator("./wwwdata/programs/"))
	{
		if ((item.path().string()).find(".cfg") != std::string::npos)
		{
			programID = SplitString(SplitString((item.path()).string(), ".cfg")[0], "/programs/")[1];
			walletInfo["ProgramID"] = programID;
			life = GetProgramLifeLeft();
			if (WalletSettingValues::verbose >= 2) {
				console::MiningPrint();
				console::WriteLine("Program life is " + std::to_string(life) + " mins.");
			}
			break;
		}
	}

	try
	{
		if (life <= 0)
		{
			for (auto oldProgram : fs::directory_iterator("./wwwdata/programs/"))
			{
				try
				{
					remove(oldProgram.path());
				}
				catch (const std::exception&)
				{
					console::ErrorPrint();
					console::WriteLine("Error removing \"" + oldProgram.path().string() + "\"");
				}
			}

			Http http;
			std::vector<std::string> args = { "query=assignProgram" };
			std::string assignedProgram = http.StartHttpWebRequest(serverURL + "/dcc/", args);

			console::NetworkPrint();
			console::WriteLine("Assigning Program...");

			programID = assignedProgram;

			if (WalletSettingValues::verbose >= 2) {
				console::NetworkPrint();
				console::WriteLine("./wwwdata/programs/" + programID + ".cfg");
			}

			DownloadFile(serverURL + "/dcc/programs/" + programID + ".cfg", "./wwwdata/programs/" + programID + ".cfg", true);
			DownloadFile(serverURL + "/dcc/programs/" + programID + ".zip", "./wwwdata/programs/" + programID + ".zip", true);

			std::string tarExtractCommand = "tar -xf ./wwwdata/programs/" + programID + ".zip -C ./wwwdata/programs/";

			//ExecuteCommand(tarExtractCommand.c_str());
			if (!fs::exists("./wwwdata/programs/" + programID)) {
				//ExecuteCommand(("mkdir ./wwwdata/programs/" + programID).c_str());
				//fs::create_directory("./wwwdata/programs/" + programID);
				ExecuteCommand(tarExtractCommand.c_str());
			}
			//ExecuteCommand(("cargo build ./wwwdata/programs/" + programID + "/").c_str());
			//ExtractZip("./wwwdata/programs/" + programID + ".zip", "./wwwdata/programs/" + programID);

		//// If improperly zipped (meaning Cargo.toml file is deeper in the directory than the base folder),
		//// the contents will be moved up a single directory.
		//if (!fs::exists("./wwwdata/programs/" + programID + "/Cargo.toml"))
		//{
		//	Directory.Move(Directory.GetDirectories("./wwwdata/programs/" + programID)[0], "./wwwdata/programs/tmpdir");
		//	Directory.Delete("./wwwdata/programs/" + programID, true);
		//	Directory.Move("./wwwdata/programs/tmpdir", "./wwwdata/programs/" + programID);
		//}
		}

		char sha256OutBuffer[65];
		sha256_file((char*)("./wwwdata/programs/" + programID + ".zip").c_str(), sha256OutBuffer);
		std::string ourHash = sha256OutBuffer;

		Http http1;
		std::vector<std::string> args1 = { "query=hashProgram", "programID=" + programID };
		std::string theirHash = http1.StartHttpWebRequest(serverURL + "/dcc/", args1);

		if (ourHash != theirHash)
		{
			console::MiningErrorPrint();
			console::WriteLine("Assigned program has been modified, re-downloading...");
			GetProgram(walletInfo);
		}

		programConfig = ReadProgramConfig();

		if (programConfig["Built"] == false)
		{
			console::MiningPrint();
			console::WriteLine("Building assigned program, wait until it's finished to start mining");

			console::DockerPrint();
			console::WriteLine("Compiling program... ");
			//ExecuteCommand(("cargo build --release --manifest-path ./wwwdata/programs/" + programID + "/Cargo.toml").c_str());

			ExecuteAsync("docker run -d --network none --rm --name=" + (std::string)(walletInfo["ProgramID"]) + " -v ./wwwdata/programs/" + (std::string)(walletInfo["ProgramID"]) + ":/out/ " + (std::string)(walletInfo["ProgramID"]) + " /bin/bash build.sh", true);
			boost::process::child containerProcess = ExecuteAsync("docker wait " + (std::string)(walletInfo["ProgramID"]), false);

			while (containerProcess.running()) {}

			console::DockerPrint();
			console::WriteLine("Done Compiling");

			programConfig["Built"] = true;
			WriteProgramConfig();
		}
		return 1;
	}
	catch (const std::exception& e)
	{
		ERRORMSG("Error getting program\n" << e.what());
		return 0;
	}
}

char outDatArray[DELUGE_CHUNK_SIZE + 5];
// Make a Docker container and Deluge file for a program
int MakeProgram(json& walletInfo, json& walletConfig, std::string& path)
{
	FILE* pFile;
	pFile = fopen(path.c_str(), "rb");
	fseek(pFile, 0L, SEEK_END);
	size_t size = ftell(pFile);
	fseek(pFile, 0L, SEEK_SET);
	char* byteArray = new char[size+1];
	byteArray[size] = '\0';
	if (pFile != NULL)
	{
		int counter = 0;
		do {
			byteArray[counter] = fgetc(pFile);
			counter++;
		} while (counter <= size);
		fclose(pFile);
	}

	//std::ifstream t(path);
	//if (t.is_open()) {
		//std::stringstream buffer;
		//buffer << t.rdbuf();
		//std::string content = buffer.str();
	std::cout << "total size: " << size << " bytes\n";

	// Create hash for each 32kb chunk of the file, and add to list
	std::vector<std::string> hashList;
	std::string allHashesString;
	int ind = 0;
	uint16_t chunks = 0;
	unsigned char outBuffer[20];
	char strOutBuffer[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	int actualSize = 0;
	do
	{
		csubstr(byteArray, outDatArray, ind, DELUGE_CHUNK_SIZE, size, actualSize);
		cConcatInt(outDatArray, outDatArray, actualSize, chunks);
		sha1_str(outDatArray, outBuffer);
		cstr_to_hexstr(outBuffer, 20, strOutBuffer);

		auto it = std::find(hashList.begin(), hashList.end(), strOutBuffer);

		// If element was found, only add index of it
		if (it != hashList.end())
		{
			int index = it - hashList.begin();
			hashList.push_back(std::to_string(index));
		}
		// Else, add as new element
		else
		{
			hashList.push_back(strOutBuffer);
		}

		std::cout << "Building part `" << PadString(std::to_string(chunks), '0', 4) << "`  ,  " << PadString(std::to_string(ind), '0', std::to_string(size).size()) << " of " << size << " bytes" << "   =>   " << hashList.at(hashList.size()-1) << std::endl;
		allHashesString += strOutBuffer;
		ind += DELUGE_CHUNK_SIZE;
		chunks++;
	} while (ind < size && chunks < 2000);

	// If the total number of chunks is 2000 and the index is still less than the total size,
	// then we cannot continue because this program is too large
	if (chunks >= 2000 && ind < size) {
		console::ErrorPrint();
		console::WriteLine("Could not complete, file is too large.");
		// Free memory allocated using `new`
		delete[] byteArray;
		return 1;
	}

	// Hash one last time, this time using all hashes as a total file checksum, and SHA256
	char sha256OutBuffer[65];
	unsigned char hash[32];
	sha256_string((char*)(allHashesString.c_str()), sha256OutBuffer);
	std::string hData = std::string(sha256OutBuffer);

	// Create json object storing the program data
	json programData = json::object({});
	programData = {
			{"hashList", hashList},
			{"_ip", (std::string)walletConfig["ip"]},
			{"_address", (std::string)walletInfo["Address"]},
			{"_totalHash", hData},
			{"_chunkSizeB", DELUGE_CHUNK_SIZE},
			{"_version", DELUGE_VERSION},
			{"_name", SplitGetLastAfterChar(path,"/").substr(0, 32)}, // Use path as name, also truncate to only 32 chars
			{"peers", json::array()}, // List of peers that say have this file, add self for original distribution
	};
	programData["peers"].push_back({(std::string)walletConfig["ip"], std::stoi((std::string)walletConfig["port"])});

	// Output name will be the total hash (only the first 32 characters)
	std::ofstream programDeluge("./wwwdata/developing-programs/" + hData.substr(0, 32) + ".deluge");
	if (programDeluge.is_open())
	{
		programDeluge << programData.dump();
		programDeluge.close();
	}

	// Build the container
	//ExecuteCommand("docker build " + path + " -t dccfile/" + hData.substr(0, 32));
	//ExecuteCommand("docker save -o " + hData.substr(0, 32) + ".tar dccfile/" + hData.substr(0, 32) + ":latest");

	// Free memory allocated using `new`
	delete[] byteArray;

	return 0;
}

// Verify a Deluge file for a program
bool VerifyDeluge(json& delugeJson, std::string& path)
{
	FILE* pFile;
	pFile = fopen(path.c_str(), "rb");
	fseek(pFile, 0L, SEEK_END);
	size_t size = ftell(pFile);
	fseek(pFile, 0L, SEEK_SET);
	char* byteArray = new char[size+1];
	byteArray[size] = '\0';
	if (pFile != NULL)
	{
		int counter = 0;
		do {
			byteArray[counter] = fgetc(pFile);
			counter++;
		} while (counter <= size);
		fclose(pFile);
	}

	std::cout << "total size: " << size << " bytes\n";

	// Create hash for each 32kb chunk of the file, and verify it with the one in the deluge
	std::string allHashesString;
	int ind = 0;
	uint16_t chunks = 0;
	unsigned char outBuffer[20];
	char strOutBuffer[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	int actualSize = 0;
	do
	{
		csubstr(byteArray, outDatArray, ind, DELUGE_CHUNK_SIZE, size, actualSize);
		cConcatInt(outDatArray, outDatArray, actualSize, chunks);
		sha1_str(outDatArray, outBuffer);
		cstr_to_hexstr(outBuffer, 20, strOutBuffer);

		std::string expectedHash = (std::string)delugeJson["hashList"][chunks];
		if(expectedHash.size() < 20) // If the length of the string is less than the hash size, it is referencing another index
			expectedHash = (std::string)delugeJson["hashList"][std::stoi(expectedHash)];

		if(strOutBuffer != expectedHash){
			delete[] byteArray;
			return false;
		}

		std::cout << "Checking part `" << PadString(std::to_string(chunks), '0', 4) << "`  ,  " << PadString(std::to_string(ind), '0', std::to_string(size).size()) << " of " << size << " bytes" << "   =>   " << hashList.at(hashList.size()-1) << std::endl;
		allHashesString += strOutBuffer;
		ind += DELUGE_CHUNK_SIZE;
		chunks++;
	} while (ind < size && chunks < 2000);

	// If the total number of chunks is 2000 and the index is still less than the total size,
	// then we cannot continue because this program is too large
	if (chunks >= 2000 && ind < size) {
		console::ErrorPrint();
		console::WriteLine("Could not complete, file is too large.");
		// Free memory allocated using `new`
		delete[] byteArray;
		return false;
	}

	// Hash one last time, this time using all hashes as a total file checksum, and SHA256
	char sha256OutBuffer[65];
	unsigned char hash[32];
	sha256_string((char*)(allHashesString.c_str()), sha256OutBuffer);
	std::string hData = std::string(sha256OutBuffer);

	if(hData != (std::string)(std::string)delugeJson["_totalHash"]){
		delete[] byteArray;
		return false;
	}

	// Free memory allocated using `new`
	delete[] byteArray;

	return 0;
}

// Get the amount of time left of the current assigned rust program, by asking the server     // TODO: change to ask peers instead of the server
float GetProgramLifeLeft()
{
	try
	{
		Http http;
		std::vector<std::string> args = { "query=getProgramLifeLeft", "programID=" + programID };
		std::string html = http.StartHttpWebRequest(serverURL + "/dcc/", args);

		if (html.find("ERR") != std::string::npos || html == "")
			return -100;
		std::string cpy = html;
		boost::trim(cpy);
		return stof(cpy);
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

void CreateTransaction(P2P& p2p, json& walletInfo, double& amount) {

}

// Check every single block to make sure the nonce is valid, the hash matches the earlier and later blocks, and each transaction has a valid signature.
bool IsChainValid(P2P& p2p, json& walletInfo)
{
	console::BlockchainPrint();
	console::WriteLine("Validating blockchain...");
	try {
		/*while (FileCount("./wwwdata/blockchain/") < walletInfo["BlockchainLength"])
		{
			if (SyncBlock(p2p, FileCount("./wwwdata/blockchain/") + 1) == 0)
			{
				ConnectionError();
				break;
			}
		}
		for (size_t i = 1; i < FileCount("./wwwdata/blockchain/")+1; i++)
		{
			console::Write("\rChecking existence of block " + std::to_string(i));
			SyncBlock(p2p, i);
		}*/

		int chainLength = FileCount("./wwwdata/blockchain/");

		double tmpFunds = 0;
		int txNPending = 0;

		console::BlockchainPrint();
		console::WriteLine("Checking blocks...");

		// Apply funds to user from the first block separately
		try
		{
			if (chainLength >= 1) {
				std::ifstream th;
				th.open("./wwwdata/blockchain/block1.dccblock");
				if (!th.is_open())
					ERRORMSG("Could not open file");
				std::stringstream buffer2;
				buffer2 << th.rdbuf();
				std::string content2 = buffer2.str();
				json firstBlock = json::parse(content2);

				if (firstBlock["_version"] == nullptr || firstBlock["_version"] == "" || firstBlock["_version"] != BLOCK_VERSION)
					UpgradeBlock(firstBlock);

				for (int tr = 0; tr < firstBlock["transactions"].size(); tr++) {
					std::string fromAddr = (std::string)firstBlock["transactions"][tr]["tx"]["fromAddr"];
					std::string toAddr = (std::string)firstBlock["transactions"][tr]["tx"]["toAddr"];
					float amount = firstBlock["transactions"][tr]["tx"]["amount"];
					std::string signature = decode64((std::string)firstBlock["transactions"][tr]["sec"]["signature"]);
					std::string publicKey = (std::string)firstBlock["transactions"][tr]["sec"]["pubKey"];
					std::string note = (std::string)firstBlock["transactions"][tr]["sec"]["note"];

					// Check if the sending or receiving address is the same as the user's
					if ((std::string)walletInfo["Address"] == fromAddr)
						tmpFunds -= amount;
					else if ((std::string)walletInfo["Address"] == toAddr)
						tmpFunds += amount;
				}
				// Save upgraded block
				std::ofstream blockFile("./wwwdata/blockchain/block1.dccblock");
				if (blockFile.is_open())
				{
					blockFile << firstBlock.dump();
					blockFile.close();
				}

				// Make sure it is valid:
				console::Write("\r");
				console::WriteBulleted("Validating block: " + std::to_string(1), 3);
				char sha256OutBuffer[65];
				std::string pprev = firstBlock["pprev"];
				std::string currentHash = firstBlock["hash"];
				std::string nonce = firstBlock["nonce"];
				// The data we will actually be hashing is a hash of the
				// transactions and header, so we don't need to do calculations on
				// massive amounts of data
				std::string txData; // Only use the `tx` portion of each transaction objects' data
				for (size_t i = 0; i < firstBlock["transactions"].size(); i++)
				{
					txData += (std::string)firstBlock["transactions"][i]["tx"].dump();
				}
				// Compile all data into shorter format:
				std::string fDat = (std::string)firstBlock["pprev"] + txData;
				sha256_string((char*)(fDat.c_str()), sha256OutBuffer);
				std::string hData = std::string(sha256OutBuffer);

				// Check what the hash is with the nonce:
				sha256_string((char*)(hData + nonce).c_str(), sha256OutBuffer);
				std::string blockHash = sha256OutBuffer;

				if (blockHash != currentHash)
				{
					std::string rr = "";
					if (blockHash != currentHash)
						rr += "1";
					console::WriteLine("    X Bad Block X  " + std::to_string(1) + " R" + rr, console::redFGColor, "");
					return false;
				}
			}
		}
		catch (const std::exception& e)
		{
			//if (WalletSettingValues::verbose == true) {
			std::cerr << "\n";
			ERRORMSG("Error\n" << e.what());
			//}
			//console::ExitError("Failure, exiting 854");
		}

		// Then process the rest of the blocks
		for (int i = 2; i <= chainLength; i++)
		{
			try
			{
				std::ifstream t;
				t.open("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
				if (!t.is_open())
					ERRORMSG("Could not open file");
				std::stringstream buffer;
				buffer << t.rdbuf();
				std::string content = buffer.str();
				t.close();

				bool changedBlockData = false;
				json o = json::parse(content);

				std::string rewardedAddress; // The address that is awarded the gas fees and block reward

				// Make sure block is up-to-date
				if (o["_version"] == nullptr || o["_version"] == "" || o["_version"] != BLOCK_VERSION)
				{
					UpgradeBlock(o);
					std::ofstream blockFile("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
					if (blockFile.is_open())
					{
						blockFile << o.dump();
						blockFile.close();
					}
				}

				std::string pprev = o["pprev"];
				std::string currentHash = o["hash"];
				std::string nonce = o["nonce"];

				// Get the previous block
				std::ifstream td;
				td.open("./wwwdata/blockchain/block" + std::to_string(i - 1) + ".dccblock");
				if (!td.is_open())
					ERRORMSG("Could not open file");
				std::stringstream bufferd;
				bufferd << td.rdbuf();
				td.close();
				std::string nextBlockText = bufferd.str();
				json o2 = json::parse(nextBlockText);

				std::string lastRealHash = o2["hash"];

				if (i % 10 == 0 || i >= chainLength - 2) {
					console::Write("\r");
					console::WriteBulleted("Validating block: " + std::to_string(i), 3);
				}
				char sha256OutBuffer[65];
				// The data we will actually be hashing is a hash of the
				// transactions and header, so we don't need to do calculations on
				// massive amounts of data
				std::string txData = ""; // Only use the `tx` portion of each transaction objects' data
				for (size_t i = 0; i < o["transactions"].size(); i++)
				{
					txData += (std::string)(o["transactions"][i]["tx"].dump());
				}
				std::string fDat = (std::string)o["pprev"] + txData;
				sha256_string((char*)(fDat.c_str()), sha256OutBuffer);
				std::string hData = std::string(sha256OutBuffer);

				sha256_string((char*)(hData + nonce.c_str()).c_str(), sha256OutBuffer);
				std::string blockHash = std::string(sha256OutBuffer);

				if ((blockHash[0] != '0' && blockHash[1] != '0') || blockHash != currentHash || lastRealHash != pprev)
				{
					std::string rr = "";
					if ((blockHash[0] != '0' && blockHash[1] != '0'))
						rr += "0";
					if (blockHash != currentHash)
						rr += "1";
					if (lastRealHash != pprev)
						rr += "2";
					console::WriteLine("    X Bad Block X  " + std::to_string(i) + " R" + rr + "   # " + blockHash, console::redFGColor, "");
					return false;
				}
				float tmpFunds2 = 0;
				// Check all transactions to see if they have a valid signature
				for (int tr = 0; tr < o["transactions"].size(); tr++) {
					std::string fromAddr = (std::string)o["transactions"][tr]["tx"]["fromAddr"];
					std::string toAddr = (std::string)o["transactions"][tr]["tx"]["toAddr"];
					float amount = o["transactions"][tr]["tx"]["amount"];
					std::string signature = decode64((std::string)o["transactions"][tr]["sec"]["signature"]);
					std::string publicKey = (std::string)o["transactions"][tr]["sec"]["pubKey"];
					std::string note = (std::string)o["transactions"][tr]["sec"]["note"];

					// If this is the first transaction, that is the block reward, so it should be handled differently:
					if (tr == 0) {
						if ((std::string)walletInfo["Address"] == toAddr) { // If this is the receiving address, then give reward
							tmpFunds2 += amount;
						}
						rewardedAddress = toAddr;
						continue;
					}

					// The from address should be the same as the hash of the public key, so check that first:
					char walletBuffer[65];
					sha256_string((char*)(publicKey).c_str(), walletBuffer);
					std::string expectedWallet = walletBuffer;
					if (fromAddr != expectedWallet) {
						o["transactions"].erase(o["transactions"].begin() + tr);
						continue;
					}

					// Hash transaction data
					sha256_string((char*)(o["transactions"][tr]["tx"].dump()).c_str(), sha256OutBuffer);
					std::string transHash = sha256OutBuffer;

					// Verify signature by decrypting signature with public key
					std::string decryptedSig = rsa_pub_decrypt(signature, publicKey);

					// The decrypted signature should be the same as the hash of this transaction we just generated
					if (decryptedSig != transHash) {
						o["transactions"].erase(o["transactions"].begin() + tr);
						console::Write("  Bad signature on T:" + std::to_string(tr), console::redFGColor, "");
						continue;
					}

					// Now check if the sending or receiving address is the same as the user's
					if ((std::string)walletInfo["Address"] == fromAddr) {
						tmpFunds2 -= amount;
						txNPending++;
					}
					else if ((std::string)walletInfo["Address"] == toAddr)
						tmpFunds2 += amount;
					else if (rewardedAddress == (std::string)walletInfo["Address"]) // If you are the one that mined this block, add gas fees
						tmpFunds2 += (float)o["transactions"][tr]["tx"]["transactionFee"];
				}

				// Update funds and transaction number
				tmpFunds += tmpFunds2;
				walletInfo["transactionNumber"] = txNPending;


				if (i % 10 == 0 || i >= chainLength - 2) {
					console::Write("     Transactions: " + std::to_string(o["transactions"].size()));
					console::Write("   Ok  ", console::greenFGColor, "");
				}
			}
			// If there is a failure state, assume that block is bad or does not exist.
			catch (const std::exception& e)
			{
				if (WalletSettingValues::verbose >= 1) {
					ERRORMSG("Error\n" << e.what());
				}

				console::WriteLine();
				SyncBlock(p2p, i);

				i -= 2;
				// Then recount, because we need to know if the synced block is new or overwrote an existing one.
				chainLength = FileCount("./wwwdata/blockchain/");
			}
		}

		//console::WriteLine();
		walletInfo["Funds"] = tmpFunds;
		console::Write("\r");
		console::BlockchainPrint();
		console::Write("Done!                                                         \n");
		return true;
	}
	catch (const std::exception& e)
	{
		ERRORMSG("Error validating chain:\n" << e.what());
	}
	console::Write("\r");
	console::BlockchainPrint();
	console::Write("Done! (there were problems)                                                        \n");
	return false;
}


// Calculates the difficulty of the next block by looking at the past 288 blocks,
// and averaging the time it took between each block to keep it within the 2 min (120 second) range
#define BLOCK_DIFFICULTY_CUTOFF_RANGE 48 // 48*5mins = 4 hours
std::string CalculateDifficulty(json& walletInfo) {
	std::string targetDifficulty = "000000FFFFFF0000000000000000000000000000000000000000000000000000";

	int blockCount = FileCount("./wwwdata/blockchain/");

	// Default difficulty 6 for the first 288 blocks  (24 hours)
	if (blockCount <= 289) {
		walletInfo["targetDifficulty"] = targetDifficulty;
		walletInfo["MineDifficulty"] = ExtractPaddedChars(targetDifficulty, '0');
		return targetDifficulty;
	}

	std::vector<uint16_t> secondCounts;
	uint64_t lastTime = 0;

	// Get first block time
	std::ifstream t;
	t.open("./wwwdata/blockchain/block" + std::to_string(blockCount - 288) + ".dccblock");
	std::stringstream buffer;
	buffer << t.rdbuf();
	json ot = json::parse(buffer.str());
	lastTime = (uint64_t)ot["time"];

	// Iterate last 288 blocks and add their time difference to the vector
	for (int i = blockCount - 287; i <= blockCount; i++) {
		std::ifstream tt;
		tt.open("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
		if (!tt.is_open())
			ERRORMSG("Could not open file");
		std::stringstream buffert;
		buffert << tt.rdbuf();
		json o = json::parse(buffert.str());

		// Get difference between last block time and this one, then add to vector of differences
		uint16_t difference = (uint16_t)((uint64_t)o["time"] - lastTime);
		secondCounts.push_back(difference);

		// Set new last time
		lastTime = (uint64_t)o["time"];
	}

	// Sort the vector so we can exclude the 60 lowest and 60 highest times
	std::sort(secondCounts.begin(), secondCounts.end());

	uint32_t highest = secondCounts[BLOCK_DIFFICULTY_CUTOFF_RANGE];
	uint32_t lowest = secondCounts[288 - BLOCK_DIFFICULTY_CUTOFF_RANGE];

	// Get average of middle 288 block times
	uint32_t avgTotal = 0;
	for (int i = BLOCK_DIFFICULTY_CUTOFF_RANGE; i < 288 - BLOCK_DIFFICULTY_CUTOFF_RANGE; i++)
		avgTotal += secondCounts[i];
	uint32_t average = avgTotal / (288 - BLOCK_DIFFICULTY_CUTOFF_RANGE - BLOCK_DIFFICULTY_CUTOFF_RANGE);  // Divide by total, which gives the average

	// Expected: 86400 seconds total (24 hours per 288 blocks), or 300 seconds average

	//std::map<std::string, int> difficultyOccurrences;
	std::string averageDifficulty = "0000000000000000000000000000000000000000000000000000000000000000";
	// Get the average of previous target difficulties in the past 256 blocks
	for (int i = blockCount - 256; i <= blockCount; i++) {
		std::ifstream tt;
		tt.open("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
		if (!tt.is_open())
			ERRORMSG("Could not open file");
		std::stringstream buffert;
		buffert << tt.rdbuf();
		json o = json::parse(buffert.str());
		std::string mostRecentDifficulty = (std::string)o["targetDifficulty"];

		averageDifficulty = addHexNumbers(averageDifficulty, mostRecentDifficulty);
	}
	// Divide averageDifficulty by 288
	averageDifficulty = shiftHexNumber(averageDifficulty, 2);
	targetDifficulty = PadString(averageDifficulty, '0', 64);
	//// Get the average of all the recent difficulties
	//uint16_t occ = 0;
	//std::map<std::string, int>::iterator itr;
	//for (itr = difficultyOccurrences.begin(); itr != difficultyOccurrences.end(); ++itr) {
	//	if (itr->second > occ) {
	//		occ = itr->second;
	//		targetDifficulty = itr->first;
	//	}
	//}

	// 50400 seconds for (288-60-60) = 168 blocks * 300 seconds
	float ratio = clampf(average / 300.0f, 0.25f, 4.0f);

	if (WalletSettingValues::verbose >= 2) {
		console::WriteBulleted("Average time: " + std::to_string(average) + "s  of  300\n", 3);
		console::WriteBulleted("Min/Max: " + std::to_string(highest) + "s / " + std::to_string(lowest) + "s\n", 3);
		console::WriteBulleted("Ratio: " + std::to_string(ratio) + ",  unclamped: " + std::to_string(average / 300.0) + "\n", 3);
		console::WriteBulleted("Last target difficulty: " + targetDifficulty + "\n", 3);
	}


	std::string newDifficulty = PadString(multiplyHexByFloat(targetDifficulty, (float)ratio), '0', 64);

	if (WalletSettingValues::verbose >= 2)
		console::WriteBulleted("New target difficulty:  " + newDifficulty + "\n", 3);

	if (WalletSettingValues::verbose >= 3) {
		console::WriteBulleted("Test long division:  " + longDivision("123878287", 328) + "\n", 3);
		console::WriteBulleted("Test hex division:  " + hexLongDivision("FF76200", 40) + "\n", 3);
	}

	walletInfo["targetDifficulty"] = newDifficulty;
	walletInfo["MineDifficulty"] = ExtractPaddedChars(targetDifficulty, '0');

	return newDifficulty;
}

// Create a superblock using all of the blocks in the `blockchain` directory
void CreateSuperblock() {

	std::map<std::string, double> walletBalances;

	int blockCount = FileCount("./wwwdata/blockchain/");

	// Iterate all blocks and compute each transaction
	for (int i = 1; i <= blockCount; i++) {
		std::ifstream tt("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
		std::stringstream buffert;
		buffert << tt.rdbuf();
		json o = json::parse(buffert.str());

		std::string rewardedAddress; // The address that is awarded the gas fees and block reward

		// Add / subtract value from each address in the transactions
		for (int tr = 0; tr < o["transactions"].size(); tr++) {
			std::string fromAddr = (std::string)o["transactions"][tr]["tx"]["fromAddr"];
			std::string toAddr = (std::string)o["transactions"][tr]["tx"]["toAddr"];
			double amount = o["transactions"][tr]["tx"]["amount"];
			double transactionFee = o["transactions"][tr]["tx"]["transactionFee"];

			if (tr == 0) {
				if (walletBalances.contains(toAddr))
					walletBalances[toAddr] += amount;
				else
					walletBalances[toAddr] = amount;
				rewardedAddress = toAddr;
			}
			else if (fromAddr != "Block Reward") {
				if (walletBalances.contains(fromAddr))
					walletBalances[fromAddr] -= amount;
				else
					walletBalances[fromAddr] = -amount;

				if (walletBalances.contains(toAddr))
					walletBalances[toAddr] += amount;
				else
					walletBalances[toAddr] = amount;

				if (walletBalances.contains(rewardedAddress))
					walletBalances[rewardedAddress] += transactionFee;
				else
					walletBalances[rewardedAddress] = transactionFee;
			}
		}
	}

	json superblockJson = json();

	//superblockJson["hash"] = "0000000000000000000000000000000000000000000000000000000000000000";
	//superblockJson["pprev"] = hashStr;
	//superblockJson["nonce"] = "";
	//superblockJson["time"] = "";
	//superblockJson["targetDifficulty"] = "";
	superblockJson["_version"] = BLOCK_VERSION;
	superblockJson["endHeight"] = blockCount;
	superblockJson["balances"] = json::array();


	std::map<std::string, double>::iterator it = walletBalances.begin();

	// Iterate through the map and add the elements to the array
	console::WriteLine();
	console::WriteLine("List of all wallet balances:");
	while (it != walletBalances.end())
	{
		console::WriteBulleted("", 1);
		std::printf("Wallet: %s, Balance: $%f\n", it->first.c_str(), it->second);
		json item = json::object({});
		item = {
				{"address", it->first},
				{"balance", it->second},
		};
		superblockJson["balances"].insert(superblockJson["balances"].begin(), item);
		++it;
	}
	console::WriteLine();

	int superblockCount = FileCount("./wwwdata/superchain/");
	std::ofstream blockFilew("./wwwdata/superchain/" + std::to_string(superblockCount + 1) + ".dccsuper");
	if (blockFilew.is_open())
	{
		blockFilew << superblockJson.dump();
		blockFilew.close();
	}
}

// Upgrade a block to a newer version
json UpgradeBlock(json& b)
{
	//if (WalletSettingValues::verbose == true) {
	console::WriteLine();
	console::BlockchainPrint();
	console::WriteIndented("Upgrading block ", "", "", 1);
	console::Write(std::to_string((uint64_t)b["id"]), console::greenFGColor, "");
	console::Write(" to version ");
	console::Write(BLOCK_VERSION, console::cyanFGColor, "");
	//}

	std::string currentVersion = (std::string)b["_version"];

	// v0.0.1-alpha-coin
	// Changes:
	// * Add version field
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.0.1-alpha-coin") == false)
	{
		b["_version"] = "v0.0.1-alpha-coin";
	}

	// v0.2.0-alpha-coin
	// Changes:
	// * Convert all transactions from list array to object
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.2.0-alpha-coin") == false)
	{
		b["_version"] = "v0.2.0-alpha-coin";
	}

	// v0.3.0-alpha-coin
	// Changes:
	// * Add new targetDifficulty variable
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.3.0-alpha-coin") == false)
	{
		b["targetDifficulty"] = "0000000FFFF0000000000000000000000000000000000000000000000000000";
		b["_version"] = "v0.3.0-alpha-coin";
	}

	// v0.4.0-alpha-coin
	// Changes:
	// * Remove txNum
	// * Add unlockTime
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.4.0-alpha-coin") == false)
	{
		// Add unlockTime variable to each transaction
		for (int tr = 0; tr < b["transactions"].size(); tr++) {
			b["transactions"][tr]["unlockTime"] = 0;
			b["transactions"][tr].erase("txNum");
		}
		b["_version"] = "v0.4.0-alpha-coin";
	}

	// v0.5.0-alpha-coin
	// Changes:
	// * Remove txNum (actually)
	// * Add unlockTime (actually)
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.5.0-alpha-coin") == false)
	{
		// Add unlockTime variable to each transaction
		for (int tr = 0; tr < b["transactions"].size(); tr++) {
			b["transactions"][tr]["tx"]["unlockTime"] = 0;
			b["transactions"][tr]["tx"].erase("txNum");
			b["transactions"][tr].erase("unlockTime");
		}
		b["_version"] = "v0.5.0-alpha-coin";
	}

	// v0.6.0-alpha-coin
	// Changes:
	// * Switch to using hexadecimal format for nonce.
	//       (This is optional, so no changes need to be made to the block.)
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.6.0-alpha-coin") == false)
	{
		b["_version"] = "v0.6.0-alpha-coin";
	}

	// v0.7.0-alpha-coin
	// Changes:
	// * Add transactionFee to each transaction
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.7.0-alpha-coin") == false)
	{
		// Add transactionFee to each transaction
		for (int tr = 0; tr < b["transactions"].size(); tr++) {
			b["transactions"][tr]["tx"]["transactionFee"] = 0.0;
		}
		b["_version"] = "v0.7.0-alpha-coin";
	}

	// v0.8.0-alpha-coin
	// Changes:
	// * Create containerTask section of block for holding task data sources and hashes
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.8.0-alpha-coin") == false)
	{
		// Add containerTask to each transaction
		b["containerTask"]["taskID"] = "";
		b["containerTask"]["taskInstances"] = json::array();
		b["containerTask"]["taskInstances"].push_back(json::parse("{\"seed\":\"\",\"taskDataHash\":\"\",\"responsiblePeers\":[]}"));
		b["_version"] = "v0.8.0-alpha-coin";
	}

	// v0.8.1-alpha-coin
	// Changes:
	// * Make containerTask object into containerTasks array
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.8.1-alpha-coin") == false)
	{
		// Add containerTask to each transaction
		b.erase("containerTask");
		b["containerTasks"] = json::array();
		b["_version"] = "v0.8.1-alpha-coin";
	}

	// v0.8.2-alpha-coin
	// Changes:
	// * Change `lastHash` to `pprev`
	// * Add `pnext`
	// * Update version
	if (IsVersionGreaterOrEqual(currentVersion, "v0.8.2-alpha-coin") == false)
	{
		// pprev
		b["pprev"] = b["lastHash"];
		b.erase("lastHash");
		// pnext
		std::ifstream t;
		t.open("./wwwdata/blockchain/block" + std::to_string((uint64_t)b["id"] + 1) + ".dccblock");
		if (!t.is_open()) {
			ERRORMSG("Could not open file, skipping `pnext` upgrade");
			b["pnext"] = "";
		}
		else {
			std::stringstream buffer;
			buffer << t.rdbuf();
			std::string content = buffer.str();
			t.close();
			b["pnext"] = (json::parse(content))["hash"];
		}

		b["_version"] = "v0.8.2-alpha-coin";
	}





	// Make sure there is always an upgrade step. If there isn't, then throw error
	if (b["_version"] != BLOCK_VERSION) {
		console::WriteLine();
		console::ErrorPrint();
		console::Write("No block upgrade step found from version ", console::redFGColor, "");
		console::Write("\"" + (std::string)b["_version"] + "\"", console::cyanFGColor, "");
		console::Write(" to ", console::redFGColor, "");
		console::Write("\"" + BLOCK_VERSION + "\"", console::cyanFGColor, "");
		console::ExitError("");
	}

	return b;
}
