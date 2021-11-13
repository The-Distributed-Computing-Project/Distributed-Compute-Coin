
/*       Client Program      */

using System;
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Globalization;

public class clnt
{
    static string wallet = null;
    static string username = null;
    static string password = null;
    static int pendingLength = 0;
    static int blockChainLength = 0;
    static int totalBlocks = 0;
    static string lengths = null;
    public static void Main()
    {
        string configFileRead = File.ReadAllText("./config.cfg");
        if (configFileRead.Length > 4)
        {
            username = configFileRead.Split('\n')[0].Trim();
            password = configFileRead.Split('\n')[1].Trim();
        }
        else
        {
            Console.Write("Enter your username : ");
            username = Console.ReadLine();
            Console.Write("Enter your password : ");
            password = Console.ReadLine();

            Console.Write("Stay logged in? Y/N : ");
            string stayLoggedIn = Console.ReadLine();
            if (stayLoggedIn.ToUpper() == "Y")
            {
                StreamWriter configFile = new StreamWriter("./config.cfg");
                configFile.Write(username + "\n" + password);
                configFile.Close();
            }
        }

        wallet = "dcc" + sha256(username + password);

        for (int i = 1; i < pendingLength + 1; i++)
        {
            SyncPending(blockChainLength + i);
        }
        for (int i = 1; i < blockChainLength + 1; i++)
        {
            SyncEntireChain(i);
        }

        while (true)
        {
            lengths = GetPendingLength();
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
            Console.ResetColor();

            Console.Write("What do you want to do :  ");
            String command = Console.ReadLine();
            if (command.ToUpper() == "HELP")
                Help();
            if (command.ToUpper() == "SYNC")
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
                    SyncEntireChain(i);
                }
                Console.Write(((char)7).ToString());
            }
            if (command.ToUpper() == "MINE")
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
                for (int i = 0; i < pendingLength; i++)
                {
                    SyncPending(blockChainLength + 1 + i);
                }
                for (int i = 0; i < blockChainLength; i++)
                {
                    SyncEntireChain(1 + i);
                }
                Console.WriteLine((blockChainLength + 1).ToString());
                StreamReader readBlockCurrent = new StreamReader("./pendingblocks/block" + (blockChainLength + 1).ToString() + ".txt");
                string lastHash = readBlockCurrent.ReadLine().Replace("\n", "");
                string skipline = readBlockCurrent.ReadLine();
                skipline = readBlockCurrent.ReadLine();
                string transactions = readBlockCurrent.ReadToEnd().Replace("\n", "");
                readBlockCurrent.Close();
                Mine(lastHash, transactions, (blockChainLength + 1));
            }
        }
    }

    static void Help()
    {
        Console.WriteLine("Possible Actions:\n");
        Console.WriteLine("mine: start mining");
    }

    static string GetPendingLength()
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


            //Console.WriteLine(lengths);
            return lengths;
        }
        catch (Exception e)
        {
            Console.WriteLine("Error, Try again later" + e.StackTrace);
            return "";
        }
    }

    static void SyncPending(int whichBlock)
    {
        if (whichBlock == 00)
            whichBlock = 0;
        //try
        //{
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

        Console.WriteLine(html);
        //StreamWriter writeBlock = new StreamWriter("./pendingblocks/block" + whichBlock.ToString() + ".txt");
        //writeBlock.Write(html);
        //writeBlock.Close();

        //File.Create("./pendingblocks/block" + whichBlock.ToString() + ".txt");
        File.WriteAllText("./pendingblocks/block" + whichBlock.ToString() + ".txt", html);
        //}
        //catch (Exception e)
        //{
        //	Console.WriteLine("Error, Try again later" + e.StackTrace);
        //}

        //int waitTime = 0;
        //while (waitTime < 10000000)
        //{
        //	waitTime++;
        //}
    }

    static void SyncEntireChain(int whichBlock)
    {
        if (whichBlock == 00)
            whichBlock = 0;
        //try
        //{
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

        Console.WriteLine(html);
        //StreamWriter writeBlock = new StreamWriter("./blockchain/block" + whichBlock.ToString() + ".txt");
        //writeBlock.Write(html);
        //writeBlock.Close();

        //File.Create("./blockchain/block" + whichBlock.ToString() + ".txt");
        File.WriteAllText("./blockchain/block" + whichBlock.ToString() + ".txt", html);
        //}
        //catch (Exception e)
        //{
        //	Console.WriteLine("Error, Try again later" + e.StackTrace);
        //}

        //int waitTime = 0;
        //while (waitTime < 10000000)
        //{
        //	waitTime++;
        //}
    }

    static void Mine(string lastHash, string transactionHistory, int blockNum)
    {
        DateTime startTime = DateTime.UtcNow;

        //if (!Directory.Exists("./pendingblocks/block" + blockNum.ToString() + ".txt"))
        //{
        //	Console.WriteLine("Unable to mine, try resyncing");
        //	return;
        //}

        //Checks Hash
        int nonce = 0;
        while (!sha256(lastHash + transactionHistory + nonce.ToString()).StartsWith("0000"))
        {
            nonce++;
            Console.WriteLine((DateTime.UtcNow - startTime).ToString().Split(".")[0] + " :	" + nonce.ToString() + " # " + sha256(lastHash + transactionHistory + nonce.ToString()));
        }


        string html = string.Empty;
        string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + blockNum.ToString() + "&nonce=" + nonce.ToString() + "&minedHash=" + sha256(lastHash + transactionHistory + nonce.ToString()) + "&fromAddress=" + wallet;

        //Console.WriteLine("url: " + url);

        HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
        request.AutomaticDecompression = DecompressionMethods.GZip;

        using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
        using (Stream stream = response.GetResponseStream())
        using (StreamReader reader = new StreamReader(stream))
        {
            html = reader.ReadToEnd();
        }

        Console.ForegroundColor = ConsoleColor.Blue;
        Console.WriteLine(html + " in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
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
