
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
	static String wallet = null;
	static int pendingLength = 0;
	static int blockChainLength = 0;
	static int totalBlocks = 0;
	static string lengths = null;
	public static void Main()
	{
		Console.Write("Enter your wallet address : ");
		wallet = Console.ReadLine();
		Console.Write("Syncing with server...");
		
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
			Console.WriteLine("There are " + pendingLength.ToString() + " Blocks to compute");

			Console.Write("What do you want to do :  ");
			String command = Console.ReadLine();
			if (command.ToUpper() == "HELP")
				Help();
			if (command.ToUpper() == "SYNC")
			{
				//foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\BlockchainMiner\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly))
				//{
				//	try
				//	{
				//		File.Delete(oldBlock);
				//	}
				//	catch (Exception)
				//	{
				//	}
				//}
				//foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\BlockchainMiner\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly))
				//{
				//	try
				//	{
				//		File.Delete(oldBlock);
				//	}
				//	catch (Exception)
				//	{
				//	}
				//}
				for (int i = 1; i < pendingLength+1; i++)
				{
					SyncPending(blockChainLength + i);
				}
				for (int i = 1; i < blockChainLength+1; i++)
				{
					SyncEntireChain(i);
				}
				Console.Write(((char)7).ToString());
			}
			if (command.ToUpper() == "MINE")
			{
				//foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\BlockchainMiner\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly))
				//{
				//	try
				//	{
				//		File.Delete(oldBlock);
				//	}
				//	catch (Exception)
				//	{
				//	}
				//}
				//foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\BlockchainMiner\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly))
				//{
				//	try
				//	{
				//		File.Delete(oldBlock);
				//	}
				//	catch (Exception)
				//	{
				//	}
				//}
				for (int i = 0; i < pendingLength; i++)
				{
					SyncPending(blockChainLength + 1 + i);
				}
				for (int i = 0; i < blockChainLength; i++)
				{
					SyncEntireChain(1 + i);
				}
				Console.WriteLine((blockChainLength + 1).ToString());
				StreamReader readBlockCurrent = new StreamReader("D:\\Code\\Blockchain Main\\BlockchainMiner\\pendingblocks\\block" + (blockChainLength + 1).ToString() + ".txt");
				string lastHash = readBlockCurrent.ReadLine();
				string skipline = readBlockCurrent.ReadLine();
				skipline = readBlockCurrent.ReadLine();
				string transactions = readBlockCurrent.ReadToEnd();
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
			string url = @"http://api.achillium.us.to/dcc/?query=amountOfCompletedBlocks";

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


			Console.WriteLine(lengths);
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

			Console.WriteLine(html);
			//StreamWriter writeBlock = new StreamWriter("D:\\Code\\Blockchain Main\\BlockchainMiner\\pendingblocks\\block" + whichBlock.ToString() + ".txt");
			//writeBlock.Write(html);
			//writeBlock.Close();

			File.Create("D:\\Code\\Blockchain Main\\BlockchainMiner\\pendingblocks\\block" + whichBlock.ToString() + ".txt");
			File.WriteAllText("D:\\Code\\Blockchain Main\\BlockchainMiner\\pendingblocks\\block" + whichBlock.ToString() + ".txt", html);
		}
		catch (Exception e)
		{
			Console.WriteLine("Error, Try again later" + e.StackTrace);
		}

		int waitTime = 0;
		while (waitTime < 10000000)
		{
			waitTime++;
		}
	}

	static void SyncEntireChain(int whichBlock)
	{
		if (whichBlock == 00)
			whichBlock = 0;
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

            Console.WriteLine(html);
			//StreamWriter writeBlock = new StreamWriter("D:\\Code\\Blockchain Main\\BlockchainMiner\\blockchain\\block" + whichBlock.ToString() + ".txt");
			//writeBlock.Write(html);
			//writeBlock.Close();

			File.Create("D:\\Code\\Blockchain Main\\BlockchainMiner\\blockchain\\block" + whichBlock.ToString() + ".txt");
			File.WriteAllText("D:\\Code\\Blockchain Main\\BlockchainMiner\\blockchain\\block" + whichBlock.ToString() + ".txt", html);
		}
		catch (Exception e)
		{
			Console.WriteLine("Error, Try again later" + e.StackTrace);
		}

		int waitTime = 0;
		while (waitTime < 10000000)
		{
			waitTime++;
		}
	}

	static void Mine(string lastHash, string transactionHistory, int blockNum)
	{
		DateTime startTime = DateTime.UtcNow;

		//if (!Directory.Exists("D:\\Code\\Blockchain Main\\BlockchainMiner\\pendingblocks\\block" + blockNum.ToString() + ".txt"))
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


		try
		{
			string html = string.Empty;
			string url = "http://api.achillium.us.to/dcc/?query=submitBlock&blockNum=" + blockNum + "&nonce=" + nonce + "&minedHash=" + sha256(lastHash + transactionHistory + nonce.ToString()) + "&fromAddress=dcc1f4e25f12ae7460b9f1430a5ed1858384368b70435855570cdacdc0338b708f4";

			HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);
			request.AutomaticDecompression = DecompressionMethods.GZip;

			using (HttpWebResponse response = (HttpWebResponse)request.GetResponse())
			using (Stream stream = response.GetResponseStream())
			using (StreamReader reader = new StreamReader(stream))
			{
				html = reader.ReadToEnd();
			}

			Console.WriteLine(html + " in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
			Console.Write(((char)7).ToString());
		}
		catch (Exception e)
		{
			Console.WriteLine("Error, Try again later" + e.StackTrace);
		}

		int waitTime = 0;
		while (waitTime < 10000000)
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