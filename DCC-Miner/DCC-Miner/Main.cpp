// DCC-Miner.cpp : Defines the entry point for the application.
//

#include "Main.h"

using namespace std;
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



string id = "";
json walletInfo;
int connectionStatus = 1;
const string directoryList[] = { "./wwwdata/blockchain", "./wwwdata/pendingblocks", "./wwwdata/programs" };

json programConfig;

const string blockVersion = "v0.01alpha-coin";

PHPServ phpserv = new PHPServ();
P2P p2p = new P2P();

struct stat info;

int main()
{
	for (string dir : directoryList)
		if (!fs::is_directory(dir) || !fs::exists(dir))
			fs::create_directory(dir);

	if (!fs::exists("./config.cfg"))
	{
		ofstream configFile("./config.cfg");
		if (configFile.is_open())
		{
			configFile << "";
			configFile.close();
		}
	}

	std::ifstream t("./config.cfg");
	std::stringstream buffer;
	buffer << t.rdbuf();
	string configFileRead = buffer.str();
	if (configFileRead.size() > 4)
	{
		walletInfo["Address"] = trim(SplitString(configFileRead, "\n")[0]);
	}
	else
	{
		Console().WriteDialogueAuthor(Console().Mining());
		Console().Write("Enter your payout wallet : ");
		walletInfo["Address"] = Console().ReadLine();

		Console().WriteDialogueAuthor(Console().Mining());
		Console().Write("Stay logged in? Y/N : ");
		string stayLoggedIn = Console().ReadLine();
		if (ToUpper(stayLoggedIn) == "Y")
		{
			ofstream configFile("./config.cfg");
			if (configFile.is_open())
			{
				configFile << walletInfo["Address"];
				configFile.close();
			}
		}
	}

	//phpserv.StartServer();
	//p2p.Connect("");

	while (true)
	{
		json w = GetInfo();

		if (w.is_null())
		{
			ConnectionError();
		}
		else
			walletInfo = w;

		if (connectionStatus == 1)
		{
			Console().WriteLine("There are " + (string)walletInfo["PendingLength"] + " Blocks to compute", Console().Mining());
			Console().WriteLine("The difficulty is " + ((string)walletInfo["MineDifficulty"]).size(), Console().Mining());
		}
		else
		{
			Console().WriteLine("Failed to connect", Console().NetworkError());

			Console().WriteLine("There are UNKNOWN Blocks to compute", Console().MiningError());
			Console().WriteLine("The difficulty is UNKNOWN", Console().MiningError());
		}

		Console().Write("DCC >  ");
		string command = Console().ReadLine();

		if (connectionStatus == 0)
			continue;

		if (SplitString(ToUpper(command), " ")[0] == "--HELP" || SplitString(ToUpper(command), " ")[0] == "-H")
			Help();
		if (SplitString(ToUpper(command), " ")[0] == "--SYNC" || SplitString(ToUpper(command), " ")[0] == "-S")
		{
			if (Sync() == 0)
			{
				ConnectionError();
				continue;
			}
			//Console().Write(((char)7).ToString()); // Bell char to make sound
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
						Console().WriteLine("Error removing \"" + oldBlock.path().string() + "\"", Console().Error());
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
							Console().WriteLine("Error removing \"" + oldBlock.path().string() + "\"", Console().Error());
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

				Console().WriteLine("Blockchain length: " + (walletInfo["BlockchainLength"] + 1), Console().Mining());

				std::ifstream blockFile("./wwwdata/pendingblocks/block" + to_string(walletInfo["BlockchainLength"] + 1) + ".dccblock");
				std::stringstream blockBuffer;
				blockBuffer << blockFile.rdbuf();
				string content = blockBuffer.str();

				json o = json::parse(content);
				string transactions = JoinArrayPieces((string[])o["Transactions"]);
				string lastHash = o["LastHash"];
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
			std::ifstream blockFile("./wwwdata/pendingblocks/block" + to_string(walletInfo["BlockchainLength"] + 1) + ".dccblock");
			std::stringstream blockBuffer;
			blockBuffer << blockFile.rdbuf();
			string content = blockBuffer.str();

			json o = json::parse(content);
			string transactions = JoinArrayPieces((string[])o["Transactions"]);
			string lastHash = o["LastHash"];

			string diff = "";
			if (SplitString(command, " ").size() == 3)
				diff = SplitString(command, " ")[2];
			MineAnyBlock(lastHash, transactions, stoi(SplitString(command, " ")[1]), diff);
		}
		connectionStatus = 1;
	}
}

static void Help()
{
	Console().WriteLine(R"V0G0N(

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

static int Sync()
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
				Console().WriteLine("Error removing \"" + oldBlock.path().string() + "\"", Console().Error());
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
				Console().WriteLine("Error removing \"" + oldBlock.path().string() + "\"", Console().Error());
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
	string html = p2p.Connect("index.php?query=getInfo&fromAddress=" + walletInfo["Address"]).Trim();
	return json::parse(html);
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
	string content = buffer.str();
	return json::parse(content);
}

static int WriteProgramConfig()
{
	try
	{
		ofstream configFile("./wwwdata/programs/" + id + ".cfg");
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

static int GetProgram()
{
	float life = 0;
	for (auto item : fs::directory_iterator("./wwwdata/programs/"))
	{
		if ((item.path().string()).find(".cfg") != std::string::npos)
		{
			id = SplitString(SplitString((item.path()).string(), ".cfg")[0], "/programs/")[1];
			life = GetProgramLifeLeft(id);
			Console().WriteLine("Program life is " + to_string(life));
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
					Console().WriteLine("Error removing \"" + oldProgram.path().string() + "\"", Console().Error());
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
			vector<string> args = { "query=assignProgram" };
			string assignedProgram = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

			Console().WriteLine("Assigning Program...", Console().Network());

			id = assignedProgram;

			Console().WriteLine("./wwwdata/programs/" + id + ".cfg", Console().Network());

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
		string ourHash = sha256OutBuffer;

		Http http1;
		http1.blockVersion = blockVersion;
		vector<string> args1 = { "query=hashProgram", "programID=" + id };
		string theirHash = http1.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args1);

		if (ourHash != theirHash)
		{
			Console().WriteLine("Assigned program has been modified, re-downloading...", Console().MiningError());
			GetProgram();
		}

		programConfig = ReadProgramConfig();

		if (programConfig["Built"] == false)
		{
			Console().WriteLine("Building assigned program, wait until it's finished to start mining", Console().Mining());

			Console().WriteLine("Compiling program... ", Console().Rust());
			ExecuteCommand(("cargo build ./wwwdata/programs/" + id + "/").c_str());
			Console().WriteLine("Done Compiling", Console().Rust());

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

float GetProgramLifeLeft(string id)
{
	try
	{
		Http http;
		http.blockVersion = blockVersion;
		vector<string> args = { "query=getProgramLifeLeft", "programID=" + id };
		string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

		if (html.find("ERR") != std::string::npos || html == "")
			return -100;
		return stof(trim(html));
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

		//Console().WriteLine("Synced pending: " + whichBlock, Console().Mining());
		//File.WriteAllText("./wwwdata/pendingblocks/block" + to_string(whichBlock) + ".dccblock", html);
		DownloadFile("http://api.achillium.us.to/dcc/?query=getPendingBlock&blockNum=" + to_string(whichBlock) + "&Version=" + blockVersion,
			"./wwwdata/pendingblocks/block" + to_string(whichBlock) + ".dccblock",
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

		//Console().WriteLine("Synced: " + whichBlock);
		//File.WriteAllText("./wwwdata/blockchain/block" + to_string(whichBlock) + ".dccblock", html);

		DownloadFile("http://api.achillium.us.to/dcc/?query=getBlock&blockNum=" + to_string(whichBlock) + "&Version=" + blockVersion,
			"./wwwdata/blockchain/block" + to_string(whichBlock) + ".dccblock",
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
		std::ifstream t("./wwwdata/blockchain/block" + to_string(i) + ".dccblock");
		std::stringstream buffer;
		buffer << t.rdbuf();
		string content = buffer.str();

		json o = json::parse(content);
		string trans[] = { o["Transactions"] };

		if (o["Version"] == nullptr || o["Version"] == "" || o["Version"] != blockVersion)
		{
			UpgradeBlock(o, blockVersion);
			ofstream blockFile("./wwwdata/blockchain/block" + to_string(i) + ".dccblock");
			if (blockFile.is_open())
			{
				blockFile << o.dump();
				blockFile.close();
			}
		}

		string lastHash = o["LastHash"];
		string currentHash = o["Hash"];
		string nonce = o["Nonce"];
		string transactions = JoinArrayPieces(trans);


		std::ifstream td("./wwwdata/blockchain/block" + to_string(i + 1) + ".dccblock");
		std::stringstream bufferd;
		bufferd << td.rdbuf();
		string nextBlockText = bufferd.str();
		o = json::parse(nextBlockText);

		string nextHash = o["LastHash"];

		if (o["Version"] == nullptr || o["Version"] == "" || o["Version"] != blockVersion)
		{
			UpgradeBlock(o, blockVersion);
			ofstream blockFile("./wwwdata/blockchain/block" + to_string(i + 1) + ".dccblock");
			if (blockFile.is_open())
			{
				blockFile << o.dump();
				blockFile.close();
			}
		}

		Console().WriteLine("Validating block: " + i, Console().BlockChecker());
		char sha256OutBuffer[65];
		sha256_string((char*)(lastHash + transactions + nonce).c_str(), sha256OutBuffer);
		string blockHash = sha256OutBuffer;
		if ((blockHash[0] != '0' && blockHash[1] != '0') || blockHash != currentHash || blockHash != nextHash)
		{
			return false;
		}
	}
	return true;
}

static int Mine(string lastHash, string transactionHistory, int blockNum)
{
	Console().WriteDialogueAuthor(Console().Mining());
	Console().Write("Mining ");
	Console().Write("block " + blockNum, Console().whiteBGColor, Console().blackFGColor);
	Console().Write(" at difficulty ");
	Console().Write(walletInfo["MineDifficulty"], Console().whiteBGColor, Console().blackFGColor);
	Console().Write(" :");
	Console().Write("\n");
	try
	{
		auto startTime = std::chrono::steady_clock::now();
		std::cout << "Elapsed(ms)=" << since(startTime).count() << std::endl;

		Console().WriteLine("Starting program... ", Console().Rust());
		boost::process::child cargoProc = ExecuteAsync("cargo run ./wwwdata/programs/" + id + "/");

		//Checks Hash
		int nonce = 0;
		string hash = "";
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

				Console().Write("\r" + to_string(std::round(since(startTime).count() * 1000)) + " :	" + to_string(nonce) + " # " + hash);
				Console().Write("   " + FormatHPS(hashesPerSecond) + "            ");
			}


			nonce++;
			char sha256OutBuffer[65];
			sha256_string((char*)(lastHash + transactionHistory + to_string(nonce)).c_str(), sha256OutBuffer);
			hash = sha256OutBuffer;

			if (!cargoProc.running())
				break;
		}

		if (cargoProc.running())
		{
			cargoProc.wait();
		}

		//Console().Clear();

		char sha256OutBuffer[65];
		sha256_string((char*)(lastHash + transactionHistory + to_string(nonce)).c_str(), sha256OutBuffer);
		hash = sha256OutBuffer;
		string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + to_string(blockNum) + "&nonce=" + to_string(nonce) + "&minedHash=" + hash + "&fromAddress=" + (string)walletInfo["Address"] + "&programID=" + id + "&time=" + to_string(since(startTime).count() / 1000.0f) + "&Version=" + blockVersion;

		string s = UploadFile(url, "./wwwdata/programs//" + id + "/out.txt");

		Console().WriteLine(s + " in " + to_string(std::round(since(startTime).count() * 1000)) + " s.", Console().Mining());

		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

static int MineAnyBlock(string lastHash, string transactionHistory, int blockNum, string difficulty)
{
	if (difficulty == "")
		difficulty = walletInfo["MineDifficulty"];

	auto startTime = std::chrono::steady_clock::now();

	std::ifstream td("./wwwdata/blockchain/block" + to_string(blockNum) + ".dccblock");
	std::stringstream bufferd;
	bufferd << td.rdbuf();
	string nextBlockText = bufferd.str();
	json o = json::parse(nextBlockText);

	string transactions = JoinArrayPieces((string[])o["Transactions"]);
	string currentHash = o["Hash"];
	string lastHash = o["LastHash"];

	//Checks Hash
	int nonce = 0;
	string hash = "";
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

			Console().Write("\r" + to_string(std::round(since(startTime).count() * 1000)) + " :	" + to_string(nonce) + " # " + hash);
			Console().Write("   " + FormatHPS(hashesPerSecond) + "            ");
		}


		nonce++;
		char sha256OutBuffer[65];
		sha256_string((char*)(lastHash + transactionHistory + to_string(nonce)).c_str(), sha256OutBuffer);
		hash = sha256OutBuffer;
	}

	Console().WriteLine("Debug mined in " + to_string(std::round(since(startTime).count() * 1000)) + " s.", Console().Mining());
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

static string FormatHPS(float input)
{
	if (input > 1000000000.0f)
		return to_string(round(input / 1000000000.0f, 3)) + " gH/s";
	else if (input > 1000000.0f)
		return to_string(round(input / 1000000.0f, 3)) + " mH/s";
	else if (input > 1000.0f)
		return to_string(round(input / 1000.0f, 3)) + " kH/s";
	else
		return to_string(round(input, 3)) + " H/s";
}

static string JoinArrayPieces(string input[])
{
	string outStr = "";
	for (int i = 0; i < sizeof(input) / sizeof(input[0]); i++)
	{
		outStr += input[i];
	}
	return outStr;
}

static void ConnectionError()
{
	connectionStatus = 0;
	Console().WriteLine(Console().colorText("Failed To Connect", Console().NetworkError()));
}

vector<string> SplitString(string str, string delim)
{
	vector<string> words{};

	size_t pos = 0;
	while ((pos = str.find(delim)) != string::npos) {
		words.push_back(str.substr(0, pos));
		str.erase(0, pos + delim.length());
	}
	for (const auto& str : words) {
		cout << str << endl;
	}
}

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}
// trim from both ends (in place)
static inline string trim(std::string s) {
	string sN = s;
	ltrim(sN);
	rtrim(sN);
	return sN;
}

// Convert string to uppercase
static inline string ToUpper(string s) {
	string sN = s;
	for (auto& c : sN) c = toupper(c);
	return sN;
}

std::string ExecuteCommand(const char* cmd) {
	//Console().WriteLine("Rust Compiler: ", Console().Rust());

	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw std::runtime_error("_popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
		cout << buffer.data();
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
	//Console().WriteLine("Done Compiling", Console().Rust());
	return "";
}

boost::process::child ExecuteAsync(string cmd) {
	namespace bp = boost::process;
	vector<string> splitCommand = SplitString(cmd, " ");
	string command = splitCommand[0];
	string args;
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


json UpgradeBlock(json b, string toVersion)
{
	Console().WriteLine("Upgrading block to version " + toVersion, Console().BlockChecker());

	if (toVersion == "v0.01alpha-coin")
	{
		b["Version"] = toVersion;
	}

	return b;
}

double round(float value, int decimal_places) {
	const double multiplier = std::pow(10.0, decimal_places);
	return std::round(value * multiplier) / multiplier;
}

template <
	class result_t = std::chrono::milliseconds,
	class clock_t = std::chrono::steady_clock,
	class duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start)
{
	return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

bool StringStartsWith(string str, string substr) {
	for (int i = 0; i < substr.length(); i++)
	{
		if (str[i] != substr[i])
			return false;
	}
	return true;
}

//bool isAccessibleDir(string pathname)
//{
//	if (stat(pathname.c_str(), &info) != 0) {
//		Console().WriteLine("cannot access directory \"" + pathname + "\"", Console().Error());
//		return false;
//	}
//	else if (info.st_mode & S_IFDIR) {
//		return true;
//	}
//	else {
//		Console().WriteLine("\"" + pathname + "\"" + " not a directory" + pathname, Console().Error());
//		return false;
//	}
//}