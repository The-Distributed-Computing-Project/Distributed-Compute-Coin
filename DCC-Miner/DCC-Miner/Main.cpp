// DCC-Miner.cpp

#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif

#include "Main.h"

std::string serverURL = "http://api.achillium.us.to";

using json = nlohmann::json;
namespace fs = std::filesystem;

//struct Block
//{
//public:
//	string Version;
//	string LastHash;
//	string Hash;
//	string Nonce;
//	string Time;
//	vector<string> Transactions;
//	vector<string> TransactionTimes;
//
//	void Upgrade(string toVersion)
//	{
//		BlockchainUpgrader().Upgrade(*this, toVersion);
//	}
//};
//
//struct WalletInfo
//{
//public:
//	string Address;
//	double Balance;
//	double PendingBalance;
//	int BlockchainLength;
//	int PendingLength;
//	string MineDifficulty;
//	float CostPerMinute;
//};
//
//struct ProgramConfig
//{
//public:
//	string Zip;
//	double TotalMinutes;
//	string Author;
//	double MinutesLeft;
//	int ComputationLevel;
//	double Cost;
//	bool Built;
//};


void Help();
json GetInfo();
json ReadProgramConfig();
json UpgradeBlock(json b, std::string toVersion);
int WriteProgramConfig();
int GetProgram();
int Sync();
int SyncPending(int whichBlock);
int SyncBlock(int whichBlock);
int Mine(std::string lastHash, std::string transactionHistory, int blockNum);
int MineAnyBlock(int blockNum, std::string difficulty);
float GetProgramLifeLeft(std::string id);
double round(float value, int decimal_places);
bool IsChainValid();
void ConnectionError();
std::string ExecuteCommand(const char* cmd);
std::string FormatHPS(float input);
boost::process::child ExecuteAsync(std::string cmd, bool print);
int SendFunds(std::string toAddress, float amount);


template <
	class result_t = std::chrono::milliseconds,
	class clock_t = std::chrono::steady_clock,
	class duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start)
{
	return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}



std::string id = "";
int connectionStatus = 1;
const std::string directoryList[] = { "./sec", "./wwwdata", "./wwwdata/blockchain", "./wwwdata/pendingblocks", "./wwwdata/programs" };

json programConfig;
json walletConfig;
json walletInfo;

const std::string VERSION = "v0.01alpha";
const std::string blockVersion = "v0.01alpha-coin";

std::string endpointAddr = "";
std::string endpointPort = "";
std::string peerPort = "";


struct stat info;
Console console;
P2P p2p;

std::vector<std::string> keypair = { "", "" };


int main()
{
	console.DebugPrint();
	console.WriteLine("Started");

	Logo();

	char sha256OutBuffer[65];
	unsigned char hash[32];
	char* input = (char*)"hello there";
	sha256_full_cstr(input, hash);
	cstr_to_hexstr(hash, 32, sha256OutBuffer);
	std::cout << "test:    " << sha256OutBuffer << std::endl;

	char sha256OutBuffer2[65];
	char* input2 = (char*)"hello there";
	sha256_string(input2, sha256OutBuffer2);
	std::cout << "correct: " << sha256OutBuffer2 << std::endl;

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
		console.Write("config file not found. \nPlease input the port # you want to use \n(default 5000): ");
		cin >> prt;
		if(prt <= 0 || prt > 65535)
			prt = 5000;
		std::ofstream configFile("./config.cfg");
		if (configFile.is_open())
		{
			configFile << "{\"port\":" << std::to_string(prt) << "}";
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

	char walletBuffer[65];
	sha256_string((char*)(keypair[0]).c_str(), walletBuffer);
	std::string wallet = walletBuffer;

	console.SystemPrint();
	console.Write("Your wallet: ");
	console.WriteLine(wallet, console.greenFGColor, "");
	walletInfo["Address"] = wallet;


	std::string line;
	std::ifstream peerFile("./wwwdata/peerlist.txt");
	// If the peer list file does not exist, create it
	if (!peerFile)
	{
		console.ErrorPrint();
		console.WriteLine("Error opening peer file", console.redFGColor, "");

		// Create the peer list file
		std::ofstream peerFileW("./wwwdata/peerlist.txt");
		if (peerFileW.is_open())
		{
			peerFileW << "";
			peerFileW.close();
		}
		peerFileW.close();
	}
	else
		while (std::getline(peerFile, line))
			peerList.push_back(line);
	peerFile.close();

	//// Encryption Test
	//std::cout << "\nTesting Encryption..." << std::endl;
	//std::string message = "Hello world!";
	//std::cout << "\nMessage: " << message << std::endl;
	//std::string encryptedVersion = rsa_pri_encrypt(message, keypair[1]);
	//std::cout << "\nEncrypted: " << encryptedVersion << std::endl;
	//std::string encodedVersion = encode64((const unsigned char*)encryptedVersion.c_str(), encryptedVersion.length());
	//std::cout << "\nEncoded: " << encodedVersion << std::endl;
	////char sha256OutBufferss[65];
	////sha256_string((char *)encryptedVersion.c_str(), sha256OutBufferss);
	////std::string enHash = sha256OutBufferss;
	////std::cout << "Encrypted hash: " << enHash << std::endl;
	//std::string decoded = decode64(encodedVersion);
	//std::cout << "\nDecoded: " << decoded << std::endl;
	//std::cout << "\nAre the same: " << (decoded == encryptedVersion) << std::endl;
	//std::string decryptedVersion = rsa_pub_decrypt(decoded, keypair[0]);
	//std::cout << "\nDecrypted: " << decryptedVersion << std::endl;



	//// If previous config exists, load it. Else, ask for required information.
	//std::ifstream t("./config.cfg");
	//std::stringstream buffer;
	//buffer << t.rdbuf();
	//std::string configFileRead = buffer.str();
	//if (configFileRead.size() > 4)
	//{
	//	std::string cpy = SplitString(configFileRead, "\n")[0];
	//	boost::trim(cpy);
	//	walletInfo["Address"] = cpy;
	//}
	//else
	//{
	//	console.MiningPrint();
	//	console.Write("Enter your payout wallet : ");
	//	walletInfo["Address"] = console.ReadLine();

	//	console.MiningPrint();
	//	console.Write("Stay logged in? Y/N : ");
	//	std::string stayLoggedIn = console.ReadLine();
	//	
	//	// Save to config.cfg file
	//	if (ToUpper(stayLoggedIn) == "Y")
	//	{
	//		std::ofstream configFile("./config.cfg");
	//		if (configFile.is_open())
	//		{
	//			configFile << walletInfo["Address"];
	//			configFile.close();
	//		}
	//	}
	//}

	//phpserv.StartServer();
	//p2p.Connect("");

	// Get public IP address and PORT
	Http http;
	std::vector<std::string> args;
	std::string ipPortCombo = http.StartHttpWebRequest("https://api.ipify.org", args); // This is a free API that lets you get IP free


	// Load the wallet config file and get the P2P port
	std::ifstream conf("./config.cfg");
	std::stringstream confbuf;
	confbuf << conf.rdbuf();
	walletConfig = json::parse(confbuf.str());

	ipPortCombo += ":" + std::to_string((int)walletConfig["port"]); // Default PORT is 5000

	// Open the socket required to accept P2P requests and send responses
	p2p.OpenP2PSocket(std::to_string((int)walletConfig["port"]));
	// Start the P2P listener thread
	std::thread t1(&P2P::ListenerThread, p2p, 500);
	// Start the P2P sender thread
	std::thread t2(&P2P::SenderThread, p2p);


	if (ipPortCombo != "")
	{
		endpointAddr = SplitString(ipPortCombo, ":")[0];
		endpointPort = SplitString(ipPortCombo, ":")[1];
	}
	else
	{
		console.NetworkErrorPrint();
		console.ExitError("Could not obtain public IP");
		return 1;
	}
	
	// Start command loop
	while (true)
	{
		console.SystemPrint();
		console.WriteLine("Getting wallet info...");

		//json w = GetInfo();

		//if (constants::debugPrint == true) {
		//	console.NetworkPrint();
		//	console.WriteLine("Done");
		//}

		//if (w.is_null())
		//{
		//	ConnectionError();
		//	continue;
		//}
		//else {
		//walletInfo = w;
		walletInfo["Funds"] = 0.0f;
		walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
		walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");
		//}
		IsChainValid();

		//if (connectionStatus == 1 && !w.is_null())
		if (connectionStatus == 1)
		{
			console.MiningPrint();
			console.WriteLine("There are " + std::to_string((int)walletInfo["PendingLength"]) + " Blocks to compute");
			console.MiningPrint();
			console.WriteLine("The difficulty is " + std::to_string(((std::string)walletInfo["MineDifficulty"]).size()));

			Sync();
			try
			{
				if (constants::debugPrint == true) {
					console.BlockCheckerPrint();
					console.WriteLine("Validating blockchain...");
				}
				IsChainValid();
				if (constants::debugPrint == true) {
					console.BlockCheckerPrint();
					console.WriteLine("Done!");
				}
			}
			catch (const std::exception&)
			{
				Sync();
			}

			console.SystemPrint();
			console.Write("You have: ");
			console.WriteLine("$" + CommaLargeNumber((float)walletInfo["Funds"]) + "\n", console.yellowFGColor, "");
		}
		else
		{
			console.NetworkErrorPrint();
			console.WriteLine("Failed to connect");

			console.MiningErrorPrint();
			console.WriteLine("There are UNKNOWN Blocks to compute");
			console.MiningErrorPrint();
			console.WriteLine("The difficulty is UNKNOWN");
		}

		console.Write("Input >  ");
		std::string command = console.ReadLine();

		if (connectionStatus == 0)
		{
			console.NetworkErrorPrint();
			console.WriteLine("Not connected, no commands will work."); // I'll change this to allow for offline commands in the future
			continue;
		}

		if (SplitString(ToUpper(command), " ")[0] == "--HELP" || SplitString(ToUpper(command), " ")[0] == "-H")
			Help();
		else if (SplitString(ToUpper(command), " ")[0] == "--VERSION" || SplitString(ToUpper(command), " ")[0] == "-V")
		{
			Logo();
			continue;
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--SYNC" || SplitString(ToUpper(command), " ")[0] == "-S")
		{
			if (Sync() == 0) continue;
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
		else if (SplitString(ToUpper(command), " ")[0] == "--MINE" || SplitString(ToUpper(command), " ")[0] == "-M")
		{
			int iterations = 1;
			if (SplitString(command, " ").size() > 1)
				iterations = stoi(SplitString(command, " ")[1]);

			for (int i = 0; i < iterations; i++)
			{
				for (int s = 0; s < walletInfo["PendingLength"]; s++)
				{
					if (SyncPending(walletInfo["BlockchainLength"] + 1 + s) == 0)
					{
						ConnectionError();
						break;
					}
				}

				while (!IsChainValid())
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
						if (SyncBlock(1 + a) == 0)
						{
							ConnectionError();
							break;
						}
					}
				}

				if (GetProgram() == 0)
				{
					ConnectionError();
					continue;
				}

				walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
				walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");

				console.MiningPrint();
				console.WriteLine("Blockchain length: " + std::to_string((int)walletInfo["BlockchainLength"] + 1));

				std::ifstream blockFile("./wwwdata/pendingblocks/block" + std::to_string((int)walletInfo["BlockchainLength"] + 1) + ".dccblock");
				std::stringstream blockBuffer;
				blockBuffer << blockFile.rdbuf();
				std::string content = blockBuffer.str();
				blockFile.close();

				json blockJson = json::parse(content);
				std::string transactions = JoinArrayPieces(blockJson["transactions"]);
				std::string lastHash = (std::string)blockJson["lastHash"];

				if (Mine(lastHash, transactions, ((int)walletInfo["BlockchainLength"] + 1)) == 0)
				{
					ConnectionError();
					continue;
				}

				walletInfo = GetInfo();
				walletInfo["BlockchainLength"] = FileCount("./wwwdata/blockchain/");
				walletInfo["PendingLength"] = FileCount("./wwwdata/pendingblocks/");
				if (walletInfo.is_null())
				{
					ConnectionError();
					continue;
				}
			}
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--MINEANY" || SplitString(ToUpper(command), " ")[0] == "-MA")
		{
			std::string diff = "";
			if (SplitString(command, " ").size() == 3)
				diff = SplitString(command, " ")[2];
			MineAnyBlock(stoi(SplitString(command, " ")[1]), diff);
		}
		else if (SplitString(ToUpper(command), " ")[0] == "--CONNECT" || SplitString(ToUpper(command), " ")[0] == "-C")
		{
			if (SplitString(command, " ").size() < 3)
				continue;

			endpointPort = SplitString(command, " ")[1];
			peerPort = SplitString(command, " ")[2];

			p2p.StartP2P(endpointAddr, endpointPort, peerPort);
			console.NetworkPrint();
			console.WriteLine("Closed P2P");
		}
		connectionStatus = 1;
	}
}

// Print the logo art
void Logo()
{
	console.WriteLine(R"V0G0N(
 ______      ______    ______  
|_   _ `.  .' ___  | .' ___  | 
  | | `. \/ .'   \_|/ .'   \_| 
  | |  | || |       | |        
 _| |_.' /\ `.___.'\\ `.___.'\ 
|______.'  `.____ .' `.____ .' 

DCC, copyright (c) AstroSam (sam-astro) 2023
)V0G0N");
	console.WriteLine("client: " + VERSION);
	console.WriteLine("block: " + blockVersion + "\n\n");
}

// Print the help menu
void Help()
{
	console.WriteLine(R"V0G0N(

Usage: miner [options]
		 OR (while in interactive mode)
	   Input >  [options]
Options:
  -h, --help                          Display this help menu
  -v, --version                       Print the current wallet and block version
  -s, --sync                          Manually re-sync blockchain
  -m, --mine <amount>                 Mine <amount> number of blocks, defaults to 1 if not specified
  -ma, --mineany <block num> <dif>    (Debug) Mines the block specified by <block num> at the given 
                                      difficulty <dif>
  -sn, --send <addr> <amount>         Sends the <amount> of DCC to a receiving address <addr>
  -c, --connect <local> <peer>        Opens manual shell for a peer connection, reveiving at the port
                                      <local> and sending to the peer's port at <peer>

)V0G0N");
}

// Sync the entire blockchain
int Sync()
{
	try
	{
		for (int i = 1; i < walletInfo["BlockchainLength"] + 1; i++)
			SyncBlock(i);
		GetProgram();
		return 1;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to sync chain : " << e.what() << std::endl;
		return 0;
	}
}

// Get the wallet and blockchain information from the server   // TODO: get most information locally, and get things like difficulty from peers
json GetInfo()
{
	try
	{
		Http http;
		http.blockVersion = blockVersion;
		std::vector<std::string> args = { "query=getInfo", "fromAddress=" + (std::string)walletInfo["Address"] };
		std::string html = http.StartHttpWebRequest(serverURL + "/dcc/?", args);
		//std::cout << html << std::endl;
		return json::parse(html);

		//string html = p2p.Connect("index.php?query=getInfo&fromAddress=" + walletInfo["Address"]).Trim();
		//return json::parse(html);


		//try
		//{
		//    Http http = new Http();
		//    http.blockVersion = blockVersion;
		//    string[] args = new string[] { "query=getInfo", "fromAddress=" + walletInfo.Address };
		//    string html = http.StartHttpWebRequest(serverURL + "/dcc/?", args);

		//    string content = html.Trim();
		//    return JsonConvert.DeserializeObject<WalletInfo>(content);
		//}
		//catch (const std::exception&)
		//{
		//    return null;
		//}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		console.ExitError("Failed to get info from server ");
		return json();
	}
}

// Read the configuration file of the assigned program, and return the JSON data
json ReadProgramConfig()
{
	std::ifstream t("./wwwdata/programs/" + id + ".cfg");
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
		std::ofstream configFile("./wwwdata/programs/" + id + ".cfg");
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
int GetProgram()
{
	float life = 0;
	for (auto item : fs::directory_iterator("./wwwdata/programs/"))
	{
		if ((item.path().string()).find(".cfg") != std::string::npos)
		{
			id = SplitString(SplitString((item.path()).string(), ".cfg")[0], "/programs/")[1];
			life = GetProgramLifeLeft(id);
			console.MiningPrint();
			console.WriteLine("Program life is " + std::to_string(life));
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
					console.ErrorPrint();
					console.WriteLine("Error removing \"" + oldProgram.path().string() + "\"");
				}
			}

			Http http;
			http.blockVersion = blockVersion;
			std::vector<std::string> args = { "query=assignProgram" };
			std::string assignedProgram = http.StartHttpWebRequest(serverURL + "/dcc/?", args);

			console.NetworkPrint();
			console.WriteLine("Assigning Program...");

			id = assignedProgram;

			if (constants::debugPrint == true) {
				console.NetworkPrint();
				console.WriteLine("./wwwdata/programs/" + id + ".cfg");
			}

			DownloadFile(serverURL + "/dcc/programs/" + id + ".cfg", "./wwwdata/programs/" + id + ".cfg", true);
			DownloadFile(serverURL + "/dcc/programs/" + id + ".zip", "./wwwdata/programs/" + id + ".zip", true);

			std::string tarExtractCommand = "tar -xf ./wwwdata/programs/" + id + ".zip -C ./wwwdata/programs/";

			//ExecuteCommand(tarExtractCommand.c_str());
			if (!fs::exists("./wwwdata/programs/" + id)) {
				//ExecuteCommand(("mkdir ./wwwdata/programs/" + id).c_str());
				//fs::create_directory("./wwwdata/programs/" + id);
				ExecuteCommand(tarExtractCommand.c_str());
			}
			//ExecuteCommand(("cargo build ./wwwdata/programs/" + id + "/").c_str());
			//ExtractZip("./wwwdata/programs/" + id + ".zip", "./wwwdata/programs/" + id);

		//// If improperly zipped (meaning Cargo.toml file is deeper in the directory than the base folder),
		//// the contents will be moved up a single directory.
		//if (!fs::exists("./wwwdata/programs/" + id + "/Cargo.toml"))
		//{
		//	Directory.Move(Directory.GetDirectories("./wwwdata/programs/" + id)[0], "./wwwdata/programs/tmpdir");
		//	Directory.Delete("./wwwdata/programs/" + id, true);
		//	Directory.Move("./wwwdata/programs/tmpdir", "./wwwdata/programs/" + id);
		//}
		}

		char sha256OutBuffer[65];
		sha256_file((char*)("./wwwdata/programs/" + id + ".zip").c_str(), sha256OutBuffer);
		std::string ourHash = sha256OutBuffer;

		Http http1;
		http1.blockVersion = blockVersion;
		std::vector<std::string> args1 = { "query=hashProgram", "programID=" + id };
		std::string theirHash = http1.StartHttpWebRequest(serverURL + "/dcc/?", args1);

		if (ourHash != theirHash)
		{
			console.MiningErrorPrint();
			console.WriteLine("Assigned program has been modified, re-downloading...");
			GetProgram();
		}

		programConfig = ReadProgramConfig();

		if (programConfig["Built"] == false)
		{
			console.MiningPrint();
			console.WriteLine("Building assigned program, wait until it's finished to start mining");

			console.RustPrint();
			console.WriteLine("Compiling program... ");
			ExecuteCommand(("cargo build --release --manifest-path ./wwwdata/programs/" + id + "/Cargo.toml").c_str());
			console.RustPrint();
			console.WriteLine("Done Compiling");

			programConfig["Built"] = true;
			WriteProgramConfig();
		}
		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

// Get the amount of time left of the current assigned rust program, by asking the server     // TODO: change to ask peers instead of the server
float GetProgramLifeLeft(std::string id)
{
	try
	{
		Http http;
		http.blockVersion = blockVersion;
		std::vector<std::string> args = { "query=getProgramLifeLeft", "programID=" + id };
		std::string html = http.StartHttpWebRequest(serverURL + "/dcc/?", args);

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

// Sync a single pending block from the server     // TODO: change this from asking the server to asking peers.
int SyncPending(int whichBlock)
{
	try
	{
		if (fs::exists("./wwwdata/pendingblocks/block" + std::to_string(whichBlock) + ".dccblock"))
			return 1;


		DownloadFile(serverURL + "/dcc/?query=getPendingBlock&blockNum=" + std::to_string(whichBlock) + "&Version=" + blockVersion,
			"./wwwdata/pendingblocks/block" + std::to_string(whichBlock) + ".dccblock",
			true);

		return 1;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to sync block : " << e.what() << std::endl;
		return 0;
	}
}

// Sync a single solid block from the server     // TODO: change this from asking the server to asking peers.
int SyncBlock(int whichBlock)
{
	try
	{
		if (fs::exists("./wwwdata/blockchain/block" + std::to_string(whichBlock) + ".dccblock"))
			return 1;

		DownloadFile(serverURL + "/dcc/?query=getBlock&blockNum=" + std::to_string(whichBlock) + "&Version=" + blockVersion,
			"./wwwdata/blockchain/block" + std::to_string(whichBlock) + ".dccblock",
			true);

		return 1;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to sync block : " << e.what() << std::endl;
		return 0;
	}
}

// Check every single block to make sure the nonce is valid, the hash matches the earlier and later blocks, and each transaction has a valid signature.
bool IsChainValid()
{
	while (FileCount("./wwwdata/blockchain/") < walletInfo["BlockchainLength"])
	{
		if (SyncBlock(FileCount("./wwwdata/blockchain/") + 1) == 0)
		{
			ConnectionError();
			break;
		}
	}

	int chainLength = FileCount("./wwwdata/blockchain/");

	float tmpFunds = 0;

	console.BlockCheckerPrint();
	console.WriteLine("Checking blocks...");

	// Apply funds to user from the first block separately
	try
	{
		if (chainLength >= 1) {
			std::ifstream th("./wwwdata/blockchain/block1.dccblock");
			std::stringstream buffer2;
			buffer2 << th.rdbuf();
			std::string content2 = buffer2.str();
			json firstBlock = json::parse(content2);
			//std::cout << "content: " << content2 << "\nnumoftrans: " << std::to_string(firstBlock["transactions"].size()) << std::endl;
			//std::vector<std::string> o["transactions"] = o["transactions"];
			//std::vector<uint64_t> o["transactionTimes"] = o["transactionTimes"];
			for (int tr = 0; tr < firstBlock["transactions"].size(); tr++) {
				std::string transactionContent = SplitString(firstBlock["transactions"][tr], "|")[0];
				std::string signature = SplitString(firstBlock["transactions"][tr], "|")[1];
				std::string publicKey = SplitString(firstBlock["transactions"][tr], "|")[2];
				uint64_t transactionTime = firstBlock["transactionTimes"][tr];

				// Check if the sending or receiving address is the same as the user's
				std::vector<std::string> transactionDetails = SplitString(transactionContent, "->");
				if (transactionDetails.size() == 3) {
					if ((std::string)walletInfo["Address"] == transactionDetails[1])
						tmpFunds -= stof(transactionDetails[0]);
					else if ((std::string)walletInfo["Address"] == transactionDetails[2])
						tmpFunds += stof(transactionDetails[0]);
				}
				else if ((std::string)walletInfo["Address"] == transactionDetails[1]) {
					tmpFunds += stof(transactionDetails[0]);
				}
			}
			//console.BlockCheckerPrint();
			//std::cout << "\nfunds: " + std::to_string(tmpFunds) << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		if (constants::debugPrint == true) {
			std::cerr << std::endl << e.what() << std::endl;
		}
		console.ExitError("Failure, exiting");
	}

	for (int i = 2; i < chainLength; i++)
	{
		try
		{
			std::ifstream t("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
			std::stringstream buffer;
			buffer << t.rdbuf();
			std::string content = buffer.str();

			bool changedBlockData = false;
			json o = json::parse(content);
			std::vector<std::string> trans = o["transactions"];
			std::vector<uint64_t> transTimes = o["transactionTimes"];


			//console.BlockCheckerPrint();
			//std::cout << "Transactions: " << std::to_string(trans.size()) << std::endl;


			if (o["Version"] == nullptr || o["Version"] == "" || o["Version"] != blockVersion)
			{
				UpgradeBlock(o, blockVersion);
				std::ofstream blockFile("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
				if (blockFile.is_open())
				{
					blockFile << o.dump();
					blockFile.close();
				}
			}

			std::string lastHash = o["lastHash"];
			std::string currentHash = o["hash"];
			std::string nonce = o["nonce"];
			std::string transactions = JoinArrayPieces(trans);


			std::ifstream td("./wwwdata/blockchain/block" + std::to_string(i + 1) + ".dccblock");
			std::stringstream bufferd;
			bufferd << td.rdbuf();
			std::string nextBlockText = bufferd.str();
			json o2 = json::parse(nextBlockText);

			std::string nextHash = o2["lastHash"];

			if (o2["Version"] == nullptr || o2["Version"] == "" || o2["Version"] != blockVersion)
			{
				UpgradeBlock(o, blockVersion);
				std::ofstream blockFile("./wwwdata/blockchain/block" + std::to_string(i + 1) + ".dccblock");
				if (blockFile.is_open())
				{
					blockFile << o.dump();
					blockFile.close();
				}
			}

			console.WriteBulleted("Validating block: " + std::to_string(i), 3);
			char sha256OutBuffer[65];
			sha256_string((char*)(lastHash + transactions + nonce).c_str(), sha256OutBuffer);
			std::string blockHash = sha256OutBuffer;

			if ((blockHash[0] != '0' && blockHash[1] != '0') || blockHash != currentHash || blockHash != nextHash)
			{
				std::string rr = "";
				if ((blockHash[0] != '0' && blockHash[1] != '0'))
					rr = "0";
				if (blockHash != currentHash)
					rr = "1";
				if (blockHash != nextHash)
					rr = "2";
				console.WriteLine("    X Bad Block X  " + std::to_string(i) + " R" + rr, console.redFGColor, "");
				return false;
			}
			float tmpFunds2 = 0;
			// Check all transactions to see if they have a valid signature
			for (int tr = 0; tr < trans.size(); tr++) {
				std::string transactionContent = SplitString(trans[tr], "|")[0];
				std::string signature = decode64(SplitString(trans[tr], "|")[1]);
				std::string publicKey = SplitString(trans[tr], "|")[2];
				uint64_t transactionTime = transTimes[tr];

				// The from address should be the same as the hash of the public key, so check that first:
				char walletBuffer[65];
				sha256_string((char*)(publicKey).c_str(), walletBuffer);
				std::string expectedWallet = walletBuffer;
				if (SplitString(transactionContent, "->")[1] != expectedWallet) {
					trans.erase(trans.begin() + tr);
					transTimes.erase(transTimes.begin() + tr);
					changedBlockData = true;
					continue;
				}

				// Hash transaction data
				sha256OutBuffer[65];
				sha256_string((char*)(transactionContent + " " + std::to_string(transactionTime)).c_str(), sha256OutBuffer);
				std::string tranhash = sha256OutBuffer;

				// Verify signature by decrypting hash with public key
				std::string decryptedSig = rsa_pub_decrypt(signature, publicKey);

				// The decrypted signature should be the same as the hash we just generated
				if (decryptedSig != tranhash) {
					trans.erase(trans.begin() + tr);
					transTimes.erase(transTimes.begin() + tr);
					console.Write("  Bad signature on T:" + std::to_string(tr), console.redFGColor, "");
					continue;
				}


				// Now check if the sending or receiving address is the same as the user's
				std::vector<std::string> transactionDetails = SplitString(transactionContent, "->");
				if (transactionDetails.size() >= 3) {
					if ((std::string)walletInfo["Address"] == transactionDetails[1])
						tmpFunds2 -= stof(transactionDetails[0]);
					if ((std::string)walletInfo["Address"] == TrimString(SplitString(transactionDetails[2], "|")[0]))
						tmpFunds2 += stof(transactionDetails[0]);
				}
				else if ((std::string)walletInfo["Address"] == transactionDetails[1])
					tmpFunds2 += stof(transactionDetails[0]);

			}
			// Update the old transactions with the new ones
			o["transactions"] = trans;
			o["transactionTimes"] = transTimes;

			// Save json data to file if it was changed
			if (changedBlockData) {
				try
				{
					std::ofstream blockFile("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
					if (blockFile.is_open())
					{
						blockFile << o.dump();
						blockFile.close();
					}
				}
				catch (const std::exception& e)
				{
					std::cerr << e.what() << std::endl;
					return 0;
				}
			}

			// Update funds
			tmpFunds += tmpFunds2;

			console.Write("\tTransactions: " + std::to_string(trans.size()));

			console.WriteLine(" \tOk  ", console.greenFGColor, "");

			//console.BlockCheckerPrint();
			//std::cout << "  funds: " + std::to_string(tmpFunds2) << std::endl;
		}
		catch (const std::exception& e)
		{
			if (constants::debugPrint == true) {
				std::cerr << std::endl << e.what() << std::endl;
			}
			console.ExitError("Failure, exiting");
		}
	}
	walletInfo["Funds"] = tmpFunds;
	return true;
}

// Mine a single block with specified data and using the difficulty stored in walletInfo["MineDifficulty"]
int Mine(std::string lastHash, std::string transactionHistory, int blockNum)
{
	walletInfo["MineDifficulty"] = "00000";
	console.MiningPrint();
	console.Write("Mining ");
	console.Write("block " + std::to_string(blockNum), console.whiteBGColor, console.blackFGColor);
	console.Write(" at difficulty ");
	console.Write((std::string)walletInfo["MineDifficulty"], console.whiteBGColor, console.blackFGColor);
	console.Write(" :");
	console.Write("\n");
	try
	{
		auto startTime = std::chrono::steady_clock::now();

		console.RustPrint();
		console.WriteLine("Starting program... ");
		boost::process::child cargoProc = ExecuteAsync("cargo run --manifest-path ./wwwdata/programs/" + id + "/Cargo.toml", false);

		//Checks Hash
		int nonce = 0;
		unsigned char hash[32];
		std::string dif = (std::string)walletInfo["MineDifficulty"];
		char* c_difficulty = (char*)dif.c_str();
		int difficultyLen = dif.length();
		auto hashStart = std::chrono::steady_clock::now();
		int hashesPerSecond = 0;
		int hashesAtStart = 0;
		std::string hData = lastHash + transactionHistory;
		char sha256OutBuffer[65];
		while (!CharStrStartsWith(hash, c_difficulty, difficultyLen))
		{
			if ((since(hashStart).count() / 1000) >= 1)
			{
				hashesPerSecond = nonce - hashesAtStart;
				hashStart = std::chrono::steady_clock::now();
				hashesAtStart = nonce;

				cstr_to_hexstr(hash, 32, sha256OutBuffer);
				console.Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + CommaLargeNumber(nonce) + " # " + std::string(sha256OutBuffer));
				console.Write("   " + FormatHPS(hashesPerSecond) + "            ");
			}

			nonce++;
			sha256_full_cstr((char*)(hData + std::to_string(nonce)).c_str(), hash);
		}

		std::cout << std::endl;
		
		// Wait for the rust program to finish running
		if (cargoProc.running())
			cargoProc.wait();


		/*char sha256OutBuffer[65];
		sha256_string((char*)(lastHash + transactionHistory + std::to_string(nonce)).c_str(), sha256OutBuffer);
		hash = sha256OutBuffer;
		std::string url = serverURL + "/dcc/?query=submitBlock&blockNum=" + std::to_string(blockNum) + "&nonce=" + std::to_string(nonce) + "&minedHash=" + hash + "&fromAddress=" + (std::string)walletInfo["Address"] + "&programID=" + id + "&time=" + std::to_string(since(startTime).count() / 1000.0f) + "&Version=" + blockVersion;

		std::string s = UploadFile(url, "./wwwdata/programs//" + id + "/out.txt");*/
		//std::to_string((int)walletInfo["BlockchainLength"] + 1)

		// Convert hash into hexadecimal string
		cstr_to_hexstr(hash, 32, sha256OutBuffer);
		std::string hashStr = std::string(sha256OutBuffer);


		// Write new hash and nonce into pending block
		std::ifstream blockFile("./wwwdata/pendingblocks/block" + std::to_string(blockNum) + ".dccblock");
		std::stringstream blockBuffer;
		blockBuffer << blockFile.rdbuf();
		std::string content = blockBuffer.str();
		blockFile.close();

		json blockJson = json::parse(content);

		blockJson["hash"] = hashStr;
		blockJson["nonce"] = std::to_string(nonce);
		// Get current unix time in seconds
		uint64_t sec = duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		blockJson["time"] = sec;

		// Save new json data to file into finished blockchain folder
		try
		{
			std::ofstream blockFilew("./wwwdata/blockchain/block" + std::to_string(blockNum) + ".dccblock");
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

		// Remove the block from pending
		fs::remove("./wwwdata/pendingblocks/block" + std::to_string(blockNum) + ".dccblock");

		// Create new pending block if there are no more
		if (FileCount("./wwwdata/pendingblocks/") == 0) {
			json blockJson = json();

			blockJson["hash"] = "0000000000000000000000000000000000000000000000000000000000000000";
			blockJson["lastHash"] = hashStr;
			blockJson["nonce"] = "";
			blockJson["time"] = "";
			blockJson["Version"] = blockVersion;
			blockJson["transactions"] = std::vector<std::string>();
			blockJson["transactionTimes"] = std::vector<std::string>();

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


		console.MiningPrint();
		console.WriteLine("Mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");

		return 1;
	}
	catch (const std::exception& e)
	{
		if (constants::debugPrint == true)
			std::cerr << e.what() << std::endl;
		return 0;
	}
}

// Send funds to another address, by first checking if the user has enough funds in the first place,
// then adding the transaction and signature to a pending block
int SendFunds(std::string toAddress, float amount)
{
	console.DebugPrint();
	console.Write("Sending ");
	console.Write("$" + std::to_string(amount), console.whiteBGColor, console.blackFGColor);
	console.Write(" to ");
	console.Write(toAddress, "", console.greenFGColor);
	console.Write("...");
	console.Write("\n");

	console.NetworkPrint();
	console.Write("Syncing blocks... ");
	console.Write("\n");
	Sync();

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
		blockJson["Version"] = blockVersion;
		blockJson["transactions"] = std::vector<std::string>();
		blockJson["transactionTimes"] = std::vector<std::string>();

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

	while (!IsChainValid())
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
			if (SyncBlock(1 + a) == 0)
			{
				ConnectionError();
				break;
			}
		}
	}
	//std::cout << "checnkingfunds..." << std::endl;
	// Check if user even has enough funds for the transaction
	if ((float)walletInfo["Funds"] < amount) {
		console.MiningErrorPrint();
		console.WriteLine("Not enough funds", "", console.redFGColor);
		return 0;
	}
	//std::cout << "done checnkingfunds..." << std::endl;

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
		sha256_string((char*)(std::to_string(amount) + "->" + (std::string)walletInfo["Address"] + "->" + toAddress + " " + std::to_string(sec)).c_str(), sha256OutBuffer);
		std::string hash = sha256OutBuffer;

		//std::cout << "before sig" << std::endl;
		// Generate signature by encrypting hash with private key
		std::string signature = rsa_pri_encrypt(hash, keypair[1]);
		//std::cout << "after sig" << std::endl;

		std::string sigBase64 = encode64((const unsigned char*)signature.c_str(), signature.length());
		//// Hash signature
		//sha256OutBuffer[65];
		//sha256_string((char*)(signature).c_str(), sha256OutBuffer);
		//std::string sighash = sha256OutBuffer;

		// Append transaction to list
		blockJson["transactions"].push_back(std::to_string(amount) + "->" + (std::string)walletInfo["Address"] + "->" + toAddress + "|" + sigBase64 + "|" + keypair[0]);

		// Append time to list
		blockJson["transactionTimes"].push_back(sec);

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
		}
		return 0;
	}
}

// Calculate the nonce and hash for an existing block at a specific difficulty
int MineAnyBlock(int blockNum, std::string difficulty)
{
	difficulty = ToLower(difficulty); 


	auto startTime = std::chrono::steady_clock::now();

	std::ifstream td("./wwwdata/blockchain/block" + std::to_string(blockNum) + ".dccblock");
	std::stringstream bufferd;
	bufferd << td.rdbuf();
	std::string nextBlockText = bufferd.str();
	json o = json::parse(nextBlockText);

	std::string transactions = JoinArrayPieces(o["transactions"]);
	std::string currentHash = o["hash"];
	std::string lastHash = o["lastHash"];

	//Checks Hash
	int nonce = 0;
	//std::string hash = "";
	unsigned char hash[32];
	char* c_difficulty = (char*)difficulty.c_str();
	int difficultyLen = difficulty.length();
	auto hashStart = std::chrono::steady_clock::now();
	int hashesPerSecond = 0;
	int hashesAtStart = 0;
	std::string hData = lastHash + transactions;
	//while (!StringStartsWith(hash, difficulty))
	char sha256OutBuffer[65];
	while (!CharStrStartsWith(hash, c_difficulty, difficultyLen))
	{
		if ((since(hashStart).count() / 1000) >= 1)
		{
			hashesPerSecond = nonce - hashesAtStart;
			hashStart = std::chrono::steady_clock::now();
			hashesAtStart = nonce;

			cstr_to_hexstr(hash, 32, sha256OutBuffer);
			console.Write("\r" + std::to_string((int)std::round(since(startTime).count() / 1000)) + "s :	" + CommaLargeNumber(nonce) + " # " + std::string(sha256OutBuffer));
			console.Write("   " + FormatHPS(hashesPerSecond) + "            ");
		}

		nonce++;
		sha256_full_cstr((char*)(hData + std::to_string(nonce)).c_str(), hash);
	}

	std::cout << std::endl;
	console.MiningPrint();
	console.WriteLine("Debug mined in " + std::to_string(std::round(since(startTime).count() / 1000)) + " s.");
	console.MiningPrint();
	cstr_to_hexstr(hash, 32, sha256OutBuffer);
	console.Write("Final value: hash # ");
	console.WriteLine(std::string(sha256OutBuffer), "", console.greenFGColor);
	console.MiningPrint();
	console.Write("Final value: nonce # ");
	console.WriteLine(std::to_string(nonce), "", console.greenFGColor);

	return 0;
}

// Print a connection error dialog
void ConnectionError()
{
	//connectionStatus = 0;
	console.NetworkErrorPrint();
	console.WriteLine("Failed To Connect");
}

// Execute a command in the main thread and print the output
std::string ExecuteCommand(const char* cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("_popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
		std::cout << buffer.data();
	}

	return "";
}

// Execute a process in an asynchronous background thread
boost::process::child ExecuteAsync(std::string cmd, bool printOutput)
{
	try
	{
		namespace bp = boost::process;
		std::vector<std::string> splitCommand = SplitString(cmd, " ");
		std::string command = splitCommand[0];
		std::string args;
		for (int i = 1; i < sizeof(splitCommand) / sizeof(splitCommand[0]); i++)
		{
			args += splitCommand[i] + " ";
		}
		bp::child c(cmd);

		return c;
	}
	catch (const std::exception& e)
	{
		return boost::process::child();
	}

	return boost::process::child();
}

// Upgrade a block to a newer version
json UpgradeBlock(json b, std::string toVersion)
{
	if (constants::debugPrint == true) {
		console.BlockCheckerPrint();
		console.WriteLine("Upgrading block to version " + toVersion);
	}

	if (toVersion == "v0.01alpha-coin")
	{
		b["Version"] = toVersion;
	}

	return b;
}

// Format the hashes per second float <input> into a readable and shortened string
std::string FormatHPS(float input)
{
	if (input > 1000000000.0f)
		return std::to_string(round(input / 1000000000.0f, 3)) + " gH/s";
	else if (input > 1000000.0f)
		return std::to_string(round(input / 1000000.0f, 3)) + " mH/s";
	else if (input > 1000.0f)
		return std::to_string(round(input / 1000.0f, 3)) + " kH/s";
	else
		return std::to_string(round(input, 3)) + " H/s";
}

// Round a float to <decimal_places> number of decimal places
double round(float value, int decimal_places)
{
	const double multiplier = std::pow(10.0, decimal_places);
	return std::round(value * multiplier) / multiplier;
}
