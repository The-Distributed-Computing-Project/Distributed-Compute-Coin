/*       Client Program      */

using System;
using System.IO;
using System.IO.Compression;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Globalization;
using System.Diagnostics;
using Newtonsoft.Json;

public class Block
{
    public string Version { get; set; }
    public string LastHash { get; set; }
    public string Hash { get; set; }
    public string Nonce { get; set; }
    public string Time { get; set; }
    public string[] Transactions { get; set; }
    public string[] TransactionTimes { get; set; }


    public static BlockchainUpgrader upgrader = new BlockchainUpgrader();

    public void Upgrade(string toVersion)
    {
        upgrader.Upgrade(this, toVersion);
    }
}

public class WalletInfo
{
    public string Address { get; set; }
    public double Balance { get; set; }
    public double PendingBalance { get; set; }
    public int BlockchainLength { get; set; }
    public int PendingLength { get; set; }
    public string MineDifficulty { get; set; }
    public float CostPerMinute { get; set; }
}

public class ProgramConfig
{
    public string Zip { get; set; }
    public double TotalMinutes { get; set; }
    public string Author { get; set; }
    public double MinutesLeft { get; set; }
    public int ComputationLevel { get; set; }
    public double Cost { get; set; }
    public bool Built { get; set; }
}

public class Http
{
    public string blockVersion { get; set; }

    public string StartHttpWebRequest(string URL, string[] args_vals)
    {
        string html = string.Empty;

        string url = URL;
        for (int i = 0; i < args_vals.Length; i++)
        {
            if (i > 0)
                url += "&";
            url += args_vals[i];
        }
        url += "&Version=" + blockVersion;

        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }

        return html;
    }
}


public class clnt
{
    static string id = "";
    public static WalletInfo walletInfo = new WalletInfo();
    public static int connectionStatus = 1;
    public static string[] directoryList = new string[] { "./wwwdata/blockchain", "./wwwdata/pendingblocks", "./wwwdata/programs" };

    public static ProgramConfig programConfig = new ProgramConfig();

    public static string blockVersion = "v0.01alpha-coin";


    public static void Main()
    {
        foreach (var dir in directoryList)
            if (!Directory.Exists(dir))
                Directory.CreateDirectory(dir);

        if (!File.Exists("./config.cfg"))
        {
            StreamWriter configFile = new StreamWriter("./config.cfg");
            configFile.Write("");
            configFile.Close();
        }
        if (!Directory.Exists("./wwwdata/programs/"))
        {
            Directory.CreateDirectory("./wwwdata/programs/");
        }

        string configFileRead = File.ReadAllText("./config.cfg");
        if (configFileRead.Length > 4)
        {
            walletInfo.Address = configFileRead.Split('\n')[0].Trim();
        }
        else
        {
            Console.Write("Enter your payout wallet : ");
            walletInfo.Address = Console.ReadLine();

            Console.Write("Stay logged in? Y/N : ");
            string stayLoggedIn = Console.ReadLine();
            if (stayLoggedIn.ToUpper() == "Y")
            {
                StreamWriter configFile = new StreamWriter("./config.cfg");
                configFile.Write(walletInfo.Address);
                configFile.Close();
            }
        }

        while (true)
        {
            WalletInfo w = GetInfo();

            if (w == null)
            {
                ConnectionError();
            }
            else
                walletInfo = w;

            if (connectionStatus == 1)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("There are " + walletInfo.PendingLength.ToString() + " Blocks to compute");
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("The difficulty is " + walletInfo.MineDifficulty.Length);
                Console.ResetColor();
            }
            else
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.Write("\nThere are ");
                Console.ForegroundColor = ConsoleColor.Red;
                Console.Write("unknown");
                Console.ForegroundColor = ConsoleColor.Green;
                Console.Write(" Blocks to compute\n");
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("The difficulty is unknown");
                Console.ResetColor();
            }

            Console.Write("What do you want to do :  ");
            String command = Console.ReadLine();

            if (connectionStatus == 0)
                continue;

            if (command.ToUpper() == "HELP")
                Help();
            if (command.ToUpper() == "SYNC")
            {
                if (Sync() == 0)
                {
                    ConnectionError();
                    continue;
                }
                Console.Write(((char)7).ToString());
            }
            if (command.ToUpper().Contains("MINE"))
            {
                int iterations = 1;
                if (command.Split(" ").Length > 1)
                    iterations = int.Parse(command.Split(" ")[1]);

                for (int i = 0; i < iterations; i++)
                {
                    foreach (string oldBlock in Directory.GetFiles("./wwwdata/pendingblocks/", "*.*", SearchOption.TopDirectoryOnly))
                    {
                        try
                        {
                            File.Delete(oldBlock);
                        }
                        catch (Exception)
                        {
                        }
                    }
                    for (int s = 0; s < walletInfo.PendingLength; s++)
                    {
                        if (SyncPending(walletInfo.BlockchainLength + 1 + s) == 0)
                        {
                            ConnectionError();
                            break;
                        }
                    }

                    if (!IsChainValid())
                    {
                        foreach (string oldBlock in Directory.GetFiles("./wwwdata/blockchain/", "*.*", SearchOption.TopDirectoryOnly))
                        {
                            try
                            {
                                File.Delete(oldBlock);
                            }
                            catch (Exception)
                            {
                            }
                        }
                        for (int a = 0; a < walletInfo.BlockchainLength; a++)
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

                    Console.WriteLine((walletInfo.BlockchainLength + 1).ToString());

                    string content = File.ReadAllText("./wwwdata/pendingblocks/block" + (walletInfo.BlockchainLength + 1).ToString() + ".dccblock");
                    Block o = JsonConvert.DeserializeObject<Block>(content);
                    string transactions = JoinArrayPieces(o.Transactions);
                    string lastHash = o.LastHash;
                    if (Mine(lastHash, transactions, (walletInfo.BlockchainLength + 1)) == 0)
                    {
                        ConnectionError();
                        continue;
                    }

                    walletInfo = GetInfo();
                    if (walletInfo == null)
                    {
                        ConnectionError();
                        continue;
                    }
                }
            }
            if (command.ToUpper().Contains("MINEANY"))
            {
                string diff = "";
                if (command.Split(" ").Length == 3)
                    diff = command.Split(" ")[2];
                MineAnyBlock(int.Parse(command.Split(" ")[1]), diff);
            }
            connectionStatus = 1;
        }
    }

    static void Help()
    {
        Console.WriteLine(@"
    Miner commands:
    
        mine [amount]
            mines [amount] number of blocks, will only mine 1 by default if [amount] is not specified
            
        mineany [block number] [difficulty]
            hashes [block number] with the given [difficulty]. this is for debugging purposes,
            and will not actually submit the block
            
        help
            displays this dialogue

");
    }

    static int Sync()
    {
        try
        {
            foreach (string oldBlock in Directory.GetFiles("./wwwdata/pendingblocks/", "*.*", SearchOption.TopDirectoryOnly))
            {
                try
                {
                    File.Delete(oldBlock);
                }
                catch (Exception)
                {
                }
            }
            foreach (string oldBlock in Directory.GetFiles("./wwwdata/blockchain/", "*.*", SearchOption.TopDirectoryOnly))
            {
                try
                {
                    File.Delete(oldBlock);
                }
                catch (Exception)
                {
                }
            }
            for (int i = 1; i < walletInfo.PendingLength + 1; i++)
            {
                SyncPending(walletInfo.BlockchainLength + i);
            }
            for (int i = 1; i < walletInfo.BlockchainLength + 1; i++)
            {
                SyncBlock(i);
            }
            GetProgram();
            return 1;
        }
        catch (Exception)
        {
            return 0;
        }
    }

    static WalletInfo GetInfo()
    {
        try
        {
            Http http = new Http();
            http.blockVersion = blockVersion;
            string[] args = new string[] { "query=getInfo", "fromAddress=" + walletInfo.Address };
            string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

            string content = html.Trim();
            return JsonConvert.DeserializeObject<WalletInfo>(content);
        }
        catch (Exception)
        {
            return null;
        }
    }

    static ProgramConfig ReadProgramConfig()
    {
        string content = File.ReadAllText("./wwwdata/programs/" + id + ".cfg");
        return JsonConvert.DeserializeObject<ProgramConfig>(content);
    }

    static int WriteProgramConfig()
    {
        try
        {
            File.WriteAllText("./wwwdata/programs/" + id + ".cfg", JsonConvert.SerializeObject(programConfig));
            return 1;
        }
        catch (Exception)
        {
            return 0;
        }
    }

    static int GetProgram()
    {
        string[] programFiles = Directory.GetFiles("./wwwdata/programs/");

        float life = 0;
        foreach (string item in programFiles)
        {
            if (item.Contains(".cfg"))
            {
                id = item.Split(".cfg")[0].Split("/programs/")[1];
                life = GetProgramLifeLeft(id);
                Console.WriteLine("Program life is " + life);
            }
        }

        try
        {
            if (life <= 0)
            {
                foreach (string oldProgram in Directory.GetFiles("./wwwdata/programs/", "*.*", SearchOption.TopDirectoryOnly))
                {
                    try
                    {
                        File.Delete(oldProgram);
                    }
                    catch (Exception)
                    {
                    }
                }
                foreach (string oldProgram in Directory.GetDirectories("./wwwdata/programs/", "*.*", SearchOption.TopDirectoryOnly))
                {
                    try
                    {
                        Directory.Delete(oldProgram, true);
                    }
                    catch (Exception)
                    {
                    }
                }

                Http http = new Http();
                http.blockVersion = blockVersion;
                string[] args = new string[] { "query=assignProgram" };
                string assignedProgram = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

                Console.ForegroundColor = ConsoleColor.Magenta;
                Console.WriteLine("Assigning Program...");
                Console.ResetColor();

                id = assignedProgram;

                Console.WriteLine("./wwwdata/programs/" + id + ".cfg");
                using (var client = new WebClient())
                {
                    client.DownloadFile(@"http://api.achillium.us.to/dcc/programs/" + id + ".cfg", "./wwwdata/programs/" + id + ".cfg");
                    client.DownloadFile(@"http://api.achillium.us.to/dcc/programs/" + id + ".zip", "./wwwdata/programs/" + id + ".zip");
                }

                if (!Directory.Exists("./wwwdata/programs/" + id))
                    ZipFile.ExtractToDirectory("./wwwdata/programs/" + id + ".zip", "./wwwdata/programs/" + id);

                if (!File.Exists("./wwwdata/programs/" + id + "/Cargo.toml"))
                {
                    Directory.Move(Directory.GetDirectories("./wwwdata/programs/" + id)[0], "./wwwdata/programs/tmpdir");
                    Directory.Delete("./wwwdata/programs/" + id, true);
                    Directory.Move("./wwwdata/programs/tmpdir", "./wwwdata/programs/" + id);
                }
            }

            string ourHash = sha256File("./wwwdata/programs/" + id + ".zip");

            Http http1 = new Http();
            http1.blockVersion = blockVersion;
            string[] args1 = new string[] { "query=hashProgram", "programID=" + id };
            string theirHash = http1.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args1);

            if (ourHash != theirHash)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Assigned program has been modified, re-downloading");
                Console.ResetColor();
                GetProgram();
            }

            programConfig = ReadProgramConfig();

            if (programConfig.Built == false)
            {
                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.WriteLine("Building assigned program, wait until it's finished to start mining");
                Console.ResetColor();

                Process proc = ExecuteCommand("cargo build", "./wwwdata/programs/" + id + "/");
                while (!proc.HasExited) { }
                programConfig.Built = true;
                WriteProgramConfig();
            }
            return 1;
        }
        catch (Exception)
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
            string[] args = new string[] { "query=getProgramLifeLeft", "programID=" + id };
            string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

            if (html.Contains("ERR") || html == string.Empty || html == null)
                return -100;
            return float.Parse(html.Trim());
        }
        catch (Exception)
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
            string[] args = new string[] { "query=getPendingBlock", "blockNum=" + whichBlock };
            string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

            Console.WriteLine("Synced pending: " + whichBlock);
            File.WriteAllText("./wwwdata/pendingblocks/block" + whichBlock.ToString() + ".dccblock", html);
            return 1;
        }
        catch (Exception)
        {
            return 0;
        }
    }

    static int SyncBlock(int whichBlock)
    {
        try
        {
            Http http = new Http();
            http.blockVersion = blockVersion;
            string[] args = new string[] { "query=getBlock", "blockNum=" + whichBlock };
            string html = http.StartHttpWebRequest("http://api.achillium.us.to/dcc/?", args);

            Console.WriteLine("Synced: " + whichBlock);
            File.WriteAllText("./wwwdata/blockchain/block" + whichBlock.ToString() + ".dccblock", html);
            return 1;
        }
        catch (Exception)
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

            Console.WriteLine("Validating block " + i);
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
        //Console.Clear();
        Console.BackgroundColor = ConsoleColor.DarkMagenta;
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write("Mining ");
        Console.BackgroundColor = ConsoleColor.DarkRed;
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write("block " + blockNum);
        Console.BackgroundColor = ConsoleColor.DarkMagenta;
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write(" at difficulty ");
        Console.BackgroundColor = ConsoleColor.DarkRed;
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write(walletInfo.MineDifficulty);
        Console.BackgroundColor = ConsoleColor.DarkMagenta;
        Console.ForegroundColor = ConsoleColor.White;
        Console.Write(" :");
        Console.ResetColor();
        Console.Write("\n");
        try
        {
            DateTime startTime = DateTime.UtcNow;

            var watch = new System.Diagnostics.Stopwatch();
            watch.Start();
            Process proc = ExecuteCommand("cargo run", "./wwwdata/programs/" + id + "/");

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

                    Console.Write("\r" + (DateTime.UtcNow - startTime).ToString().Split(".")[0] + " :	" + nonce.ToString() + " # " + hash);
                    Console.Write("   " + FormatHPS(hashesPerSecond) + "            ");
                }


                nonce++;
                hash = sha256(lastHash + transactionHistory + nonce.ToString());

                if (proc.HasExited)
                    watch.Stop();
            }

            if (!proc.HasExited)
            {
                proc.WaitForExit(10000);
                while (!proc.HasExited) { }
                watch.Stop();
            }

            Console.Clear();

            string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + blockNum.ToString() + "&nonce=" + nonce.ToString() + "&minedHash=" + sha256(lastHash + transactionHistory + nonce.ToString()) + "&fromAddress=" + walletInfo.Address + "&programID=" + id + "&time=" + watch.ElapsedMilliseconds / 1000f + "&Version=" + blockVersion;

            System.Net.WebClient Client = new System.Net.WebClient();
            Client.Headers.Add("Content-Type", "binary/octet-stream");
            byte[] result = Client.UploadFile(url, "POST", "./wwwdata/programs//" + id + "/out.txt");
            string s = System.Text.Encoding.UTF8.GetString(result, 0, result.Length);

            Console.ForegroundColor = ConsoleColor.Blue;
            Console.WriteLine(s + " in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
            Console.ResetColor();

            return 1;
        }
        catch (Exception)
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
            Console.WriteLine((DateTime.UtcNow - startTime).ToString().Split(".")[0] + " :	" + nonce.ToString() + " # " + sha256(lastHash + transactions + nonce.ToString()));
        }

        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine(" in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
        Console.ResetColor();
    }

    static Process ExecuteCommand(string command, string directory)
    {
        ProcessStartInfo ProcessInfo;
        Process proc;

        ProcessInfo = new ProcessStartInfo("cmd.exe", "/c " + command);
        ProcessInfo.WorkingDirectory = directory;
        ProcessInfo.CreateNoWindow = true;
        ProcessInfo.WindowStyle = ProcessWindowStyle.Hidden;
        ProcessInfo.UseShellExecute = true;

        proc = Process.Start(ProcessInfo);

        return proc;
    }

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
        foreach (byte theByte in crypto)
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
        foreach (byte theByte in crypto)
        {
            hash.Append(theByte.ToString("x2"));
        }
        return hash.ToString();
    }

    static string JoinArrayPieces(string[] input)
    {
        string outStr = "";
        foreach (string str in input)
        {
            outStr += str;
        }
        return outStr;
    }

    static void ConnectionError()
    {
        connectionStatus = 0;
        Console.ForegroundColor = ConsoleColor.Red;
        Console.WriteLine("Failed To Connect");
        Console.ResetColor();
    }
}
