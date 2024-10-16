// DCC-Miner.cpp

#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif

#include "Main.h"

using json = nlohmann::json;
namespace fs = std::filesystem;


void Help();
void Logo();
int SendFunds(std::string& toAddress, float amount);




int connectionStatus = 1;
const std::string directoryList[] = { "./sec", "./wwwdata", "./wwwdata/blockchain", "./wwwdata/pendingblocks", "./wwwdata/programs", "./wwwdata/superchain" };

json walletConfig;
json walletInfo;

std::string endpointAddr = "";
std::string endpointPort = "";
std::string peerPort = "";

//int transactionNumber = 0;

Console console;

struct stat info;

std::vector<std::string> keypair = { "", "" };

P2P p2p;

int main()
{
	Logo();

	console.WriteLine("hextest: ");
	console.WriteLine("\"" + divideHexByFloat("ffffff", 1.3) + "\"");
	console.WriteLine("\"" + divideHexByFloat("0f0", 2) + "\"");
	console.WriteLine("\"" + divideHexByFloat("0fff0", 2) + "\"");
	console.WriteLine("\"" + multiplyHexByFloat("ff00", 2) + "\"");

	// Create required directories if they don't exist
	for (std::string dir : directoryList)
		if (!fs::is_directory(dir) || !fs::exists(dir)) {
			console.SystemPrint();
			console.WriteLine("Creating " + dir);
			fs::create_directory(dir);
		}

	// Create config.cfg file if it doesn't exist 
	console.SystemPrint();
	console.WriteLine("Checking config.cfg");
	if (!fs::exists("./config.cfg"))
	{
		int prt;
		console.ErrorPrint();
		console.Write("Config file not found. \nPlease input the port # you want to use \n(default 5000): ");
		std::cin >> prt;
		if (prt <= 0 || prt > 65535)
			prt = 5000;

		// Get public IP address
		console.NetworkPrint();
		console.WriteLine("Getting public IP address...");
		Http http;
		std::vector<std::string> args;
		std::string ipStr = http.StartHttpWebRequest("https://api.ipify.org", args); // This is a free API that lets you get IP 
		console.NetworkPrint();
		console.WriteLine("Done.");

		std::ofstream configFile("./config.cfg");
		if (configFile.is_open())
		{
			configFile << "{\"port\":" << std::to_string(prt) << ",\"ip\":\"" << ipStr << "\"}";
			configFile.close();
		}
	}

	// Generate and save keypair if it doesn't exist
	console.SystemPrint();
	console.WriteLine("Checking keypairs...");
	if (!fs::exists("./sec/prikey.pem"))
	{
		console.SystemPrint();
		console.WriteLine("None found, generating keypairs...");
		keypair = GenerateKeypair();
		std::cout << "Public key: " << std::endl;
		std::cout << keypair[0] << std::endl;
		std::cout << "Private key: " << std::endl;
		std::cout << keypair[1] << std::endl;
	}
	// Load public key as keypair[0]
	std::ifstream pkey("./sec/pubkey.pem");
	std::stringstream keybuf;
	keybuf << pkey.rdbuf();
	keypair[0] = keybuf.str();
	// Load private key as keypair[1]
	std::ifstream skey("./sec/prikey.pem");
	std::stringstream skeybuf;
	skeybuf << skey.rdbuf();
	keypair[1] = skeybuf.str();

	// Calculate wallet from hash of public key
	char walletBuffer[65];
	sha256_string((char*)(keypair[0]).c_str(), walletBuffer);
	std::string wallet = walletBuffer;

	console.SystemPrint();
	console.Write("Your wallet: ");
	console.WriteLine(wallet, console.greenFGColor, "");
	walletInfo["Address"] = wallet;


	p2p.InitPeerList();


	// Load the wallet config file and get the P2P port and IP
	std::ifstream conf("./config.cfg");
	std::stringstream confbuf;
	confbuf << conf.rdbuf();
	walletConfig = json::parse(confbuf.str());

	endpointAddr = (std::string)walletConfig["ip"];
	endpointPort = std::to_string((int)walletConfig["port"]);

	console.SystemPrint();
	console.WriteLine("Client endpoint: " + (std::string)walletConfig["ip"] + ":" + std::to_string((int)walletConfig["port"]));


	// Open the socket required to accept P2P requests and send responses
	p2p.OpenP2PSocket((int)walletConfig["port"]);
	// Start the P2P listener thread
	std::thread t1(&P2P::ListenerThread, &p2p, 500);
	// Start the P2P sender thread
	std::thread t2(&P2P::SenderThread, &p2p);


	//
	// Gather wallet information, validate blockchain, and print information.
	//	

	console.SystemPrint();
	console.WriteLine("Getting wallet info...");

	walletInfo["Funds"] = 0.0f;
	walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
	walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");

	console.MiningPrint();
	console.Write("There are ");
	console.Write(std::to_string((int)walletInfo["PendingLength"]), console.greenFGColor, "");
	console.WriteLine(" Block(s) to compute");

	console.SystemPrint();
	console.WriteLine("Calculating difficulty...");
	std::string dif = CalculateDifficulty(walletInfo);
	console.SystemPrint();
	console.Write("The current difficulty looks like: ");
	console.WriteLine(ExtractPaddedChars(dif, '0'), console.redFGColor, "");

	console.NetworkPrint();
	console.WriteLine("Syncing blocks...");
	Sync(p2p, walletInfo);
	try
	{
		console.BlockCheckerPrint();
		console.WriteLine("Validating blockchain...");

		IsChainValid(p2p, walletInfo);

		console.BlockCheckerPrint();
		console.WriteLine("Done!");
	}
	catch (const std::exception&)
	{
		Sync(p2p, walletInfo);
	}

	console.SystemPrint();
	console.Write("You have: ");
	console.WriteLine("$" + CommaLargeNumberF((double)walletInfo["Funds"]) + " credits\n", console.yellowFGColor, "");


	//
	// Start command loop
	//
	while (true)
	{
		console.Write("Input:  ");
		std::string command = console.ReadLine();

		if (command == "") {
			console.ErrorPrint();
			console.WriteLine("Invalid command");
			continue;
		}

		if (SplitString(ToUpper(command), " ")[0] == "--HELP" || SplitString(ToUpper(command), " ")[0] == "-H")
			Help();
		else if (SplitString(ToUpper(command), " ")[0] == "--VERSION" || SplitString(ToUpper(command), " ")[0] == "-V")
		{
			Logo();
			continue;
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--FUNDS")
		{
			console.SystemPrint();
			console.Write("You have: ");
			console.WriteLine("$" + CommaLargeNumberF((double)walletInfo["Funds"]) + " credits\n", console.yellowFGColor, "");
			continue;
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--DIFFICULTY")
		{
			console.SystemPrint();
			console.WriteLine("Calculating difficulty...");
			std::string dif = CalculateDifficulty(walletInfo);
			console.SystemPrint();
			console.Write("The current difficulty looks like: ");
			console.WriteLine(ExtractPaddedChars(dif, '0'), console.redFGColor, "");
			continue;
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--SYNC" || SplitString(ToUpper(command), " ")[0] == "-S")
		{
			if (Sync(p2p, walletInfo) == 0) continue;
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--SYNCBLOCK" || SplitString(ToUpper(command), " ")[0] == "-SB")
		{
			int blockNum;
			try
			{
				blockNum = stoi(SplitString(ToUpper(command), " ")[1]);
				SyncBlock(p2p, blockNum);
			}
			catch (const std::exception& e)
			{
				console.WriteLine("Error syncing. : " + (std::string)e.what(), "", console.redFGColor);
			}
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--SEND" || SplitString(ToUpper(command), " ")[0] == "-SN")
		{
			std::string toAddr;
			float amnt;
			try
			{
				toAddr = SplitString(command, " ")[1];
				amnt = stof(SplitString(ToUpper(command), " ")[2]);

				if (SendFunds(toAddr, amnt) == 0)
				{
					ConnectionError();
					continue;
				}
			}
			catch (const std::exception& e)
			{
				console.WriteLine("Error sending : " + (std::string)e.what(), "", console.redFGColor);
			}
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--VERIFY" || SplitString(ToUpper(command), " ")[0] == "-VF")
		{
			try
			{
				console.BlockCheckerPrint();
				console.WriteLine("Validating blockchain...");

				IsChainValid(p2p, walletInfo);

				console.BlockCheckerPrint();
				console.WriteLine("Done!");
			}
			catch (const std::exception&)
			{
				Sync(p2p, walletInfo);
			}
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--MINE" || SplitString(ToUpper(command), " ")[0] == "-M")
		{
			int iterations = 1;
			if (SplitString(command, " ").size() > 1)
				iterations = stoi(SplitString(command, " ")[1]);

			for (int i = 0; i < iterations; i++)
			{
				IsChainValid(p2p, walletInfo);

				if (GetProgram(walletInfo) == 0)
				{
					ConnectionError();
					continue;
				}

				walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");

				// Create a superblock if the number is 262800 (the number of blocks created in a year)
				if (walletInfo["BlockchainLength"] >= 262800)
				{

					walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
				}

				console.MiningPrint();
				console.WriteLine("Blockchain length: " + std::to_string((int)walletInfo["BlockchainLength"]));

				// Make sure the pending blocks are newer than the confirmed chain height, but not from the future
				std::string path = "./wwwdata/pendingblocks/";
				bool isFirst = true;
				for (const auto& entry : fs::directory_iterator(path)) {
					std::string name = SplitString(entry.path().filename().string(), "block")[1];
					name = SplitString(name, ".dcc")[0];
					// Delete old pending blocks, or ones that are too high
					if (stoi(name) <= walletInfo["BlockchainLength"] || (stoi(name) >= (int)walletInfo["BlockchainLength"] + 2 && isFirst)) {
						fs::remove(entry);
						console.MiningPrint();
						console.WriteLine("Removing unneeded block: " + entry.path().filename().string());
					}
					isFirst = false;
				}

				// If there are no blocks to mine, stop process.
				walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");
				if (walletInfo["PendingLength"] == 0) {
					console.MiningErrorPrint();
					console.WriteLine("No pending blocks found. Wait for transactions on network.");
					break;
				}

				std::ifstream blockFile("./wwwdata/pendingblocks/block" + std::to_string((int)walletInfo["BlockchainLength"] + 1) + ".dccblock");
				std::stringstream blockBuffer;
				blockBuffer << blockFile.rdbuf();
				std::string content = blockBuffer.str();
				blockFile.close();

				json blockJson = json::parse(content);

				std::string dif = CalculateDifficulty(walletInfo);
				console.SystemPrint();
				console.WriteLine("The current difficulty is: " + dif);
				console.WriteLine("Which looks like: " + ExtractPaddedChars(dif, '0'));

				// Add the mine award to the front of transactions before starting to mine,
				// which will verify this as the recipient should it succeed.
				json txDat = json::object({});
				txDat["tx"] = {
						{"fromAddr", "Block Reward"},
						{"toAddr", (std::string)walletInfo["Address"]},
						{"amount", 1},
						{"unlockTime", 10}
				};
				txDat["sec"] = {
						{"signature", ""},
						{"pubKey", keypair[0]},
						{"note", ""}
				};
				blockJson["transactions"].insert(blockJson["transactions"].begin(), txDat);

				if (Mine(blockJson, ((int)walletInfo["BlockchainLength"] + 1), walletInfo) == 0)
				{
					ConnectionError();
					continue;
				}

				walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
				walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");
				if (walletInfo.is_null())
				{
					ConnectionError();
					continue;
				}

				std::cout << "\n\n";
			}
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--MINEANY" || SplitString(ToUpper(command), " ")[0] == "-MA")
		{
			std::string diff = "";
			if (SplitString(command, " ").size() == 3)
				diff = SplitString(command, " ")[2];
			MineAnyBlock(stoi(SplitString(command, " ")[1]), diff);
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--POOL" || SplitString(ToUpper(command), " ")[0] == "-P")
		{
			std::string poolURL = "http://dccpool.us.to:3333";
			if (SplitString(command, " ").size() == 2)
				poolURL = SplitString(command, " ")[1];
			PoolMine(poolURL, walletInfo);
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--SUPERBLOCK" || SplitString(ToUpper(command), " ")[0] == "-SP")
		{
			CreateSuperblock();
		}
		//else if (SplitString(ToUpper(command), " ")[0] == "--CONNECT" || SplitString(ToUpper(command), " ")[0] == "-C")
		//{
		//	if (SplitString(command, " ").size() < 3)
		//		continue;

		//	endpointPort = SplitString(command, " ")[1];
		//	peerPort = SplitString(command, " ")[2];

		//	p2p.StartP2P(endpointAddr, endpointPort, peerPort);
		//	console.NetworkPrint();
		//	console.WriteLine("Closed P2P");
		//}
		else {
			console.ErrorPrint();
			console.WriteLine("Invalid command");
		}
		connectionStatus = 1;
	}
}

// Print the logo art
void Logo()
{
	console.WriteLine(R"V0G0N( ______      ______    ______  
|_   _ `.  .' ___  | .' ___  | 
  | | `. \/ .'   \_|/ .'   \_| 
  | |  | || |       | |        
 _| |_.' /\ `.___.'\\ `.___.'\ 
|______.'  `.____ .' `.____ .' 

DCC, copyright (c) AstroSam (sam-astro) 2021-2023
)V0G0N", console.cyanFGColor, "");
	console.WriteLine("client: " + VERSION, console.cyanFGColor, "");
	console.WriteLine("block: " + BLOCK_VERSION + "\n\n", console.cyanFGColor, "");
}

// Print the help menu
void Help()
{
	console.WriteLine(R"V0G0N(

Usage: miner.exe [options]
		 OR (while in interactive mode)
       Input: [options]
Options:
  -h, --help                          Display this help menu
  -v, --version                       Print the current wallet and block version
  -s, --sync                          Manually re-sync blockchain
  -sb, --syncblock                    Manually re-sync a single block on the blockchain
  -m, --mine <amount>                 Mine <amount> number of blocks, defaults to 1 if not specified
  -ma, --mineany <block num> <dif>    (Debug) Mines the block specified by <block num> at the given 
                                          difficulty <dif>
  --funds                             Count and print the funds of the user
  --difficulty                        Calculate the expected block's difficulty
  -sn, --send <addr> <amount>         Sends the <amount> of DCC to a receiving address <addr>
  -sp, --superblock                   Generates a debug superblock to summarize all transactions
  -vf, --verify                       Verify the entire blockchain to make sure all blocks are valid
  -p, --pool <url>                    Start mining at a pool, given by <url>. Default is http://dccpool.us.to:3333

)V0G0N");
}



// Send funds to another address, by first checking if the user has enough funds in the first place,
// then adding the transaction and signature to a pending block
int SendFunds(std::string& toAddress, float amount)
{
	console.DebugPrint();
	console.Write("Sending ");
	console.Write("$" + std::to_string(amount), console.whiteBGColor, console.blackFGColor);
	console.Write(" to ");
	console.Write(toAddress, "", console.greenFGColor);
	console.WriteLine("...");

	console.NetworkPrint();
	console.WriteLine("Syncing blocks...");
	Sync(p2p, walletInfo);

	// If there are no pending blocks, create one
	if (FileCount("./wwwdata/pendingblocks/") == 0) {
		// Load last block to get the hash
		std::ifstream blockFile("./wwwdata/blockchain/block" + std::to_string(FileCount("./wwwdata/blockchain/")) + ".dccblock");
		std::stringstream blockBuffer;
		blockBuffer << blockFile.rdbuf();
		std::string content = blockBuffer.str();
		blockFile.close();
		json lastBlockJson = json::parse(content);


		json blockJson = json();

		blockJson["hash"] = "0000000000000000000000000000000000000000000000000000000000000000";
		blockJson["lastHash"] = (std::string)lastBlockJson["hash"];
		blockJson["nonce"] = "";
		blockJson["time"] = "";
		blockJson["targetDifficulty"] = "";
		blockJson["_version"] = BLOCK_VERSION;
		blockJson["transactions"] = json::array();
		blockJson["id"] = FileCount("./wwwdata/blockchain/") + 1;

		// Save new json data to file into finished blockchain folder
		try
		{
			std::ofstream blockFilew("./wwwdata/pendingblocks/block" + std::to_string(FileCount("./wwwdata/blockchain/") + 1) + ".dccblock");
			if (blockFilew.is_open())
			{
				blockFilew << blockJson.dump();
				blockFilew.close();
			}
		}
		catch (const std::exception& e)
		{
			if (constants::debugPrint == true) {
				std::cerr << e.what() << std::endl;
			}
			return 0;
		}
	}

	walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
	walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");

	while (!IsChainValid(p2p, walletInfo))
	{
		for (auto oldBlock : fs::directory_iterator("./wwwdata/blockchain/"))
		{
			try
			{
				remove(oldBlock.path());
			}
			catch (const std::exception&)
			{
				console.ErrorPrint();
				console.WriteLine("Error removing \"" + oldBlock.path().string() + "\"");
			}
		}
		for (int a = 0; a < walletInfo["BlockchainLength"]; a++)
		{
			if (SyncBlock(p2p, 1 + a) == 0)
			{
				ConnectionError();
				break;
			}
		}
	}

	// Check if user even has enough funds for the transaction
	if ((float)walletInfo["Funds"] < amount) {
		console.MiningErrorPrint();
		console.WriteLine("Not enough funds", "", console.redFGColor);
		return 0;
	}


	try
	{
		// Read contents of last pending block
		std::ifstream blkData("./wwwdata/pendingblocks/block" + std::to_string((int)walletInfo["BlockchainLength"] + (int)walletInfo["PendingLength"]) + ".dccblock");
		std::stringstream bufferd;
		bufferd << blkData.rdbuf();
		std::string blockText = bufferd.str();
		if (constants::debugPrint == true) {
			std::cout << "read from: " << ("./wwwdata/pendingblocks/block" + std::to_string((int)walletInfo["BlockchainLength"] + (int)walletInfo["PendingLength"]) + ".dccblock") << std::endl;
			std::cout << "textread: " << blockText << std::endl;
		}
		json blockJson = json::parse(blockText);


		// Get current unix time in seconds
		uint64_t sec = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		// Hash transaction data
		char sha256OutBuffer[65];
		json txDat = json::object({});
		txDat["tx"] = {
				{"fromAddr", (std::string)walletInfo["Address"]},
				{"toAddr", toAddress},
				{"amount", amount},
				{"unlockTime", 10}
		};
		txDat["sec"] = {
				{"signature", ""},
				{"pubKey", keypair[0]},
				{"note", ""}
		};

		std::cout << std::setw(4) << txDat << std::endl;

		sha256_string((char*)(txDat["tx"].dump()).c_str(), sha256OutBuffer);
		std::string hash = sha256OutBuffer;

		// Generate signature by encrypting hash with private key
		std::string signature = rsa_pri_encrypt(hash, keypair[1]);

		std::string sigBase64 = encode64((const unsigned char*)signature.c_str(), signature.length());

		txDat["sec"]["signature"] = sigBase64;

		// Append transaction to list
		blockJson["transactions"].push_back(txDat);

		// Save new json data to file
		try
		{
			std::ofstream blockFile("./wwwdata/pendingblocks/block" + std::to_string((int)walletInfo["BlockchainLength"] + (int)walletInfo["PendingLength"]) + ".dccblock");
			if (blockFile.is_open())
			{
				blockFile << blockJson.dump();
				blockFile.close();
			}
		}
		catch (const std::exception& e)
		{
			if (constants::debugPrint == true) {
				std::cerr << e.what() << std::endl;
				std::cerr << "line 1350" << std::endl;
			}
			return 0;
		}


		console.DebugPrint();
		console.Write("Success", console.whiteBGColor, console.blackFGColor);
		console.Write("\n");

		return 1;
	}
	catch (const std::exception& e)
	{
		if (constants::debugPrint == true) {
			std::cerr << e.what() << std::endl;
			std::cerr << "line 1365" << std::endl;
		}
		return 0;
	}
}