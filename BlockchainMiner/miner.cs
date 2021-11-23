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
class WalletInfo
{
    public string Address { get; set; }
    public float Balance { get; set; }
    public float PendingBalance { get; set; }
    public int BlockchainLength { get; set; }
    public int PendingLength { get; set; }
    public string MineDifficulty { get; set; }
    public float CostPerMinute { get; set; }
}
public class clnt
{
    static string wallet = null;
    static string username = null;
    static string password = null;
    static int pendingLength = 0;
    static int blockChainLength = 0;
    static int totalBlocks = 0;
    static string lengths = null;
    static string id = "";
    static string mineLevel;

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
            wallet = configFileRead.Split('\n')[0].Trim();
        }
        else
        {
            Console.Write("Enter your payout wallet : ");
            wallet = Console.ReadLine();

            Console.Write("Stay logged in? Y/N : ");
            string stayLoggedIn = Console.ReadLine();
            if (stayLoggedIn.ToUpper() == "Y")
            {
                StreamWriter configFile = new StreamWriter("./config.cfg");
                configFile.Write(wallet);
                configFile.Close();
            }
        }

        //wallet = "dcc" + sha256(username + password);

        while (true)
        {
            lengths = GetLength();
            mineLevel = GetMineLevel();
            try
            {
                pendingLength = int.Parse(lengths.Split("##")[0]);
                blockChainLength = int.Parse(lengths.Split("##")[1]);
            }
            catch (Exception)
            {
                pendingLength = 0;
                blockChainLength = 0;
            }
            totalBlocks = pendingLength + blockChainLength;
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("There are " + pendingLength.ToString() + " Blocks to compute");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("The difficulty is " + mineLevel.Length);
            Console.ResetColor();

            Console.Write("What do you want to do :  ");
            String command = Console.ReadLine();
            if (command.ToUpper() == "HELP")
                Help();
            if (command.ToUpper() == "SYNC")
            {
                Sync();
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
                    for (int s = 0; s < pendingLength; s++)
                    {
                        SyncPending(blockChainLength + 1 + s);
                    }

                    while (Directory.GetFiles("./blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length < blockChainLength)
                    {
                        SyncBlock(Directory.GetFiles("./blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length + 1);
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
                        for (int a = 0; a < blockChainLength; a++)
                        {
                            SyncBlock(1 + a);
                        }
                    }

                    GetProgram();

                    Console.WriteLine((blockChainLength + 1).ToString());

                    string content = File.ReadAllText("./pendingblocks/block" + (blockChainLength + 1).ToString() + ".txt");
                    Block o = JsonConvert.DeserializeObject<Block>(content);
                    string transactions = JoinArrayPieces(o.Transactions);
                    string lastHash = o.LastHash;
                    Mine(lastHash, transactions, (blockChainLength + 1));


                    lengths = GetLength();
                    mineLevel = GetMineLevel();
                    try
                    {
                        pendingLength = int.Parse(lengths.Split("##")[0]);
                        blockChainLength = int.Parse(lengths.Split("##")[1]);
                    }
                    catch (Exception)
                    {
                        pendingLength = 0;
                        blockChainLength = 0;
                    }
                    totalBlocks = pendingLength + blockChainLength;
                }
            }
            if (command.ToUpper().Contains("MINEANY"))
            {
                string diff = "";
                if (command.Split(" ").Length == 3)
                    diff = command.Split(" ")[2];
                MineAnyBlock(int.Parse(command.Split(" ")[1]), diff);
            }
        }
    }

    static void Help()
    {
        Console.WriteLine("
    Miner commands:
    
        mine [amount]
            mines [amount] number of blocks, will only mine 1 by default if [amount] is not specified
            
        mineany [block number] [difficulty]
            hashes [block number] with the given [difficulty]. this is for debugging purposes,
            and will not actually submit the block
            
        help
            displays this dialogue");
    }

    static void Sync()
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
        for (int i = 1; i < pendingLength + 1; i++)
        {
            SyncPending(blockChainLength + i);
        }
        for (int i = 1; i < blockChainLength + 1; i++)
        {
            SyncBlock(i);
        }
        GetProgram();
    }

    static string GetLength()
    {
        try
        {
            string lengths = "";

            string html = string.Empty;
            string url = @"http://api.achillium.us.to/dcc/?query=amountOfPendingBlocks";

            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
            request.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                html = reader.ReadToEnd();
            }
            lengths += html.Trim();


            html = string.Empty;
            url = @"http://api.achillium.us.to/dcc/?query=amountOfCompletedBlocks";

            request = (HttpWebRequest)WebRequest.Create(url);
            request.AutomaticDecompression = DecompressionMethods.GZip;

            using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
            using (Stream stream = response.GetResponseStream())
            using (StreamReader reader = new StreamReader(stream))
            {
                html = reader.ReadToEnd();
            }
            lengths += "##" + html.Trim();

            return lengths;
        }
        catch (Exception e)
        {
            Console.WriteLine("Error, Try again later" + e.StackTrace);
            return "";
        }
    }

    void GetInfo()
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=getWalletInfo&fromAddress=" + wallet + "&username=" + username + "&password=" + password;

        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }
        
        string content = html.Trim();
        walletInfo = JsonConvert.DeserializeObject<WalletInfo>(content);
    }
    
    static void GetProgram()
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

        while(id == null || id == "")
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
        }

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
        Console.WriteLine("Building assigned program, wait until build window closes to start mining");
        Console.ResetColor();

        Process proc = ExecuteCommand("cargo build", "./programs/" + id + "/");
        while (!proc.HasExited) { }
    }

    static float GetProgramLifeLeft(string id)
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

    static string GetMineLevel()
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=getDifficulty";

        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }

        return html.Trim();
    }

    static void SyncPending(int whichBlock)
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

        File.WriteAllText("./pendingblocks/block" + whichBlock.ToString() + ".txt", html);
    }

    static void SyncBlock(int whichBlock)
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
        File.WriteAllText("./blockchain/block" + whichBlock.ToString() + ".txt", html);
    }

    static bool IsChainValid()
    {
        string[] blocks = Directory.GetFiles("./blockchain/", "*.txt");

        for (int i = 1; i < blocks.Length; i++)
        {
            string content = File.ReadAllText("./blockchain/block" + i + ".txt");
            Block o = JsonConvert.DeserializeObject<Block>(content);
            string[] trans = o.Transactions;

            string lastHash = o.LastHash;
            string currentHash = o.Hash;
            string nonce = o.Nonce;
            string transactions = JoinArrayPieces(trans);

            content = File.ReadAllText("./blockchain/block" + (i + 1) + ".txt");
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

    static void Mine(string lastHash, string transactionHistory, int blockNum)
    {
        DateTime startTime = DateTime.UtcNow;

        var watch = new System.Diagnostics.Stopwatch();
        watch.Start();
        Process proc = ExecuteCommand("cargo run", "./programs/" + id + "/");

        //Checks Hash
        int nonce = 0;
        while (!sha256(lastHash + transactionHistory + nonce.ToString()).StartsWith(mineLevel))
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

        string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + blockNum.ToString() + "&nonce=" + nonce.ToString() + "&minedHash=" + sha256(lastHash + transactionHistory + nonce.ToString()) + "&fromAddress=" + wallet + "&programID=" + id + "&time=" + watch.ElapsedMilliseconds / 1000f;

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

        int waitTime = 0;
        while (waitTime < 1000000000)
        {
            waitTime++;
        }
    }

    static void MineAnyBlock(int blockNum, string difficulty)
    {
        if (difficulty == null || difficulty == "")
            difficulty = mineLevel;

        DateTime startTime = DateTime.UtcNow;

        string content = File.ReadAllText("./blockchain/block" + blockNum + ".txt");
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

}
