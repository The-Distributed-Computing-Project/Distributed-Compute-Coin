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

class Http
{
public:
	string blockVersion = "";

	string StartHttpWebRequest(string URL, vector<string> args_vals)
	{
		string html = "";

		string url = URL;
		for (int i = 0; i < args_vals.size(); i++)
		{
			if (i > 0)
				url += "&";
			url += args_vals.at(i);
		}
		if (blockVersion != "")
			url += "&Version=" + blockVersion;

		auto response = cpr::Get(cpr::Url{ url });
		html = response.text;

		Console().WriteLine(html, Console().Debug());

		return html;
	}
};


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
		Console().Write("Enter your payout wallet : ", Console().Mining());
		walletInfo["Address"] = Console().ReadLine();

		Console().Write("Stay logged in? Y/N : ", Console().Mining());
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
			string diff = "";
			if (SplitString(command, " ").size() == 3)
				diff = SplitString(command, " ")[2];
			MineAnyBlock(stoi(SplitString(command, " ")[1]), diff);
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

			ExecuteCommand(("cargo build ./wwwdata/programs/" + id + "/").c_str());

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

static float GetProgramLifeLeft(string id)
{
	try
	{
		Http http = new Http();
		http.blockVersion = blockVersion;
		string[] args = new string[]{ "query=getProgramLifeLeft", "programID=" + id };
		string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

		if (html.Contains("ERR") || html == string.Empty || html == null)
			return -100;
		return float.Parse(html.Trim());
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

static int SyncPending(int whichBlock)
{
	try
	{
		Http http = new Http();
		http.blockVersion = blockVersion;
		string[] args = new string[]{ "query=getPendingBlock", "blockNum=" + whichBlock };
		string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

		Console().WriteLine("Synced pending: " + whichBlock);
		File.WriteAllText("./wwwdata/pendingblocks/block" + whichBlock.ToString() + ".dccblock", html);
		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

int DownloadFile(string url, string saveAs) {
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		fp = fopen(saveAs.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	return 0;
}
int DownloadFile(string url, string saveAs, bool printStatus) {
	CURL* curl;
	FILE* fp;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		if (printStatus)
			Console().Write("Downloading from: \"" + url + "\" ...\r", Console().Network());
		fp = fopen(saveAs.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
		if (printStatus)
			Console().Write("Downloading from: \"" + url + "\" Done\n", Console().Network());
	}
	return 0;
}

static int SyncBlock(int whichBlock)
{
	try
	{
		Http http = new Http();
		http.blockVersion = blockVersion;
		string[] args = new string[]{ "query=getBlock", "blockNum=" + whichBlock };
		string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

		Console().WriteLine("Synced: " + whichBlock);
		File.WriteAllText("./wwwdata/blockchain/block" + whichBlock.ToString() + ".dccblock", html);
		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

static bool IsChainValid()
{
	while (Directory.GetFiles("./wwwdata/blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length < walletInfo.BlockchainLength)
	{
		if (SyncBlock(Directory.GetFiles("./wwwdata/blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length + 1) == 0)
		{
			ConnectionError();
			break;
		}
	}

	string[] blocks = Directory.GetFiles("./wwwdata/blockchain/", "*.dccblock");

	for (int i = 1; i < blocks.Length; i++)
	{
		string content = File.ReadAllText("./wwwdata/blockchain/block" + i + ".dccblock");
		Block o = JsonConvert.DeserializeObject<Block>(content);
		string[] trans = o.Transactions;

		if (o.Version == null || o.Version == "" || o.Version != blockVersion)
		{
			o.Upgrade(blockVersion);
			File.WriteAllText("./wwwdata/blockchain/block" + i + ".dccblock", JsonConvert.SerializeObject(o));
		}

		string lastHash = o.LastHash;
		string currentHash = o.Hash;
		string nonce = o.Nonce;
		string transactions = JoinArrayPieces(trans);

		content = File.ReadAllText("./wwwdata/blockchain/block" + (i + 1) + ".dccblock");
		o = JsonConvert.DeserializeObject<Block>(content);
		string nextHash = o.LastHash;

		if (o.Version == null || o.Version == "" || o.Version != blockVersion)
		{
			o.Upgrade(blockVersion);
			File.WriteAllText("./wwwdata/blockchain/block" + (i + 1) + ".dccblock", JsonConvert.SerializeObject(o));
		}

		Console().WriteLine("Validating block " + i);
		string blockHash = sha256(lastHash + transactions + nonce);
		if (!blockHash.StartsWith("00") || blockHash != currentHash || blockHash != nextHash)
		{
			return false;
		}
	}
	return true;
}

static int Mine(string lastHash, string transactionHistory, int blockNum)
{
	//Console().Clear();
	Console().BackgroundColor = ConsoleColor.DarkMagenta;
	Console().ForegroundColor = ConsoleColor.White;
	Console().Write("Mining ");
	Console().BackgroundColor = ConsoleColor.DarkRed;
	Console().ForegroundColor = ConsoleColor.White;
	Console().Write("block " + blockNum);
	Console().BackgroundColor = ConsoleColor.DarkMagenta;
	Console().ForegroundColor = ConsoleColor.White;
	Console().Write(" at difficulty ");
	Console().BackgroundColor = ConsoleColor.DarkRed;
	Console().ForegroundColor = ConsoleColor.White;
	Console().Write(walletInfo.MineDifficulty);
	Console().BackgroundColor = ConsoleColor.DarkMagenta;
	Console().ForegroundColor = ConsoleColor.White;
	Console().Write(" :");
	Console().ResetColor();
	Console().Write("\n");
	try
	{
		DateTime startTime = DateTime.UtcNow;

		var watch = new System.Diagnostics.Stopwatch();
		watch.Start();
		ExecuteCommand(("cargo run ./wwwdata/programs/" + id + "/").c_str());

		//Checks Hash
		int nonce = 0;
		string hash = "";
		DateTime hashStart = DateTime.Now;
		int hashesPerSecond = 0;
		int hashesAtStart = 0;
		while (!hash.StartsWith(walletInfo.MineDifficulty))
		{
			if ((DateTime.Now - hashStart).TotalSeconds >= 1)
			{
				hashesPerSecond = nonce - hashesAtStart;
				hashStart = DateTime.Now;
				hashesAtStart = nonce;

				Console().Write("\r" + (DateTime.UtcNow - startTime).ToString().Split(".")[0] + " :	" + nonce.ToString() + " # " + hash);
				Console().Write("   " + FormatHPS(hashesPerSecond) + "            ");
			}


			nonce++;
			hash = sha256(lastHash + transactionHistory + nonce.ToString());

			if (proc.HasExited)
				watch.Stop();
		}

		if (!proc.HasExited)
		{
			proc.WaitForExit(10000);
			while (!proc.HasExited) {}
			watch.Stop();
		}

		Console().Clear();

		string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + blockNum.ToString() + "&nonce=" + nonce.ToString() + "&minedHash=" + sha256(lastHash + transactionHistory + nonce.ToString()) + "&fromAddress=" + walletInfo.Address + "&programID=" + id + "&time=" + watch.ElapsedMilliseconds / 1000f + "&Version=" + blockVersion;

		System.Net.WebClient Client = new System.Net.WebClient();
		Client.Headers.Add("Content-Type", "binary/octet-stream");
		byte[] result = Client.UploadFile(url, "POST", "./wwwdata/programs//" + id + "/out.txt");
		string s = System.Text.Encoding.UTF8.GetString(result, 0, result.Length);

		Console().ForegroundColor = ConsoleColor.Blue;
		Console().WriteLine(s + " in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
		Console().ResetColor();

		return 1;
	}
	catch (const std::exception&)
	{
		return 0;
	}
}

static void MineAnyBlock(int blockNum, string difficulty)
{
	if (difficulty == null || difficulty == "")
		difficulty = walletInfo.MineDifficulty;

	DateTime startTime = DateTime.UtcNow;

	string content = File.ReadAllText("./wwwdata/blockchain/block" + blockNum + ".dccblock");
	Block o = JsonConvert.DeserializeObject<Block>(content);
	string transactions = JoinArrayPieces(o.Transactions);
	string currentHash = o.Hash;
	string lastHash = o.LastHash;

	//Checks Hash
	int nonce = 0;
	while (!sha256(lastHash + transactions + nonce.ToString()).StartsWith(difficulty))
	{
		nonce++;
		Console().WriteLine((DateTime.UtcNow - startTime).ToString().Split(".")[0] + " :	" + nonce.ToString() + " # " + sha256(lastHash + transactions + nonce.ToString()));
	}

	Console().ForegroundColor = ConsoleColor.Blue;
	Console().WriteLine(" in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
	Console().ResetColor();
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
	if (input > 1000000000f)
		return Math.Round(input / 1000000000f, 3).ToString() + " gH/s";
	else if (input > 1000000f)
		return Math.Round(input / 1000000f, 3).ToString() + " mH/s";
	else if (input > 1000f)
		return Math.Round(input / 1000f, 3).ToString() + " kH/s";
	else
		return Math.Round(input, 3).ToString() + " H/s";
}

static string sha256(string input)
{
	var crypt = new System.Security.Cryptography.SHA256Managed();
	var hash = new System.Text.StringBuilder();
	byte[] crypto = crypt.ComputeHash(Encoding.UTF8.GetBytes(input));
	foreach(byte theByte in crypto)
	{
		hash.Append(theByte.ToString("x2"));
	}
	return hash.ToString();
}

static string sha256File(string fileLocation)
{
	byte[] fileBytes = File.ReadAllBytes(fileLocation);

	var crypt = new System.Security.Cryptography.SHA256Managed();
	var hash = new System.Text.StringBuilder();
	byte[] crypto = crypt.ComputeHash(fileBytes);
	foreach(byte theByte in crypto)
	{
		hash.Append(theByte.ToString("x2"));
	}
	return hash.ToString();
}

static string JoinArrayPieces(string input[])
{
	string outStr = "";
	for (string str : input)
	{
		outStr += str;
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
	Console().WriteLine("Rust Compiler: ", Console().Compiler());

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
	Console().WriteLine("Done Compiling", Console().Compiler());
	return result;
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