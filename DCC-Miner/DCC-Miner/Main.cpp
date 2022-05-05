// DCC-Miner.cpp

#if defined(__unix__)
#define UNIX true
#define WINDOWS false
#elif defined(_MSC_VER)
#define UNIX false
#define WINDOWS true
#endif

#include "Main.h"

//using namespace std;
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
boost::process::child ExecuteAsync(std::string cmd);


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
const std::string directoryList[] = { "./wwwdata", "./wwwdata/blockchain", "./wwwdata/pendingblocks", "./wwwdata/programs" };

json programConfig;
json walletInfo;

const std::string blockVersion = "v0.01alpha-coin";

std::string endpointAddr = "";
std::string endpointPort = "";

//PHPServ phpserv = new PHPServ();
//P2P p2p = new P2P();

struct stat info;
Console console;

int main()
{
	console.DebugPrint();
	console.WriteLine("Started");
	//console.WriteLine("Error Code " + std::to_string(ec), console.Debug());

	for (std::string dir : directoryList)
		if (!fs::is_directory(dir) || !fs::exists(dir)) {
			console.DebugPrint();
			console.WriteLine("Creating " + dir);
			fs::create_directory(dir);
		}

	console.DebugPrint();
	console.WriteLine("Checking config.cfg");
	if (!fs::exists("./config.cfg"))
	{
		std::ofstream configFile("./config.cfg");
		if (configFile.is_open())
		{
			configFile << "";
			configFile.close();
		}
	}

	std::ifstream t("./config.cfg");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string configFileRead = buffer.str();
	if (configFileRead.size() > 4)
	{
		std::string cpy = SplitString(configFileRead, "\n")[0];
		boost::trim(cpy);
		walletInfo["Address"] = cpy;
	}
	else
	{
		console.MiningPrint();
		//console.WriteDialogueAuthor(console.Mining());
		console.Write("Enter your payout wallet : ");
		walletInfo["Address"] = console.ReadLine();

		console.MiningPrint();
		//console.WriteDialogueAuthor(console.Mining());
		console.Write("Stay logged in? Y/N : ");
		std::string stayLoggedIn = console.ReadLine();
		if (ToUpper(stayLoggedIn) == "Y")
		{
			std::ofstream configFile("./config.cfg");
			if (configFile.is_open())
			{
				configFile << walletInfo["Address"];
				configFile.close();
			}
		}
	}

	//phpserv.StartServer();
	//p2p.Connect("");

	// Get public IP address and PORT
	Http http;
	std::vector<std::string> args;
	std::string ipPortCombo = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/ipget.php", args);
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


	console.DebugPrint();
	console.WriteLine("Starting P2P with: " + ipPortCombo);
	StartP2P(endpointAddr, endpointPort);

	console.ErrorPrint();
	console.ExitError("Stopped before json, after P2P.");

	while (true)
	{
		json w = GetInfo();

		if (w.is_null())
		{
			ConnectionError();
		}
		else
			walletInfo = w;

		if (connectionStatus == 1 && !w.is_null())
		{
			console.MiningPrint();
			console.WriteLine("There are " + (std::string)walletInfo["PendingLength"] + " Blocks to compute");
			console.MiningPrint();
			console.WriteLine("The difficulty is " + ((std::string)walletInfo["MineDifficulty"]).size());
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

		console.Write("DCC >  ");
		std::string command = console.ReadLine();

		if (connectionStatus == 0)
		{
			console.NetworkErrorPrint();
			console.WriteLine("Not connected, no commands will work."); // I'll change this to allow for offline commands in the future
			continue;
		}

		if (SplitString(ToUpper(command), " ")[0] == "--HELP" || SplitString(ToUpper(command), " ")[0] == "-H")
			Help();
		if (SplitString(ToUpper(command), " ")[0] == "--SYNC" || SplitString(ToUpper(command), " ")[0] == "-S")
		{
			if (Sync() == 0)
			{
				ConnectionError();
				continue;
			}
			//console.Write(((char)7).ToString()); // Bell char to make sound
		}
		if (SplitString(ToUpper(command), " ")[0] == "--MINE" || SplitString(ToUpper(command), " ")[0] == "-M")
		{
			int iterations = 1;
			if (SplitString(command, " ").size() > 1)
				iterations = stoi(SplitString(command, " ")[1]);

			for (int i = 0; i < iterations; i++)
			{
				for (auto oldBlock : fs::directory_iterator("./wwwdata/pendingblocks/"))
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
				for (int s = 0; s < walletInfo["PendingLength"]; s++)
				{
					if (SyncPending(walletInfo["BlockchainLength"] + 1 + s) == 0)
					{
						ConnectionError();
						break;
					}
				}

				if (!IsChainValid())
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

				console.MiningPrint();
				console.WriteLine("Blockchain length: " + (walletInfo["BlockchainLength"] + 1));

				std::ifstream blockFile("./wwwdata/pendingblocks/block" + std::to_string(walletInfo["BlockchainLength"] + 1) + ".dccblock");
				std::stringstream blockBuffer;
				blockBuffer << blockFile.rdbuf();
				std::string content = blockBuffer.str();

				json o = json::parse(content);
				std::string transactions = JoinArrayPieces((std::string[])o["Transactions"]);
				std::string lastHash = o["LastHash"];
				if (Mine(lastHash, transactions, (walletInfo["BlockchainLength"] + 1)) == 0)
				{
					ConnectionError();
					continue;
				}

				walletInfo = GetInfo();
				if (walletInfo.is_null())
				{
					ConnectionError();
					continue;
				}
			}
		}
		if (SplitString(ToUpper(command), " ")[0] == "--MINEANY" || SplitString(ToUpper(command), " ")[0] == "-MA")
		{
			//std::ifstream blockFile("./wwwdata/pendingblocks/block" + to_string(walletInfo["BlockchainLength"] + 1) + ".dccblock");
			//std::stringstream blockBuffer;
			//blockBuffer << blockFile.rdbuf();
			//std::string content = blockBuffer.str();

			//json o = json::parse(content);
			//std::string transactions = JoinArrayPieces((std::string[])o["Transactions"]);
			//std::string lastHash = o["LastHash"];

			std::string diff = "";
			if (SplitString(command, " ").size() == 3)
				diff = SplitString(command, " ")[2];
			MineAnyBlock(stoi(SplitString(command, " ")[1]), diff);
		}
		connectionStatus = 1;
	}
}

void Help()
{
	console.WriteLine(R"V0G0N(

Usage: miner [options]
		 OR (while in interactive mode)
	   DCC >  [options]
Options:
  -h, --help						  Display this help menu
  -s, --sync						  Manually re-sync blockchain
  -m, --mine <amount>			      Mine <amount> number of blocks, defaults to 1 if not specified
  -ma, --mineany <block num> <dif>	  (Debug) Mines the block specified by <block num> at the given difficulty <dif>

)V0G0N");
}

int Sync()
{
	try
	{
		for (auto oldBlock : fs::directory_iterator("./wwwdata/pendingblocks/"))
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
		for (int i = 1; i < walletInfo["PendingLength"] + 1; i++)
		{
			SyncPending(walletInfo["PendingLength"] + i);
		}
		for (int i = 1; i < walletInfo["BLockchainLength"] + 1; i++)
		{
			SyncBlock(i);
		}
		GetProgram();
		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

json GetInfo()
{
	Http http;
	http.blockVersion = blockVersion;
	std::vector<std::string> args = { "query=getInfo", "fromAddress=" + walletInfo["Address"] };
	std::string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);
	return json::parse(html);

	//string html = p2p.Connect("index.php?query=getInfo&fromAddress=" + walletInfo["Address"]).Trim();
	//return json::parse(html);


	//try
	//{
	//    Http http = new Http();
	//    http.blockVersion = blockVersion;
	//    string[] args = new string[] { "query=getInfo", "fromAddress=" + walletInfo.Address };
	//    string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

	//    string content = html.Trim();
	//    return JsonConvert.DeserializeObject<WalletInfo>(content);
	//}
	//catch (const std::exception&)
	//{
	//    return null;
	//}
}

json ReadProgramConfig()
{
	std::ifstream t("./wwwdata/programs/" + id + ".cfg");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string content = buffer.str();
	return json::parse(content);
}

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
			//foreach(string oldProgram in Directory.GetDirectories("./wwwdata/programs/", "*.*", SearchOption.TopDirectoryOnly))
			//{
			//	try
			//	{
			//		Directory.Delete(oldProgram, true);
			//	}
			//	catch (const std::exception&)
			//	{
			//	}
			//}

			Http http;
			http.blockVersion = blockVersion;
			std::vector<std::string> args = { "query=assignProgram" };
			std::string assignedProgram = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

			console.NetworkPrint();
			console.WriteLine("Assigning Program...");

			id = assignedProgram;

			console.NetworkPrint();
			console.WriteLine("./wwwdata/programs/" + id + ".cfg");

			DownloadFile("http://api.achillium.us.to/dcc/programs/" + id + ".cfg", "./wwwdata/programs/" + id + ".cfg", true);
			DownloadFile("http://api.achillium.us.to/dcc/programs/" + id + ".zip", "./wwwdata/programs/" + id + ".zip", true);

			if (!fs::exists("./wwwdata/programs/" + id))
				ExtractZip("./wwwdata/programs/" + id + ".zip", "./wwwdata/programs/" + id);

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
		std::string theirHash = http1.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args1);

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
			ExecuteCommand(("cargo build ./wwwdata/programs/" + id + "/").c_str());
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

float GetProgramLifeLeft(std::string id)
{
	try
	{
		Http http;
		http.blockVersion = blockVersion;
		std::vector<std::string> args = { "query=getProgramLifeLeft", "programID=" + id };
		std::string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

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

int SyncPending(int whichBlock)
{
	try
	{
		//Http http;
		//http.blockVersion = blockVersion;
		//vector<string> args = { "query=getPendingBlock", "blockNum=" + whichBlock };
		//string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

		//console.WriteLine("Synced pending: " + whichBlock, console.Mining());
		//File.WriteAllText("./wwwdata/pendingblocks/block" + to_string(whichBlock) + ".dccblock", html);
		DownloadFile("http://api.achillium.us.to/dcc/?query=getPendingBlock&blockNum=" + std::to_string(whichBlock) + "&Version=" + blockVersion,
			"./wwwdata/pendingblocks/block" + std::to_string(whichBlock) + ".dccblock",
			true);

		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

int SyncBlock(int whichBlock)
{
	try
	{
		//Http http;
		//http.blockVersion = blockVersion;
		//vector<string> args = { "query=getBlock", "blockNum=" + whichBlock };
		//string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

		//console.WriteLine("Synced: " + whichBlock);
		//File.WriteAllText("./wwwdata/blockchain/block" + to_string(whichBlock) + ".dccblock", html);

		DownloadFile("http://api.achillium.us.to/dcc/?query=getBlock&blockNum=" + std::to_string(whichBlock) + "&Version=" + blockVersion,
			"./wwwdata/blockchain/block" + std::to_string(whichBlock) + ".dccblock",
			true);

		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

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

	for (int i = 1; i < chainLength; i++)
	{
		std::ifstream t("./wwwdata/blockchain/block" + std::to_string(i) + ".dccblock");
		std::stringstream buffer;
		buffer << t.rdbuf();
		std::string content = buffer.str();

		json o = json::parse(content);
		std::string trans[] = { o["Transactions"] };

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

		std::string lastHash = o["LastHash"];
		std::string currentHash = o["Hash"];
		std::string nonce = o["Nonce"];
		std::string transactions = JoinArrayPieces(trans);


		std::ifstream td("./wwwdata/blockchain/block" + std::to_string(i + 1) + ".dccblock");
		std::stringstream bufferd;
		bufferd << td.rdbuf();
		std::string nextBlockText = bufferd.str();
		o = json::parse(nextBlockText);

		std::string nextHash = o["LastHash"];

		if (o["Version"] == nullptr || o["Version"] == "" || o["Version"] != blockVersion)
		{
			UpgradeBlock(o, blockVersion);
			std::ofstream blockFile("./wwwdata/blockchain/block" + std::to_string(i + 1) + ".dccblock");
			if (blockFile.is_open())
			{
				blockFile << o.dump();
				blockFile.close();
			}
		}

		console.BlockCheckerPrint();
		console.WriteLine("Validating block: " + i);
		char sha256OutBuffer[65];
		sha256_string((char*)(lastHash + transactions + nonce).c_str(), sha256OutBuffer);
		std::string blockHash = sha256OutBuffer;
		if ((blockHash[0] != '0' && blockHash[1] != '0') || blockHash != currentHash || blockHash != nextHash)
		{
			return false;
		}
	}
	return true;
}

int Mine(std::string lastHash, std::string transactionHistory, int blockNum)
{
	console.MiningPrint();
	console.Write("Mining ");
	console.Write("block " + blockNum, console.whiteBGColor, console.blackFGColor);
	console.Write(" at difficulty ");
	console.Write(walletInfo["MineDifficulty"], console.whiteBGColor, console.blackFGColor);
	console.Write(" :");
	console.Write("\n");
	try
	{
		auto startTime = std::chrono::steady_clock::now();
		std::cout << "Elapsed(ms)=" << since(startTime).count() << std::endl;

		console.RustPrint();
		console.WriteLine("Starting program... ");
		boost::process::child cargoProc = ExecuteAsync("cargo run ./wwwdata/programs/" + id + "/");

		//Checks Hash
		int nonce = 0;
		std::string hash = "";
		auto hashStart = std::chrono::steady_clock::now();
		int hashesPerSecond = 0;
		int hashesAtStart = 0;
		while (!StringStartsWith(hash, walletInfo["MineDifficulty"]))
		{
			if (since(startTime).count() * 1000 >= 1)
			{
				hashesPerSecond = nonce - hashesAtStart;
				hashStart = std::chrono::steady_clock::now();
				hashesAtStart = nonce;

				console.Write("\r" + std::to_string(std::round(since(startTime).count() * 1000)) + " :	" + std::to_string(nonce) + " # " + hash);
				console.Write("   " + FormatHPS(hashesPerSecond) + "            ");
			}


			nonce++;
			char sha256OutBuffer[65];
			sha256_string((char*)(lastHash + transactionHistory + std::to_string(nonce)).c_str(), sha256OutBuffer);
			hash = sha256OutBuffer;

			if (!cargoProc.running())
				break;
		}

		if (cargoProc.running())
		{
			cargoProc.wait();
		}

		//console.Clear();

		char sha256OutBuffer[65];
		sha256_string((char*)(lastHash + transactionHistory + std::to_string(nonce)).c_str(), sha256OutBuffer);
		hash = sha256OutBuffer;
		std::string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + std::to_string(blockNum) + "&nonce=" + std::to_string(nonce) + "&minedHash=" + hash + "&fromAddress=" + (std::string)walletInfo["Address"] + "&programID=" + id + "&time=" + std::to_string(since(startTime).count() / 1000.0f) + "&Version=" + blockVersion;

		std::string s = UploadFile(url, "./wwwdata/programs//" + id + "/out.txt");

		console.MiningPrint();
		console.WriteLine(s + " in " + std::to_string(std::round(since(startTime).count() * 1000)) + " s.");

		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

int MineAnyBlock(int blockNum, std::string difficulty)
{
	if (difficulty == "")
		difficulty = walletInfo["MineDifficulty"];

	auto startTime = std::chrono::steady_clock::now();

	std::ifstream td("./wwwdata/blockchain/block" + std::to_string(blockNum) + ".dccblock");
	std::stringstream bufferd;
	bufferd << td.rdbuf();
	std::string nextBlockText = bufferd.str();
	json o = json::parse(nextBlockText);

	std::string transactions = JoinArrayPieces((std::string[])o["Transactions"]);
	std::string currentHash = o["Hash"];
	std::string lastHash = o["LastHash"];

	//Checks Hash
	int nonce = 0;
	std::string hash = "";
	auto hashStart = std::chrono::steady_clock::now();
	int hashesPerSecond = 0;
	int hashesAtStart = 0;
	while (!StringStartsWith(hash, difficulty))
	{
		if (since(startTime).count() * 1000 >= 1)
		{
			hashesPerSecond = nonce - hashesAtStart;
			hashStart = std::chrono::steady_clock::now();
			hashesAtStart = nonce;

			console.Write("\r" + std::to_string(std::round(since(startTime).count() * 1000)) + " :	" + std::to_string(nonce) + " # " + hash);
			console.Write("   " + FormatHPS(hashesPerSecond) + "            ");
		}


		nonce++;
		char sha256OutBuffer[65];
		sha256_string((char*)(lastHash + transactions + std::to_string(nonce)).c_str(), sha256OutBuffer);
		hash = sha256OutBuffer;
	}

	console.MiningPrint();
	console.WriteLine("Debug mined in " + std::to_string(std::round(since(startTime).count() * 1000)) + " s.");

	return 0;
}

//static Process ExecuteCommand(string command, string directory)
//{
//	ProcessStartInfo ProcessInfo;
//	Process proc;
//
//	ProcessInfo = new ProcessStartInfo("cmd.exe", "/c " + command);
//	ProcessInfo.WorkingDirectory = directory;
//	ProcessInfo.CreateNoWindow = true;
//	ProcessInfo.WindowStyle = ProcessWindowStyle.Hidden;
//	ProcessInfo.UseShellExecute = true;
//
//	proc = Process.Start(ProcessInfo);
//
//	return proc;
//}

void ConnectionError()
{
	connectionStatus = 0;
	console.NetworkErrorPrint();
	console.WriteLine("Failed To Connect");
}

std::string ExecuteCommand(const char* cmd)
{
	//console.WriteLine("Rust Compiler: ", console.Rust());

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

	//STARTUPINFO sinfo = { 0 };
	//PROCESS_INFORMATION pinfo = { 0 };
	//if (!CreateProcess(cmd, buf, NULL, NULL, FALSE, 0, NULL, NULL, &sinfo, &pinfo)) {
	//	Fail("Could not launch Vim");
	//}
	//if (WaitForSingleObject(pinfo.hProcess, INFINITE) == WAIT_FAILED) {
	//	Fail("WaitForSingleObject");
	//}

	//system(("start " + (string)(cmd)).c_str());
	//console.WriteLine("Done Compiling", console.Rust());
	return "";
}

boost::process::child ExecuteAsync(std::string cmd)
{
	namespace bp = boost::process;
	std::vector<std::string> splitCommand = SplitString(cmd, " ");
	std::string command = splitCommand[0];
	std::string args;
	for (int i = 0; i < sizeof(splitCommand) / sizeof(splitCommand[0]); i++)
	{
		args += splitCommand[i];
	}
	bp::child c(bp::search_path(cmd), args);

	std::cout << c.id() << std::endl;
	// ... do something with ID ...

	return c;
	//c.wait();
}


json UpgradeBlock(json b, std::string toVersion)
{
	console.BlockCheckerPrint();
	console.WriteLine("Upgrading block to version " + toVersion);

	if (toVersion == "v0.01alpha-coin")
	{
		b["Version"] = toVersion;
	}

	return b;
}

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

double round(float value, int decimal_places)
{
	const double multiplier = std::pow(10.0, decimal_places);
	return std::round(value * multiplier) / multiplier;
}


//bool isAccessibleDir(string pathname)
//{
//	if (stat(pathname.c_str(), &info) != 0) {
//		console.WriteLine("cannot access directory \"" + pathname + "\"", console.Error());
//		return false;
//	}
//	else if (info.st_mode & S_IFDIR) {
//		return true;
//	}
//	else {
//		console.WriteLine("\"" + pathname + "\"" + " not a directory" + pathname, console.Error());
//		return false;
//	}
//}
