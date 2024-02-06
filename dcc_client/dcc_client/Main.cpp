// dcc_client.cpp

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
void Version();
void Logo();
int SendFunds(P2P& p2p, std::string& toAddress, float amount);




int connectionStatus = 1;
const std::string directoryList[] = { "./sec", "./wwwdata", "./wwwdata/blockchain", "./wwwdata/pendingblocks", "taskcontatiners", "./wwwdata/taskdeluges", "./wwwdata/superchain", "./wwwdata/developing-programs" };

json walletConfig;
json walletInfo;

std::string endpointAddr = "";
std::string endpointPort = "";
std::string peerPort = "";

//int transactionNumber = 0;

//Console console;

struct stat info;

std::vector<std::string> keypair = { "", "" };

P2P p2p;

int main()
{
	Logo();
	srand(time(0));


	if (WalletSettingValues::verbose >= 3) {
		console::WriteLine("hextest: ");
		console::WriteLine("\"" + divideHexByFloat("ffffff", 1.3) + "\"");
		console::WriteLine("\"" + divideHexByFloat("0f0", 2) + "\"");
		console::WriteLine("\"" + divideHexByFloat("0fff0", 2) + "\"");
		console::WriteLine("\"" + multiplyHexByFloat("ff00", 2) + "\"");
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
	console::WriteLine("Getting public IP address...");
	Http http;
	std::vector<std::string> args;
	std::string ipStr = http.StartHttpWebRequest("http://dccpool.us.to/dcc/ipget.php", args); // use custom server for getting IP:PORT
	//std::string ipStr = http.StartHttpWebRequest("https://api.ipify.org", args); // This is a free API that lets you get IP 
	//console::WriteLine(ipStr);
	console::NetworkPrint();
	console::WriteLine("Done.");

	// Create config.cfg file if it doesn't exist 
	console::SystemPrint();
	console::WriteLine("Checking config.cfg");
	//if (!fs::exists("./config.cfg"))
	//{
		//int prt;
		//console::ErrorPrint();
		//console::Write("Config file not found. \nPlease input the port # you want to use \n(default 5000): ");
		//std::cin >> prt;
		//if (prt <= 0 || prt > 65535)
		//	prt = 5000;

		std::ofstream configFile("./config.cfg");
		if (configFile.is_open())
		{
			configFile << "{\"port\":" << SplitString(ipStr, ":")[1] << ",\"ip\":\"" << SplitString(ipStr, ":")[0] << "\"}";
			configFile.close();
		}
		/*walletConfig["ip"] = SplitString(ipStr, ":")[0];
			walletConfig["port"] = SplitString(ipStr, ":")[1];*/
	//}

	// Generate and save keypair if it doesn't exist
	console::SystemPrint();
	console::WriteLine("Checking keypairs...");
	if (!fs::exists("./sec/prikey.pem"))
	{
		console::SystemPrint();
		console::WriteLine("None found, generating keypairs...");
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

	console::SystemPrint();
	console::Write("Your wallet: ");
	console::WriteLine(wallet, console::greenFGColor, "");
	walletInfo["Address"] = wallet;

	// Get all deluge files, and ensure they are complete
	//walletInfo["FullPrograms"] = json::array();
	for (auto deluge : fs::directory_iterator("./wwwdata/taskdeluges/"))
	{
		std::ifstream delugeFile(deluge.path());
		if (delugeFile.is_open()) {
			std::stringstream delugeFilebuf;
			delugeFilebuf << delugeFile.rdbuf();
			json delugeJson = json::parse(delugeFilebuf.str());
			delugeFile.close();

			// Verify the deluge, by checking each chunk with its expected hash, and then the full hash
			if(VerifyDeluge(delugeJson, "./wwwdata/taskcontainers/" + (std::string)delugeJson["_totalHash"].subtr(0,32)+".img")){
				// Add deluge full hash to list with it's path as a value
				delugeList[(std::string)delugeJson["_totalHash"]] = deluge.path();
			}
			// If the deluge is invalid, remove it's file
			else{
				try{
					remove(deluge.path());
				}
				catch(...){}
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

	console::SystemPrint();
	console::WriteLine("Client endpoint: " + (std::string)walletConfig["ip"] + ":" + std::to_string((int)walletConfig["port"]));


	// Open the socket required to accept P2P requests and send responses
	p2p.OpenP2PSocket((int)walletConfig["port"]);
	// Start the P2P listener thread
	std::thread t1(&P2P::ListenerThread, &p2p, 10);
	// Start the P2P sender thread
	std::thread t2(&P2P::SenderThread, &p2p);


	//
	// Gather wallet information, validate blockchain, and print information.
	//	

	console::SystemPrint();
	console::WriteLine("Getting wallet info...");

	console::NetworkPrint();
	console::WriteLine("Syncing blocks...");
	Sync(p2p, walletInfo);
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

		// Make uppercase
		command = ToUpper(command);

		// Get parts split by space character
		std::vector<std::string> commandParts = SplitString(command, " ");

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
		}
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

				if (GetProgram(walletInfo) == 0)
				{
					console::ConnectionError();
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
					while (!GetAsyncKeyState(0x52));
					getch();
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
		else if (commandParts[0] == "--MINEANY" || commandParts[0] == "-MA")
		{
			std::string diff = "";
			if (commandParts.size() == 3)
				diff = commandParts[2];
			MineAnyBlock(stoi(commandParts[1]), diff);
		}
		else if (commandParts[0] == "--POOL" || commandParts[0] == "-P")
		{
			std::string poolURL = "http://dccpool.us.to";
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
		connectionStatus = 1;
	}
}

// Print the client and block versions
void Version()
{
	console::WriteLine("client: " + VERSION, console::cyanFGColor, "");
	console::WriteLine("block: " + BLOCK_VERSION + "\n\n", console::cyanFGColor, "");
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

DCC, copyright (c) AstroSam (sam-astro) 2021-2024
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
