// dcc_client.cpp

#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif

#include "Main.h"

#if defined(__unix__)

#include <curses.h>
#define ISKEYDOWN(X) (getch() == X)

#else

#define ISKEYDOWN(X) GetAsyncKeyState(X)

#endif




using json = nlohmann::json;
namespace fs = std::filesystem;


void Help();
void Version();
void Logo();
int SendFunds(P2P& p2p, std::string& toAddress, float amount);




int connectionStatus = 1;
const std::string directoryList[] = { "./sec", "./wwwdata", "./wwwdata/blockchain", "./wwwdata/pendingblocks", "./wwwdata/containers", "./wwwdata/deluges", "./wwwdata/developing-containers", "./wwwdata/developing-deluges", "./wwwdata/superchain" };

json walletConfig;
json walletInfo;

std::string endpointAddr = "";
std::string endpointPort = "";
std::string peerPort = "";

unsigned long long flops = 0;

//int transactionNumber = 0;

//Console console;

struct stat info;

std::vector<std::string> keypair = { "", "" };

P2P p2p;

int main()
{
	Logo();
	srand(time(0));


	console::SystemPrint();
	console::WriteLine("Benchmarking... ");
	flops = benchmark();
	std::cout << "                                                          \r";
	console::SystemPrint();
	console::Write("Benchmark results: ");
	console::Write(truncateNum(flops) + "Flops", console::cyanFGColor);
	console::Write(" across ");
	console::Write(std::to_string(processor_count) + " cores", console::cyanFGColor);
	console::WriteLine(" ok", console::greenFGColor);

	if (WalletSettingValues::verbose >= 6) {
		console::WriteLine("hextest: ");
		console::WriteLine("\"" + divideHexByFloat("ffffff", 1.3) + "\"");
		console::WriteLine("\"" + divideHexByFloat("0f0", 2) + "\"");
		console::WriteLine("\"" + divideHexByFloat("0fff0", 2) + "\"");
		//console::WriteLine("\"" + multiplyHexByFloat("ff00", 2) + "\"");
	}

	// Create required directories if they don't exist
	for (std::string dir : directoryList)
		if (!fs::is_directory(dir) || !fs::exists(dir)) {
			console::SystemPrint();
			console::WriteLine("Creating " + dir);
			fs::create_directory(dir);
		}

	// Get public IP address
	console::NetworkPrint();
	console::Write("Getting public IP address...");
	//Http http;
	std::string ipStr;
	try{
		ipStr = DownloadFileAsString(serverURL+"/ipget.php"); // use custom server for getting IP:PORT
	}
	catch(...){
		console::WriteLine(" failed", console::redFGColor);
		//ERRORMSG("Connection to server timed out\n");
		console::ExitError("Connection to server timed out");
	}
	//std::string ipStr = http.StartHttpWebRequest("https://api.ipify.org", args); // This is a free API that lets you get IP 
	//console::WriteLine(ipStr);
	console::WriteLine(" ok", console::greenFGColor);

	bool permanentPort = false;

	
	// Create config.cfg file if it doesn't exist 
	console::SystemPrint();
	console::Write("Checking config.cfg");
	if (!fs::exists("./config.cfg"))
	{
		//int prt;
		//console::ErrorPrint();
		//console::Write("Config file not found. \nPlease input the port # you want to use \n(default 5060): ");
		//std::cin >> prt;
		//if (prt <= 0 || prt > 65535)
		//	prt = 5060;
		console::Write(" creating.", console::yellowFGColor);
	
		std::ofstream configFile("./config.cfg");
		if (configFile.is_open())
		{
			configFile
				<< "{\"port\":" << SplitString(ipStr, ":")[1]
				<< ",\"ip\":\"" << SplitString(ipStr, ":")[0]
				<< "\",\"permanentPort\":false,\"keepAlive\":false,\"isServer\":false,\"verbosity\":-1}";
			configFile.close();
		}

		console::WriteLine(" ok", console::greenFGColor);
		/*walletConfig["ip"] = SplitString(ipStr, ":")[0];
			walletConfig["port"] = SplitString(ipStr, ":")[1];*/
	}
	else {
		std::ifstream conf("./config.cfg");
		if (conf.is_open()) {
			std::stringstream confbuf;
			confbuf << conf.rdbuf();
			walletConfig = json::parse(confbuf.str());
			conf.close();
			console::WriteLine(" ok", console::greenFGColor);
		}
		if (walletConfig["permanentPort"] == false) {
			walletConfig["port"] = stoi(SplitString(ipStr, ":")[1]);
			walletConfig["ip"] = SplitString(ipStr, ":")[0];
			std::ofstream configFile("./config.cfg");
			if (configFile.is_open())
			{
				configFile << walletConfig.dump();
				configFile.close();
			}
		}
	}


	// Generate and save keypair if it doesn't exist
	console::SystemPrint();
	console::Write("Checking keypairs...");
	if (!fs::exists("./sec/prikey.pem"))
	{
		console::WriteLine(" not found", console::redFGColor);
		console::SystemPrint();
		console::WriteLine("None found, generating keypairs...");
		keypair = GenerateKeypair();
		std::cout << "Public key: " << std::endl;
		std::cout << keypair[0] << std::endl;
		std::cout << "Private key: " << std::endl;
		std::cout << keypair[1] << std::endl;
	}
	else{
		console::WriteLine(" ok", console::greenFGColor);
	}
	// Load public key as keypair[0]
	std::ifstream pkey("./sec/pubkey.pem");
	if (pkey.is_open()) {
		std::stringstream keybuf;
		keybuf << pkey.rdbuf();
		keypair[0] = keybuf.str();
		pkey.close();
	}
	// Load private key as keypair[1]
	std::ifstream skey("./sec/prikey.pem");
	if (skey.is_open()) {
		std::stringstream skeybuf;
		skeybuf << skey.rdbuf();
		keypair[1] = skeybuf.str();
		skey.close();
	}

	// Calculate wallet from hash of public key
	char walletBuffer[65];
	sha256_string((char*)(keypair[0]).c_str(), walletBuffer);
	std::string wallet = walletBuffer;

	console::SystemPrint();
	console::Write("Your wallet: ");
	console::WriteLine(wallet, console::cyanFGColor, "");
	walletInfo["Address"] = wallet;

	// Get all deluge files, and ensure they are complete
	//walletInfo["FullPrograms"] = json::array();
	for (auto deluge : fs::directory_iterator("./wwwdata/deluges/"))
	{
		std::ifstream delugeFile(deluge.path());
		if (delugeFile.is_open()) {
			std::stringstream delugeFilebuf;
			delugeFilebuf << delugeFile.rdbuf();
			json delugeJson = json::parse(delugeFilebuf.str());
			delugeFile.close();

			// Verify the deluge, by checking each chunk with its expected hash, and then the full hash
			std::string delugePath = "./wwwdata/containers/" + ((std::string)delugeJson["_totalHash"]).substr(0, 32) + ".tar.zip";


			if(VerifyDeluge(delugeJson, delugePath)){
				for(int hs = 0; hs < delugeJson["hashList"].size(); hs++){
					p2p.completeDelugeList[(std::string)delugeJson["_totalHash"]][delugeJson["hashList"][hs]] = hs;
				}
				//// Add deluge full hash to list with it's path as a value
				//completeDelugeList[(std::string)delugeJson["_totalHash"]] = delugeJson["_hashList"];
			}
			// If the deluge is invalid, remove it's file
			else {
				try {
					remove(deluge.path());
				}
				catch (...) {}
			}
		}
	}

	p2p.InitPeerList();


	// Load the wallet config file and get the P2P port and IP
	std::ifstream conf("./config.cfg");
	if (conf.is_open()) {
		std::stringstream confbuf;
		confbuf << conf.rdbuf();
		walletConfig = json::parse(confbuf.str());
		conf.close();
	}

	endpointAddr = (std::string)walletConfig["ip"];
	endpointPort = std::to_string((int)walletConfig["port"]);

	if((int)walletConfig["verbosity"] != -1)
		WalletSettingValues::verbose = (int)walletConfig["verbosity"];

	console::SystemPrint();
	console::Write("Client endpoint: ");
	console::WriteLine((std::string)walletConfig["ip"] + ":" + std::to_string((int)walletConfig["port"]), console::cyanFGColor);

	if (walletConfig["permanentPort"]) {
		console::WriteIndented("(The port is set permanently in the config file to ", console::yellowFGColor, "", 3);
		console::Write(std::to_string((int)walletConfig["port"]), console::blueFGColor, "");
		console::Write(")\n", console::yellowFGColor, "");
		console::WriteIndented("Make sure this port is opened in your firewall, or the\n", console::yellowFGColor, "", 3);
		console::WriteIndented("system will not be able to communicate with peers.\n", console::yellowFGColor, "", 3);
	}

	// Open the socket required to accept P2P requests and send responses
	p2p.OpenP2PSocket((int)walletConfig["port"]);
	p2p.keepPeersAlive = (bool)walletConfig["keepAlive"];
	p2p.isServer = (bool)walletConfig["isServer"];
	// Start the P2P listener thread
	std::thread t1(&P2P::ListenerThread, &p2p, 10);
	// Start the P2P sender thread
	std::thread t2(&P2P::SenderThread, &p2p);

	//
	// Gather wallet information, validate blockchain, and print information.
	//	

	console::NetworkPrint();
	console::Write("Syncing blocks...");
	Sync(p2p, walletInfo);
	console::WriteLine(" ok", console::greenFGColor);
	try
	{
		IsChainValid(p2p, walletInfo);
	}
	catch (const std::exception&)
	{
		Sync(p2p, walletInfo);
	}

	console::SystemPrint();
	console::Write("You have: ");
	console::WriteLine("$" + CommaLargeNumberF((double)walletInfo["Funds"]) + " credits\n", console::yellowFGColor, "");
	/*console::Write("Your address is:\n");
	console::WriteLine((std::string)walletInfo["Address"], console::g, "");*/


	//walletInfo["Funds"] = 0.0f;
	walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
	walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");

	console::MiningPrint();
	console::Write("There are ");
	console::Write(std::to_string((int)walletInfo["PendingLength"]), console::greenFGColor, "");
	console::WriteLine(" Block(s) to compute");

	console::SystemPrint();
	console::WriteLine("Calculating difficulty...");
	std::string dif = CalculateDifficulty(walletInfo);
	console::SystemPrint();
	console::Write("The current difficulty looks like: ");
	console::WriteLine(ExtractPaddedChars(dif, '0'), console::redFGColor, "");

	//console::Write("scanning ports...");
	//ScanAllPorts("74.78.145.2");


	AnnounceToPeers(p2p);
	//
	// Start command loop
	//
	while (true)
	{
		console::Write("Input:  ");
		std::string command = console::ReadLine();

		if (command == "") {
			console::ErrorPrint();
			console::WriteLine("Invalid command");
			continue;
		}

		// Allow for no -- or - prefix
		if (!StringStartsWith(command, "-")) {
			if (command.size() <= 2)
				command = "-" + command;
			else
				command = "--" + command;
		}

		// Get parts split by space character
		std::vector<std::string> commandParts = SplitString(command, " ");

		// Make (only first cmd part) uppercase
		commandParts[0] = ToUpper(commandParts[0]);

		try{
			if (commandParts[0] == "--HELP" || commandParts[0] == "-H")
				Help();
			else if (commandParts[0] == "--VERSION" || commandParts[0] == "-V")
			{
				Version();
				continue;
			}
			else if (commandParts[0] == "--QUIT" || commandParts[0] == "--EXIT")
			{
				exit(0);
			}
			else if (commandParts[0] == "--FUNDS")
			{
				console::SystemPrint();
				console::Write("You have: ");
				console::WriteLine("$" + CommaLargeNumberF((double)walletInfo["Funds"]) + " credits\n", console::yellowFGColor, "");
				continue;
			}
			else if (commandParts[0] == "--DIFFICULTY")
			{
				console::SystemPrint();
				console::WriteLine("Calculating difficulty...");
				std::string dif = CalculateDifficulty(walletInfo);
				console::SystemPrint();
				console::Write("The current difficulty looks like: ");
				console::WriteLine(ExtractPaddedChars(dif, '0'), console::redFGColor, "");
				continue;
			}
			else if (commandParts[0] == "--SYNC" || commandParts[0] == "-S")
			{
				if (Sync(p2p, walletInfo) == 0) continue;
			}
			else if (commandParts[0] == "--SYNCBLOCK" || commandParts[0] == "-SB")
			{
				int blockNum = 0;
				try
				{
					blockNum = stoi(commandParts[1]);
					console::NetworkPrint();
					console::WriteLine("Syncing block " + std::to_string(blockNum));
					SyncBlock(p2p, blockNum, true);
				}
				catch (const std::exception& e)
				{
					console::NetworkErrorPrint();
					console::WriteLine("Error syncing. : " + (std::string)e.what(), "", console::redFGColor);
				}
			}
			else if (commandParts[0] == "--SEND" || commandParts[0] == "-SN")
			{
				std::string toAddr;
				float amnt;
				try
				{
					toAddr = commandParts[1];
					amnt = stof(commandParts[2]);

					if (SendFunds(p2p, toAddr, amnt) == 0)
					{
						console::ConnectionError();
						continue;
					}
				}
				catch (const std::exception& e)
				{
					console::WriteLine("Error sending : " + (std::string)e.what(), "", console::redFGColor);
				}
			}
			else if (commandParts[0] == "--ADDPEER")
			{
				p2p.peerList.push_back(commandParts[1] + ":0");
				p2p.SavePeerList();
			}
			//else if (commandParts[0] == "--VERBOSITY")
			//{
			//	WalletSettingValues::verbose = std::stoi(commandParts[1]);
			//}
			else if (commandParts[0] == "--VERIFY" || commandParts[0] == "-VF")
			{
				try
				{
					IsChainValid(p2p, walletInfo);
				}
				catch (const std::exception&)
				{
					Sync(p2p, walletInfo);
				}
			}
			else if (commandParts[0] == "--MINE" || commandParts[0] == "-M")
			{
				int iterations = 1;
				if (commandParts.size() > 1)
					iterations = stoi(commandParts[1]);

				for (int i = 0; i < iterations; i++)
				{
					IsChainValid(p2p, walletInfo);

					if (GetProgram(p2p, walletInfo) != 0)
					{
						//console::ConnectionError();
						ERRORMSG("Failed to get deluge program");
						continue;
					}

					walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");

					// Create a superblock if the number is 262800 (the number of blocks created in a year)
					if (walletInfo["BlockchainLength"] >= 262800)
					{

						walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
					}

					if (WalletSettingValues::verbose >= 1) {
						console::MiningPrint();
						console::WriteLine("Blockchain length: " + std::to_string((int)walletInfo["BlockchainLength"]));
					}

					// Make sure the pending blocks are newer than the confirmed chain height, but not from the future
					std::string path = "./wwwdata/pendingblocks/";
					bool isFirst = true;
					for (const auto& entry : fs::directory_iterator(path)) {
						std::string name = SplitString(entry.path().filename().string(), "block")[1];
						name = SplitString(name, ".dcc")[0];
						// Delete old pending blocks, or ones that are too high
						if (stoi(name) <= walletInfo["BlockchainLength"] || (stoi(name) >= (int)walletInfo["BlockchainLength"] + 2 && isFirst)) {
							fs::remove(entry);
							console::MiningPrint();
							console::WriteLine("Removing unneeded block: " + entry.path().filename().string());
						}
						isFirst = false;
					}

					// If there are no blocks to mine, stop process.
					walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");
					if (walletInfo["PendingLength"] == 0) {
						console::MiningErrorPrint();
						console::WriteLine("No pending blocks found. Wait for transactions on network.");
						break;
					}

					std::ifstream blockFile("./wwwdata/pendingblocks/block" + std::to_string((int)walletInfo["BlockchainLength"] + 1) + ".dccblock");
					std::stringstream blockBuffer;
					blockBuffer << blockFile.rdbuf();
					std::string content = blockBuffer.str();
					blockFile.close();

					json blockJson = json::parse(content);

					std::string dif = CalculateDifficulty(walletInfo);
					if (WalletSettingValues::verbose >= 1) {
						console::SystemPrint();
						console::WriteLine("The current difficulty is: " + dif);
						console::WriteLine("Which looks like: " + ExtractPaddedChars(dif, '0'));
					}

					// Add the mine award to the front of transactions before starting to mine,
					// which will verify this as the recipient should it succeed.
					json txDat = json::object({});
					txDat["tx"] = {
							{"fromAddr", "Block Reward"},
							{"toAddr", (std::string)walletInfo["Address"]},
							{"amount", 1},
							{"unlockTime", 10},
							{"transactionFee", 0.01}
					};
					txDat["sec"] = {
							{"signature", ""},
							{"pubKey", keypair[0]},
							{"note", ""}
					};
					blockJson["transactions"].insert(blockJson["transactions"].begin(), txDat);

					int returnVal = Mine(blockJson, ((int)walletInfo["BlockchainLength"] + 1), walletInfo);
					if (returnVal == 0)
					{
						console::ConnectionError();
						continue;
					}
					// End early
					else if (returnVal == 2) {
						std::cout << "\n\n";
						//getch();
						break;
					}
					// Pause
					else if (returnVal == 3) {
						console::Write("\nPaused, press");
						console::Write(" R ", console::greenFGColor, "");
						console::WriteLine("to resume");
						while (!ISKEYDOWN(0x52));
						//while (!GetAsyncKeyState(0x52));
						//GETKEY();
					}

					walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
					walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");
					if (walletInfo.is_null())
					{
						console::ConnectionError();
						continue;
					}

					std::cout << "\n\n";
				}
			}
			else if (commandParts[0] == "--VERBOSITY" || commandParts[0] == "-VB")
			{
				if (commandParts.size() == 2){
					WalletSettingValues::verbose = stoi(commandParts[1]);
					console::WriteLine("Changed verbosity to ("+std::to_string(WalletSettingValues::verbose)+")");
				}
			}
			else if (commandParts[0] == "--MINEANY" || commandParts[0] == "-MA")
			{
				std::string diff = "";
				if (commandParts.size() == 3)
					diff = commandParts[2];
				MineAnyBlock(stoi(commandParts[1]), diff);
			}
			else if (commandParts[0] == "--POOL" || commandParts[0] == "-P")
			{
				std::string poolURL = serverURL;
				if (commandParts.size() == 2)
					poolURL = commandParts[1];
				PoolMine(poolURL, walletInfo);
			}
			else if (commandParts[0] == "--SUPERBLOCK" || commandParts[0] == "-SP")
			{
				CreateSuperblock();
			}
			else if (commandParts[0] == "--MAKE-CONTAINER" || commandParts[0] == "-MK")
			{
				if (commandParts.size() == 2) {
					MakeProgram(walletInfo, walletConfig, commandParts[1]);
				}
			}
			else if (commandParts[0] == "--LIST-CONTAINERS" || commandParts[0] == "-LS")
			{
				console::WriteLine();
				const std::string delugeDirectories[2] = { "./wwwdata/deluges/", "./wwwdata/developing-deluges/" };
				for (std::string delugeDir : delugeDirectories) {
					console::Write("\nDeluges in directory: ");
					console::WriteLine("\"" + delugeDir + "\"", console::yellowFGColor);

					// Make table
					console::WriteIndented("+---------------------------------+-------------------------+--------+\n", "", "", 1);

					// Colored headers
					console::WriteIndented("| ", "", "", 1);
					console::Write("Name", console::cyanFGColor);
					console::Write("                            |");
					console::Write(" ID", console::cyanFGColor, "");
					console::Write("                      |");
					console::Write(" Peers", console::cyanFGColor, "");
					console::Write("  |");
					console::WriteLine();

					console::WriteIndented("+---------------------------------+-------------------------+--------+\n", "", "", 1);
					for (auto deluge : fs::directory_iterator(delugeDir))
					{
						std::ifstream delugeFile(deluge.path());
						if (delugeFile.is_open()) {
							std::stringstream delugeFilebuf;
							delugeFilebuf << delugeFile.rdbuf();
							json delugeJson = json::parse(delugeFilebuf.str());
							delugeFile.close();

							console::WriteIndented("| " + PadStringRight((std::string)delugeJson["_name"], ' ', 32) + "| ", "", "", 1);
							console::Write(((std::string)delugeJson["_totalHash"]).substr(0, 20) + "... |");
							console::Write(PadString(std::to_string(delugeJson["peers"].size()), ' ', 7) + " |");
							console::WriteLine();

							//// Verify the deluge, by checking each chunk with its expected hash, and then the full hash
							//std::string delugePath = "./wwwdata/containers/" + ((std::string)delugeJson["_totalHash"]).substr(0, 32) + ".tar.zip";
							//if(VerifyDeluge(delugeJson, delugePath)){
							//	// Add deluge full hash to list with it's path as a value
							//	//completeDelugeList[(std::string)delugeJson["_totalHash"]] = deluge.path();
							//}
							//// If the deluge is invalid, remove it's file
							//else{
							//	try{
							//		remove(deluge.path());
							//	}
							//	catch(...){}
							//}
						}
					}
					console::WriteIndented("+---------------------------------+-------------------------+--------+\n", "", "", 1);
				}
			}
			else if (commandParts[0] == "--LIST-PEERS" || commandParts[0] == "-LP")
			{
				console::WriteLine();
				console::Write("\nPeers: ");

				// Make table
				console::WriteIndented("+-----------------+-------+------------+-------+\n", "", "", 1);

				// Colored headers
				console::WriteIndented("| ", "", "", 1);
				console::Write("IP", console::cyanFGColor);
				console::Write("              |");
				console::Write(" PORT", console::cyanFGColor, "");
				console::Write("  |");
				console::Write(" Status", console::cyanFGColor, "");
				console::Write("     |");
				console::Write(" Tries", console::cyanFGColor, "");
				console::Write(" |");
				console::WriteLine();

				console::WriteIndented("+-----------------+-------+------------+-------+\n", "", "", 1);
				for(std::string peer : p2p.peerList)
				{
						console::WriteIndented("| " + PadStringRight(SplitString(peer, ":")[0], ' ', 16) + "| ", "", "", 1);
						console::Write(PadStringRight(SplitString(peer, ":")[1], ' ', 5) + " | ");
						int statusNum = stoi(SplitString(peer, ":")[2]);
						if (statusNum <= 1)
							console::Write(PadStringRight("online", ' ', 10), console::greenFGColor);
						else if (statusNum <= 5)
							console::Write(PadStringRight("stalling", ' ', 10), console::yellowFGColor);
						else if (statusNum <= 9)
							console::Write(PadStringRight("offline", ' ', 10), console::redFGColor);
						console::Write(" | ");

						console::Write(PadStringRight(SplitString(peer, ":")[2], ' ', 5) + " | ");

						console::WriteLine();

				}
				console::WriteIndented("+-----------------+-------+------------+-------+\n", "", "", 1);
			}

			//else if (commandParts[0] == "--CONNECT" || commandParts[0] == "-C")
			//{
			//	if (commandParts.size() < 3)
			//		continue;

			//	endpointPort = commandParts[1];
			//	peerPort = commandParts[2];

			//	p2p.StartP2P(endpointAddr, endpointPort, peerPort);
			//	console::NetworkPrint();
			//	console::WriteLine("Closed P2P");
			//}
			else {
				console::ErrorPrint();
				console::WriteLine("Invalid command");
			}
		}
		catch(...){
			console::ErrorPrint();
			console::WriteLine("Invalid command");
		}
		connectionStatus = 1;
	}
}

// Print the client and block versions
void Version()
{
	console::Write("client: " + VERSION, console::cyanFGColor, "");
	console::WriteLine(PLATFORMSTR, PLATFORMCOLOR, "");
	console::WriteLine("block:  " + BLOCK_VERSION, console::cyanFGColor, "");
	console::WriteLine("deluge: " + DELUGE_VERSION, console::cyanFGColor, "");
	console::WriteLine("\n");
}

// Print the logo art
void Logo()
{
	console::WriteLine(R"V0G0N( ______      ______    ______  
|_   _ `.  .' ___  | .' ___  | 
  | | `. \/ .'   \_|/ .'   \_| 
  | |  | || |       | |        
 _| |_.' /\ `.___.'\\ `.___.'\ 
|______.'  `.____ .' `.____ .' 

DCC, copyright (c) AstroSam (sam-astro) 
   The-Distributed-Computing-Project 
   2021-2024        
)V0G0N", console::cyanFGColor, "");
	Version();
}

// Print the help menu
void Help()
{
	console::WriteLine(R"V0G0N(

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
  -p, --pool <url>                    Start mining at a pool, given by <url>. Default is http://dccpool.us.to
  -mk, --make-container <path>        Create a new Deluge for sharing a container (this won't publish it yet)

)V0G0N");
}



// Send funds to another address, by first checking if the user has enough funds in the first place,
// then adding the transaction and signature to a pending block
int SendFunds(P2P& p2p, std::string& toAddress, float amount)
{
	console::DebugPrint();
	console::Write("Sending ");
	console::Write("$" + std::to_string(amount), console::whiteBGColor, console::blackFGColor);
	console::Write(" to ");
	console::Write(toAddress, "", console::greenFGColor);
	console::WriteLine("...");

	console::NetworkPrint();
	console::WriteLine("Syncing blocks...");
	Sync(p2p, walletInfo);


	// Make sure chain is completely valid
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
				console::ErrorPrint();
				console::WriteLine("Error removing \"" + oldBlock.path().string() + "\"");
			}
		}
		for (int a = 0; a < walletInfo["BlockchainLength"]; a++)
		{
			if (SyncBlock(p2p, 1 + a) == 0)
			{
				console::ConnectionError();
				break;
			}
		}
	}

	// Check if user even has enough funds for the transaction
	if ((float)walletInfo["Funds"] < amount) {
		console::MiningErrorPrint();
		console::WriteLine("Not enough funds", "", console::redFGColor);
		return 0;
	}

	// If the transaction appears possible, create it:

	// Hash transaction data
	char sha256OutBuffer[65];
	json txDat = json::object({});
	txDat["tx"] = {
			{"fromAddr", (std::string)walletInfo["Address"]},
			{"toAddr", toAddress},
			{"amount", amount},
			{"unlockTime", 10},
			{"transactionFee", 0.01}
	};
	txDat["sec"] = {
			{"signature", ""},
			{"pubKey", keypair[0]},
			{"note", ""}
	};

	console::WriteLine();
	console::NetworkPrint();
	console::WriteLine("Transaction info for your reference:");
	std::cout << std::setw(4) << txDat << std::endl;

	sha256_string((char*)(txDat["tx"].dump()).c_str(), sha256OutBuffer);
	std::string hash = sha256OutBuffer;

	// Generate signature by encrypting hash with private key
	std::string signature = rsa_pri_encrypt(hash, keypair[1]);
	std::string sigBase64 = encode64((const unsigned char*)signature.c_str(), signature.length());

	txDat["sec"]["signature"] = sigBase64;


	// Now announce the transaction to peers
	p2p.messageStatus = p2p.requesting_transaction_process;
	p2p.messageAttempt = 0;
	p2p.reqDat = 0;
	p2p.extraData = ReplaceEscapeSymbols(txDat.dump());

	while (p2p.isAwaiting()) {}

	return 1;
}
