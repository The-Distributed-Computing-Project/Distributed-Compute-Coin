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

public class clnt
{
	//public String wallet = null;
	public TcpClient tcpclnt = null;
	public int blockchainlength = 0;
	public string confirmTransact = null;
	public string transactionDetails = null;
	public float balance;
	public void Client(string wlt)
	{
		//Console.Write("Enter your wallet address : ");
		//wallet = Console.ReadLine();
		//Console.Write("Syncing with server...");
		balance = GetBalance(wlt);
		//Console.WriteLine("You have " + balance.ToString() + " Compute Credits");
		
		StreamReader readConfig = new StreamReader("D:\\Code\\DC-Cryptocurrency\\ClientCSForm\\config.txt");
		confirmTransact = readConfig.ReadLine();
		transactionDetails = readConfig.ReadLine();
		readConfig.Close();
		
		if(confirmTransact == "true")
			WaitForConfirmation();

		//while (true)
		//{
		//	Console.Write("What do you want to do :  ");
		//	String command = Console.ReadLine();
		//	if (command.ToUpper() == "TRADE")
		//		Trade();
		//	if (command.ToUpper() == "HELP")
		//		Help();
		//	if (command.ToUpper() == "SYNC")
		//	{
		//		for (int i = 1; i < blockchainlength; i++)
		//		{
		//			Sync(i);
		//		}
		//	}


		//}
	}
	
	public void WaitForConfirmation()
	{
		Console.WriteLine("Waiting for transaction confirmation...");
		try
		{
			tcpclnt = new TcpClient();
			tcpclnt.Connect("192.168.0.15", 8001);

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
			StreamWriter writeBlock = new StreamWriter("D:\\Code\\DC-Cryptocurrency\\ClientCSForm\\config.txt");
			writeBlock.Write(" \n ");
			writeBlock.Close();
		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
		}
	}

	public void Trade(String recipient, String sendAmount, String wallet)
	{
		try
		{
			tcpclnt = new TcpClient();
			Console.WriteLine("\nStarting trading dialogue...");
			tcpclnt.Connect("192.168.0.1", 8001);

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
			
			StreamWriter writeBlock = new StreamWriter("D:\\Code\\DC-Cryptocurrency\\ClientCSForm\\config.txt");
			writeBlock.Write("true\n" + sendAmount + "->" + wallet + "->" + recipient);
			writeBlock.Close();

			balance -= float.Parse(sendAmount);
		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
			return;
		}

		//Console.WriteLine("Your wallet has:  " + walletValue + " Compute Tokens");
	}

	public void Help()
	{
		Console.WriteLine("Possible Actions:\n");
		Console.WriteLine("trade: send tokens to anybody");
	}

	public float GetBalance(string walletAddress)
	{
		string[] separator = { "##" };
		try
		{
			tcpclnt = new TcpClient();
			Console.WriteLine("\nGetting current balance...");
			tcpclnt.Connect("192.168.0.15", 8001);

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

			blockchainlength = int.Parse(received.Trim().Split(separator, System.StringSplitOptions.RemoveEmptyEntries)[1]);
			return float.Parse(received.Trim().Split(separator, System.StringSplitOptions.RemoveEmptyEntries)[0]);
		}
		catch (Exception e)
		{
			tcpclnt.Close();
			Console.WriteLine("Error, Try again later" + e.StackTrace);
			return -1.0f;
		}
	}

	public void Sync(int whichBlock)
	{
		try
		{
			tcpclnt = new TcpClient();
			tcpclnt.Connect("192.168.0.15", 8001);

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
			StreamWriter writeBlock = new StreamWriter("D:\\Code\\DC-Cryptocurrency\\ClientCSForm\\blockchain\\block" + whichBlock + ".txt");
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

	public int GetBlockChainLength()
	{
		try
		{
			tcpclnt = new TcpClient();
			Console.WriteLine("\nGetting blockchain length...");
			tcpclnt.Connect("192.168.0.15", 8001);

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
