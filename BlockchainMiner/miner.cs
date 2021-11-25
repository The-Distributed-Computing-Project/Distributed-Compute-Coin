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

class Block
{
    public string LastHash { get; set; }
    public string Hash { get; set; }
    public string Nonce { get; set; }
    public string Time { get; set; }
    public string[] Transactions { get; set; }
    public string[] TransactionTimes { get; set; }
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


public class clnt
{
    static string id = "";
    public static WalletInfo walletInfo = new WalletInfo();
    public static int connectionStatus = 1;

    public static void Main()
    {
        if (!File.Exists("./config.cfg"))
        {
            StreamWriter configFile = new StreamWriter("./config.cfg");
            configFile.Write("");
            configFile.Close();
        }
        if (!Directory.Exists("./programs"))
        {
            Directory.CreateDirectory("./programs");
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

            if(connectionStatus == 1)
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
                    foreach (string oldBlock in Directory.GetFiles("./pendingblocks/", "*.*", SearchOption.TopDirectoryOnly))
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

                    while (Directory.GetFiles("./blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length < walletInfo.BlockchainLength)
                    {
                        if (SyncBlock(Directory.GetFiles("./blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length + 1) == 0)
                        {
                            ConnectionError();
                            break;
                        }
                    }

                    if (!IsChainValid())
                    {
                        foreach (string oldBlock in Directory.GetFiles("./blockchain/", "*.*", SearchOption.TopDirectoryOnly))
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

                    string content = File.ReadAllText("./pendingblocks/block" + (walletInfo.BlockchainLength + 1).ToString() + ".dccblock");
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
            foreach (string oldBlock in Directory.GetFiles("./pendingblocks/", "*.*", SearchOption.TopDirectoryOnly))
            {
                try
                {
                    File.Delete(oldBlock);
                }
                catch (Exception)
                {
                }
            }
            foreach (string oldBlock in Directory.GetFiles("./blockchain/", "*.*", SearchOption.TopDirectoryOnly))
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
            string html = string.Empty;
            string url = @"http://api.achillium.us.to/dcc/?query=getInfo&fromAddress=" + walletInfo.Address;

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            request.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                html = reader.ReadToEnd();
            }

            string content = html.Trim();
            return JsonConvert.DeserializeObject<WalletInfo>(content);
        }
        catch (Exception)
        {
            return null;
        }
    }

    static int GetProgram()
    {
        string[] programFiles = Directory.GetFiles("./programs/");

        float life = 0;
        foreach (string item in programFiles)
        {
            if (item.Contains(".cfg"))
            {
                id = item.Split(".cfg")[0].Split("/programs/")[1];
                life = GetProgramLifeLeft(id);
            }
        }

        foreach (string oldProgram in Directory.GetFiles("./programs/", "*.*", SearchOption.TopDirectoryOnly))
        {
            try
            {
                File.Delete(oldProgram);
            }
            catch (Exception)
            {
            }
        }
        foreach (string oldProgram in Directory.GetDirectories("./programs/", "*.*", SearchOption.TopDirectoryOnly))
        {
            try
            {
                Directory.Delete(oldProgram, true);
            }
            catch (Exception)
            {
            }
        }

        try
        {
            string assignedProgram = string.Empty;
            string url = @"http://api.achillium.us.to/dcc/?query=assignProgram";

            Console.ForegroundColor = ConsoleColor.Magenta;
            Console.WriteLine("Assigning Program...");
            Console.ResetColor();
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            request.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                assignedProgram = reader.ReadToEnd();
            }
            id = assignedProgram;

            Console.WriteLine("./programs/" + id + ".cfg");
            using (var client = new WebClient())
            {
                client.DownloadFile(@"http://api.achillium.us.to/dcc/programs/" + id + ".cfg", "./programs/" + id + ".cfg");
                client.DownloadFile(@"http://api.achillium.us.to/dcc/programs/" + id + ".zip", "./programs/" + id + ".zip");
            }

            if (!Directory.Exists("./programs/" + id))
                ZipFile.ExtractToDirectory("./programs/" + id + ".zip", "./programs/" + id);

            if (!File.Exists("./programs/" + id + "/Cargo.toml"))
            {
                Directory.Move(Directory.GetDirectories("./programs/" + id)[0], "./programs/tmpdir");
                Directory.Delete("./programs/" + id, true);
                Directory.Move("./programs/tmpdir", "./programs/" + id);
            }


            string ourHash = sha256File("./programs/" + id + ".zip");
            string theirHash = string.Empty;

            string hashUrl = @"http://api.achillium.us.to/dcc/?query=hashProgram&programID=" + id;

            HttpWebRequest hashRequest = (HttpWebRequest)WebRequest.Create(hashUrl);
            hashRequest.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)hashRequest.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                theirHash = reader.ReadToEnd();
            }

            if (ourHash != theirHash)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("Assigned program has been modified, re-downloading");
                Console.ResetColor();
                GetProgram();
            }

            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine("Building assigned program, wait finished to start mining");
            Console.ResetColor();

            Process proc = ExecuteCommand("cargo build", "./programs/" + id + "/");
            while (!proc.HasExited) { }
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
            string html = string.Empty;
            string url = @"http://api.achillium.us.to/dcc/?query=getProgramLifeLeft&programID=" + id;

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            request.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                html = reader.ReadToEnd();
            }

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
            string html = string.Empty;
            string url = @"http://api.achillium.us.to/dcc/?query=getPendingBlock&blockNum=" + whichBlock;

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            request.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                html = reader.ReadToEnd();
            }

            Console.WriteLine("Synced pending: " + whichBlock);

            File.WriteAllText("./pendingblocks/block" + whichBlock.ToString() + ".dccblock", html);
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
            string html = string.Empty;
            string url = @"http://api.achillium.us.to/dcc/?query=getBlock&blockNum=" + whichBlock;

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            request.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                html = reader.ReadToEnd();
            }

            Console.WriteLine("Synced: " + whichBlock);
            File.WriteAllText("./blockchain/block" + whichBlock.ToString() + ".dccblock", html);
            return 1;
        }
        catch (Exception)
        {
            return 0;
        }
    }

    static bool IsChainValid()
    {
        string[] blocks = Directory.GetFiles("./blockchain/", "*.dccblock");

        for (int i = 1; i < blocks.Length; i++)
        {
            string content = File.ReadAllText("./blockchain/block" + i + ".dccblock");
            Block o = JsonConvert.DeserializeObject<Block>(content);
            string[] trans = o.Transactions;

            string lastHash = o.LastHash;
            string currentHash = o.Hash;
            string nonce = o.Nonce;
            string transactions = JoinArrayPieces(trans);

            content = File.ReadAllText("./blockchain/block" + (i + 1) + ".dccblock");
            o = JsonConvert.DeserializeObject<Block>(content);
            string nextHash = o.LastHash;

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
        try
        {
            DateTime startTime = DateTime.UtcNow;

            var watch = new System.Diagnostics.Stopwatch();
            watch.Start();
            Process proc = ExecuteCommand("cargo run", "./programs/" + id + "/");

            //Checks Hash
            int nonce = 0;
            while (!sha256(lastHash + transactionHistory + nonce.ToString()).StartsWith(walletInfo.MineDifficulty))
            {
                nonce++;
                Console.WriteLine((DateTime.UtcNow - startTime).ToString().Split(".")[0] + " :	" + nonce.ToString() + " # " + sha256(lastHash + transactionHistory + nonce.ToString()));

                if (proc.HasExited)
                    watch.Stop();
            }

            if (!proc.HasExited)
            {
                proc.WaitForExit(10000);
                while (!proc.HasExited) { }
                watch.Stop();
            }
            Console.WriteLine(watch.ElapsedMilliseconds / 1000f);

            string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + blockNum.ToString() + "&nonce=" + nonce.ToString() + "&minedHash=" + sha256(lastHash + transactionHistory + nonce.ToString()) + "&fromAddress=" + walletInfo.Address + "&programID=" + id + "&time=" + watch.ElapsedMilliseconds / 1000f;

            System.Net.WebClient Client = new System.Net.WebClient();
            Client.Headers.Add("Content-Type", "binary/octet-stream");
            byte[] result = Client.UploadFile(url, "POST", "./programs/" + id + "/out.txt");
            string s = System.Text.Encoding.UTF8.GetString(result, 0, result.Length);

            Console.ForegroundColor = ConsoleColor.Blue;
            Console.WriteLine(s + " in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
            Console.ForegroundColor = ConsoleColor.Magenta;
            Console.WriteLine(url);
            Console.ResetColor();
            Console.Write(((char)7).ToString());

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

        string content = File.ReadAllText("./blockchain/block" + blockNum + ".dccblock");
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
