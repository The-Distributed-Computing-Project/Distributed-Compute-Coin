/*       Client Program      */

using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using Newtonsoft.Json;
using QRCoder;

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
    public float Balance { get; set; }
    public float PendingBalance { get; set; }
    public int BlockchainLength { get; set; }
    public int PendingLength { get; set; }
    public string MineDifficulty { get; set; }
    public float CostPerMinute { get; set; }
}

public class DCCPayload : QRCoder.PayloadGenerator.Payload
{
    private string _addr;
    public DCCPayload(string addr)
    {
        _addr = addr;
    }

    public override string ToString()
    {
        return $"dcc://{_addr}";
    }
}

public class clnt
{
    public int blockchainlength = 0;
    public string username;
    public string password;
    public http httpServ;
    static string lengths = null;
    public static List<List<string>> programsData = new List<List<string>>();

    public WalletInfo walletInfo = new WalletInfo();
    //static string appURL = "dcc:" + walletInfo.Address;
    public static Bitmap qrCodeAsBitmap;

    public void Client(string usrn, string pswd, bool stayLoggedIn)
    {
        //Process proc = new Process();
        //proc.StartInfo.FileName = "netsh";
        //proc.StartInfo.Arguments = "http add urlacl url = http://192.168.0.21:9090/ user=Everyone";
        //Console.WriteLine(proc.StartInfo.Arguments);
        //proc.StartInfo.CreateNoWindow = true;
        //proc.StartInfo.UseShellExecute = false;
        //proc.StartInfo.RedirectStandardOutput = true;
        //proc.StartInfo.RedirectStandardError = true;
        //proc.Start();


        //httpServ.startHttpServ();

        username = usrn;
        password = pswd;
        string configFileRead = File.ReadAllText("./config.cfg");
        if (configFileRead.Length > 4)
        {
            username = configFileRead.Split('\n')[0].Trim();
            password = configFileRead.Split('\n')[1].Trim();
        }
        else
        {
            if (stayLoggedIn)
            {
                StreamWriter configFile = new StreamWriter("./config.cfg");
                configFile.Write(username + "\n" + password);
                configFile.Close();
            }
        }

        walletInfo.Address = "dcc" + sha256(username + password);
        walletInfo = GetInfo();

        DCCPayload generator = new DCCPayload(walletInfo.Address);
        string payload = generator.ToString();
        QRCodeGenerator qRCodeGenerator = new QRCodeGenerator();
        QRCodeData qRCodeData = qRCodeGenerator.CreateQrCode(payload, QRCodeGenerator.ECCLevel.M);
        QRCode qRCode = new QRCode(qRCodeData);
        qrCodeAsBitmap = qRCode.GetGraphic(20);

        while (Directory.GetFiles("./wwwdata/blockchain/", "*.*", SearchOption.TopDirectoryOnly).Length < walletInfo.BlockchainLength)
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
            for (int i = 0; i < walletInfo.BlockchainLength; i++)
            {
                SyncBlock(1 + i);
            }
        }

        walletInfo.Balance = GetBalance(walletInfo.Address);
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
            string content = File.ReadAllText("./wwwdata/blockchain/block" + i + ".txt");
            Block o = JsonConvert.DeserializeObject<Block>(content);
            string[] trans = o.Transactions;

            string lastHash = o.LastHash;
            string currentHash = o.Hash;
            string nonce = o.Nonce;
            string transactions = JoinArrayPieces(trans);

            content = File.ReadAllText("./wwwdata/blockchain/block" + (i + 1) + ".txt");
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

    public string Trade(String recipient, float sendAmount)
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=sendToAddress&sendAmount=" + sendAmount + "&username=" + username + "&password=" + password + "&fromAddress=" + walletInfo.Address + "&recipientAddress=" + recipient;
        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }
        walletInfo.Balance = GetBalance(walletInfo.Address);
        Console.WriteLine(html);
        return html.Trim();
    }

    public string UploadProgram(string fileName, float minutes, int computationLevel)
    {
        string baseName = fileName.Split('\\')[fileName.Split('\\').Length - 1];

        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=uploadProgram&fileName=" + baseName + "&username=" + username + "&password=" + password + "&fromAddress=" + walletInfo.Address + "&minutes=" + minutes + "&computationLevel=" + computationLevel;

        System.Net.WebClient Client = new System.Net.WebClient();
        Client.Headers.Add("Content-Type", "binary/octet-stream");
        byte[] result = Client.UploadFile(url, "POST", fileName);
        string s = System.Text.Encoding.UTF8.GetString(result, 0, result.Length);

        Console.WriteLine(s);
        return s.Trim(); ;
    }

    public float GetBalance(string walletAddress)
    {
        float bal = 0f;
        string[] blocks = Directory.GetFiles("./wwwdata/blockchain/");

        for (int i = 0; i < blocks.Length; i++)
        {
            string content = File.ReadAllText(blocks[i]);

            Block o = JsonConvert.DeserializeObject<Block>(content);
            string[] trans = o.Transactions;

            for (int l = 0; l < trans.Length; l++)
            {
                if (trans[l].Trim().Replace("->", ">").Split('>').Length >= 3)
                {
                    if (trans[l].Trim().Replace("->", ">").Split('>')[1].Split('&')[0] == walletInfo.Address && trans[l].Trim().Replace("->", ">").Split('>')[2].Split('&')[0] != walletInfo.Address)
                    {
                        bal -= float.Parse(trans[l].Trim().Replace("->", ">").Split('>')[0]);
                    }
                    else if (trans[l].Trim().Replace("->", ">").Split('>')[2].Split('&')[0] == walletInfo.Address && trans[l].Trim().Replace("->", ">").Split('>')[1].Split('&')[0] != walletInfo.Address)
                    {
                        bal += float.Parse(trans[l].Trim().Replace("->", ">").Split('>')[0]);
                    }
                }
                else if (trans[l].Trim().Replace("->", ">").Split('>')[1].Split('&')[0] == walletInfo.Address && trans[l].Trim().Replace("->", ">").Split('>').Length < 3)
                {
                    bal += float.Parse(trans[l].Trim().Replace("->", ">").Split('>')[0]);
                }
            }
        }

        return (float)Math.Truncate(bal * 10000) / 10000;
    }

    WalletInfo GetInfo()
    {
        string html = string.Empty;
        string url = @"http://api.achillium.us.to/dcc/?query=getInfo&fromAddress=" + walletInfo.Address + "&username=" + username + "&password=" + password;

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
