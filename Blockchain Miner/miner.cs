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
	static TcpClient tcpclnt = null;
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
				foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly))
				{
					try
					{
						File.Delete(oldBlock);
					}
					catch (Exception)
					{
					}
				}
				foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Miner\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly))
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
				Console.Write(((char)7).ToString());
			}
			if (command.ToUpper() == "MINE")
			{
				foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly))
				{
					try
					{
						File.Delete(oldBlock);
					}
					catch (Exception)
					{
					}
				}
				foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Miner\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly))
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
				StreamReader readBlockCurrent = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\block" + (blockChainLength + 1).ToString() + ".txt");
				string lastHash = readBlockCurrent.ReadLine();
				string skipline = readBlockCurrent.ReadLine();
				skipline = readBlockCurrent.ReadLine();
				string transactions = readBlockCurrent.ReadToEnd();
				readBlockCurrent.Close();
				Mine(lastHash, transactions, (blockChainLength + 1));
			}
			if (command.ToUpper() == "MINE ALL")
			{
				while (Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly).Length > 0)
				{
					foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly))
					{
						try
						{
							File.Delete(oldBlock);
						}
						catch (Exception)
						{
						}
					}
					foreach (string oldBlock in Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Miner\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly))
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
					StreamReader readBlock = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\block" + (blockChainLength + 1).ToString() + ".txt");
					string lastHash = readBlock.ReadLine();
					string skipline = readBlock.ReadLine();
					skipline = readBlock.ReadLine();
					string transactions = readBlock.ReadToEnd();
					Mine(lastHash, transactions, (blockChainLength + 1));
				}
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
			tcpclnt = new TcpClient();
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = "$GETPENDINGLENGTH##";
			Stream stm = tcpclnt.GetStream();

			ASCIIEncoding asen = new ASCIIEncoding();
			byte[] ba = asen.GetBytes(str);

			stm.Write(ba, 0, ba.Length);

			byte[] bb = new byte[100];
			int k = stm.Read(bb, 0, 100);
			string received = "";
			for (int i = 0; i < k; i++)
				received += Convert.ToChar(bb[i]).ToString();

			tcpclnt.Close();
			return received;
		}
		catch (Exception e)
		{
			tcpclnt.Close();
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
			tcpclnt = new TcpClient();
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = "$GETPENDINGBLOCKS##" + whichBlock;
			Stream stm = tcpclnt.GetStream();

			ASCIIEncoding asen = new ASCIIEncoding();
			byte[] ba = asen.GetBytes(str);

			stm.Write(ba, 0, ba.Length);

			byte[] bb = new byte[500];
			int k = stm.Read(bb, 0, 500);
			string received = "";
			for (int recv = 0; recv < k; recv++)
			{
				try
				{
					received += Convert.ToChar(bb[recv]).ToString();
				}
				catch (Exception)
				{
					Console.WriteLine("==EndOfBlock==");
				}
			}

			tcpclnt.Close();
			Console.WriteLine(received.Replace("##DIVIDE$LINE##", "\n"));
			StreamWriter writeBlock = new StreamWriter("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\block" + whichBlock.ToString() + ".txt");
			writeBlock.Write(received.Replace("##DIVIDE$LINE##", "\n"));
			writeBlock.Close();
		}
		catch (Exception e)
		{
			tcpclnt.Close();
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
			tcpclnt = new TcpClient();
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = "$GETBLOCKCHAIN##" + whichBlock;
			Stream stm = tcpclnt.GetStream();

			ASCIIEncoding asen = new ASCIIEncoding();
			byte[] ba = asen.GetBytes(str);

			stm.Write(ba, 0, ba.Length);

			byte[] bb = new byte[500];
			int k = stm.Read(bb, 0, 500);
			string received = "";
			for (int recv = 0; recv < k; recv++)
			{
				try
				{
					received += Convert.ToChar(bb[recv]).ToString();
				}
				catch (Exception)
				{
					Console.WriteLine("==EndOfBlock==");
				}
			}

			tcpclnt.Close();
			Console.WriteLine(received.Replace("##DIVIDE$LINE##", "\n"));
			StreamWriter writeBlock = new StreamWriter("D:\\Code\\Blockchain Main\\Blockchain Miner\\blockchain\\block" + whichBlock.ToString() + ".txt");
			writeBlock.Write(received.Replace("##DIVIDE$LINE##", "\n"));
			writeBlock.Close();
		}
		catch (Exception e)
		{
			tcpclnt.Close();
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

		//if (!Directory.Exists("D:\\Code\\Blockchain Main\\Blockchain Miner\\pendingblocks\\block" + blockNum.ToString() + ".txt"))
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
			tcpclnt = new TcpClient();
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = "$SUBMITMINE##" + nonce + "##" + wallet + "##" + blockNum;
			Stream stm = tcpclnt.GetStream();

			ASCIIEncoding asen = new ASCIIEncoding();
			byte[] ba = asen.GetBytes(str);

			stm.Write(ba, 0, ba.Length);

			byte[] bb = new byte[500];
			int k = stm.Read(bb, 0, 500);
			string received = "";
			for (int recv = 0; recv < k; recv++)
			{
				try
				{
					received += Convert.ToChar(bb[recv]).ToString();
				}
				catch (Exception)
				{

				}
			}

			tcpclnt.Close();
			Console.WriteLine(received.Replace("##DIVIDE$LINE##", "\n") + " in " + (DateTime.UtcNow - startTime).ToString().Split(".")[0]);
			Console.Write(((char)7).ToString());
		}
		catch (Exception e)
		{
			tcpclnt.Close();
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
