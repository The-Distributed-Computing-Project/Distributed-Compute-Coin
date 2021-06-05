/*       Client Program      */

using System;
using System.IO;
using System.Net;
using System.Text;
using System.Net.Sockets;


public class clnt
{
	static String wallet = null;
	static TcpClient tcpclnt = null;
	static int blockchainlength = 0;
	static string confirmTransact = null;
	static string transactionDetails = null;
	public static void Main()
	{
		Console.Write("Enter your wallet address : ");
		wallet = Console.ReadLine();
		Console.Write("Syncing with server...");
		float balance = GetBalance(wallet);
		Console.WriteLine("You have " + balance.ToString() + " Compute Credits");
		
		StreamReader readConfig = new StreamReader("D:\\Blockchain Main\\Blockchain Client\\config.txt");
		confirmTransact = readConfig.readLine();
		transactionDetails = readConfig.readLine();
		readConfig.Close();
		
		if(confirmTransact == "true")
			WaitForConfirmation();

		while (true)
		{
			Console.Write("What do you want to do :  ");
			String command = Console.ReadLine();
			if (command.ToUpper() == "TRADE")
				Trade();
			if (command.ToUpper() == "HELP")
				Help();
			if (command.ToUpper() == "SYNC")
			{
				for (int i = 1; i < blockchainlength; i++)
				{
					Sync(i);
				}
			}


		}
	}
	
	static void WaitForConfirmation()
	{
		Console.WriteLine("Waiting for transaction confirmation...");
		try
		{
			tcpclnt = new TcpClient();
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = "$ISTRANSACTIONREADY##" + transactionDetails;
			Stream stm = tcpclnt.GetStream();

			ASCIIEncoding asen = new ASCIIEncoding();
			byte[] ba = asen.GetBytes(str);

			stm.Write(ba, 0, ba.Length);

			byte[] bb = new byte[500];
			int k = stm.Read(bb, 0, 500);
			string received = "";
			for (int recv = 0; recv < k; recv++)
			{
				received += Convert.ToChar(bb[recv]).ToString();
			}

			tcpclnt.Close();
			StreamWriter writeBlock = new StreamWriter("D:\\Blockchain Main\\Blockchain Client\\config.txt");
			writeBlock.Write(" \n ");
			writeBlock.Close();
		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
		}
	}

	static void Trade()
	{
		Console.Write("Wallet address of who you want to send to : ");
		String recipient = Console.ReadLine();
		Console.Write("Amount of Compute Credits : ");
		String sendAmount = Console.ReadLine();

		String walletValue = "0";
		try
		{
			tcpclnt = new TcpClient();
			Console.WriteLine("\nStarting trading dialogue...");
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = sendAmount + "->" + wallet + "->" + recipient;
			Stream stm = tcpclnt.GetStream();

			ASCIIEncoding asen = new ASCIIEncoding();
			byte[] ba = asen.GetBytes(str);

			stm.Write(ba, 0, ba.Length);

			byte[] bb = new byte[100];
			int k = stm.Read(bb, 0, 100);
			string received = "";
			for (int i = 0; i < k; i++)
				received += Convert.ToChar(bb[i]).ToString();

			Console.WriteLine(received);

			tcpclnt.Close();
			
			StreamWriter writeBlock = new StreamWriter("D:\\Blockchain Main\\Blockchain Client\\config.txt");
			writeBlock.Write("true\n" + sendAmount + "->" + wallet + "->" + recipient);
			writeBlock.Close();
		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
			return;
		}

		//Console.WriteLine("Your wallet has:  " + walletValue + " Compute Tokens");
	}

	static void Help()
	{
		Console.WriteLine("Possible Actions:\n");
		Console.WriteLine("trade: send tokens to anybody");
	}

	static float GetBalance(string walletAddress)
	{
		try
		{
			tcpclnt = new TcpClient();
			Console.WriteLine("\nGetting current balance...");
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = "$GETWALLLETBALANCE##" + walletAddress;
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

			blockchainlength = int.Parse(received.Trim().Split("##")[1]);
			return float.Parse(received.Trim().Split("##")[0]);
		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
			return 0;
		}
	}

	static void Sync(int whichBlock)
	{
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
			StreamWriter writeBlock = new StreamWriter("D:\\Blockchain Main\\Blockchain Client\\blockchain\\block" + whichBlock + ".txt");
			writeBlock.Write(received.Replace("##DIVIDE$LINE##", "\n"));
			writeBlock.Close();
		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
		}

		int waitTime = 0;
		while(waitTime < 10000000)
		{
			waitTime++;
		}
	}

	static int GetBlockChainLength()
	{
		try
		{
			tcpclnt = new TcpClient();
			Console.WriteLine("\nGetting blockchain length...");
			tcpclnt.Connect("192.168.56.1", 8001);

			String str = "$GETBLOCKCHAINLENGTH##";
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
			return int.Parse(received.Trim());

		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
			return 0;
		}
	}

}
