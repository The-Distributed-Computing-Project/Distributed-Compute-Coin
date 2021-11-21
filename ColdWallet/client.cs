/*       Client Program      */

using System;
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Security.Cryptography;


public class clnt
{
    public int blockchainlength = 0;
    public float balance;
    public float pendingBalance;
    public string username;
    public string password;
    public string wallet;
    public float costPerMinute;
    public http httpServ;
    internal static readonly char[] chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890".ToCharArray();
    static int pendingLength = 0;
    static int blockChainLength = 0;
    static int totalBlocks = 0;
    static string lengths = null;

    public void Client()
    {
        Process proc = new Process();
        proc.StartInfo.FileName = "netsh";
        proc.StartInfo.Arguments = "http add urlacl url = http://74.78.145.2:8000/ user=Everyone";
        Console.WriteLine(proc.StartInfo.Arguments);
        proc.StartInfo.CreateNoWindow = true;
        proc.StartInfo.UseShellExecute = false;
        proc.StartInfo.RedirectStandardOutput = true;
        proc.StartInfo.RedirectStandardError = true;
        proc.Start();

        string configFileRead = File.ReadAllText("./config.cfg");
        if (configFileRead.Length > 4)
        {
            username = sha256(configFileRead.Split('\n')[0].Trim());
            password = sha256(configFileRead.Split('\n')[1].Trim());
        }
        else
        {
            string keyA = GetUniqueKey(8192);
            string keyB = GetUniqueKey(8192);
            username = sha256(keyA);
            password = sha256(keyB);

            Console.WriteLine(keyA);
            Console.WriteLine(keyB);

            InitializeNewAddress();

            StreamWriter configFile = new StreamWriter("./config.cfg");
            configFile.Write(keyA + "\n" + keyB);
            configFile.Close();
        }

        wallet = "dcc" + sha256(username + password);

        lengths = GetLength();
        try
        {
            pendingLength = int.Parse(lengths.Split('#')[0]);
            blockChainLength = int.Parse(lengths.Split('#')[1]);
        }
        catch (Exception)
        {
            pendingLength = 0;
            blockChainLength = 0;
        }

        while (Directory.GetFiles("./wwwdata/blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length < blockChainLength)
        {
            SyncBlock(Directory.GetFiles("./wwwdata/blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length + 1);
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
            for (int i = 0; i < blockChainLength; i++)
            {
                SyncBlock(1 + i);
            }
        }

        balance = GetBalance(wallet);
        pendingBalance = GetPendingBalance(wallet);
        costPerMinute = GetCostPerMinute();
    }

    public static string GetUniqueKey(int size)
    {
        byte[] data = new byte[4 * size];
        using (var crypto = RandomNumberGenerator.Create())
        {
            crypto.GetBytes(data);
        }
        StringBuilder result = new StringBuilder(size);
        for (int i = 0; i < size; i++)
        {
            var rnd = BitConverter.ToUInt32(data, i * 4);
            var idx = rnd % chars.Length;

            result.Append(chars[idx]);
        }

        return result.ToString();
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
            lengths += "#" + html.Trim();

            return lengths;
        }
        catch (Exception e)
        {
            Console.WriteLine("Error, Try again later" + e.StackTrace);
            return "";
        }
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
        File.WriteAllText("./wwwdata/blockchain/block" + whichBlock.ToString() + ".txt", html);
    }

    static bool IsChainValid()
    {
        string[] blocks = Directory.GetFiles("./wwwdata/blockchain/", "*.txt");

        for (int i = 1; i < blocks.Length; i++)
        {
            StreamReader readBlock = new StreamReader("./wwwdata/blockchain/block" + i + ".txt");
            string lastHash = readBlock.ReadLine().Trim();
            string currentHash = readBlock.ReadLine().Trim();
            string nonce = readBlock.ReadLine().Trim();
            string transactions = readBlock.ReadToEnd().Replace("\n", "");
            readBlock.Close();

            string nextHash = "";

            readBlock = new StreamReader("./wwwdata/blockchain/block" + (i + 1) + ".txt");
            nextHash = readBlock.ReadLine().Trim();
            readBlock.Close();

            Console.WriteLine("Validating block " + i);
            string blockHash = sha256(lastHash + transactions + nonce);
            if (!blockHash.StartsWith("00") || blockHash != currentHash || blockHash != nextHash)
            {
                return false;
            }
        }
        return true;
    }

    public string Trade(String recipient, float sendAmount)
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=sendToAddress&sendAmount=" + sendAmount + "&username=" + username + "&password=" + password + "&fromAddress=" + wallet + "&recipientAddress=" + recipient;
        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }
        balance = GetBalance(wallet);
        Console.WriteLine(html);
        return html.Trim();
    }

    public string UploadProgram(string fileName, float minutes, int computationLevel)
    {
        string baseName = fileName.Split('\\')[fileName.Split('\\').Length - 1];

        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=uploadProgram&fileName=" + baseName + "&username=" + username + "&password=" + password + "&fromAddress=" + wallet + "&minutes=" + minutes + "&computationLevel=" + computationLevel;

        System.Net.WebClient Client = new System.Net.WebClient();
        Client.Headers.Add("Content-Type", "binary/octet-stream");
        byte[] result = Client.UploadFile(url, "POST", fileName);
        string s = System.Text.Encoding.UTF8.GetString(result, 0, result.Length);

        Console.WriteLine(s);
        return s.Trim(); ;
    }

    public void Help()
    {
        Console.WriteLine("Possible Actions:\n");
        Console.WriteLine("trade: send tokens to anybody");
    }

    public float GetBalance(string walletAddress)
    {
        float bal = 0f;
        string[] blocks = Directory.GetFiles("./wwwdata/blockchain/");

        for (int i = 0; i < blocks.Length; i++)
        {
            string[] content = File.ReadAllLines(blocks[i]);

            for (int l = 3; l < content.Length; l++)
            {
                if (content[l].Trim().Replace("->", ">").Split('>').Length >= 3)
                {
                    if (content[l].Trim().Replace("->", ">").Split('>')[1] == wallet && content[l].Trim().Replace("->", ">").Split('>')[2] != wallet)
                    {
                        bal -= float.Parse(content[l].Trim().Replace("->", ">").Split('>')[0]);
                    }
                    else if (content[l].Trim().Replace("->", ">").Split('>')[2] == wallet && content[l].Trim().Replace("->", ">").Split('>')[1] != wallet)
                    {
                        bal += float.Parse(content[l].Trim().Replace("->", ">").Split('>')[0]);
                    }
                }
                else if (content[l].Trim().Replace("->", ">").Split('>')[1] == wallet && content[l].Trim().Replace("->", ">").Split('>').Length < 3)
                {
                    bal += float.Parse(content[l].Trim().Replace("->", ">").Split('>')[0]);
                }
            }
        }

        return (float)Math.Truncate(bal * 10000) / 10000;

        //string html = string.Empty;
        //string url = @"http://api.achillium.us.to/dcc/?query=getBalance&fromAddress=" + wallet + "&username=" + username + "&password=" + password;

        //HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        //request.AutomaticDecompression = DecompressionMethods.GZip;

        //using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        //using (Stream stream = response.GetResponseStream())
        //using (StreamReader reader = new StreamReader(stream))
        //{
        //	html = reader.ReadToEnd();
        //}

        //Console.WriteLine(html);
        //return float.Parse(html.Trim());
    }

    public float GetPendingBalance(string walletAddress)
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=pendingFunds&fromAddress=" + wallet + "&username=" + username + "&password=" + password;

        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }

        Console.WriteLine(html);

        return (float)Math.Truncate(float.Parse(html.Trim()) * 10000) / 10000;
    }

    public float GetCostPerMinute()
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=getCostPerMinute";

        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }

        Console.WriteLine(html);
        return float.Parse(html.Trim());
    }

    public void InitializeNewAddress()
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=initializeNewAddress" + "&username=" + username + "&password=" + password;

        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }

        Console.WriteLine(html);
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
}
