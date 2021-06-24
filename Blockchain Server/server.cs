//
/*   Server Program    */

using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Security.Cryptography;
using System.Collections.Generic;

public class serv
{
	public static void Main()
	{
		Socket s = null;
		TcpListener myList = null;

		int amountOfBlocks = Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly).Length;
		while (true)
		{
			try
			{
				#region connects to client and get collected data
				if(s != null)
				{
					s.Close();
					myList.Stop();
				}
				IPAddress ipAd = IPAddress.Parse("192.168.56.1");
				// use local m/c IP address, and 
				// use the same in the client

				/* Initializes the Listener */
				myList = new TcpListener(ipAd, 8001);
				/* Start Listeneting at the specified port */
				myList.Start();
				Console.WriteLine("\nThe server is running at port 8001...");
				Console.WriteLine("The local End point is  :" +
								  myList.LocalEndpoint);

				Console.WriteLine("Waiting for a connection...");
				s = myList.AcceptSocket();

				Console.WriteLine("Connection accepted from " + s.RemoteEndPoint);

				byte[] b = new byte[1000];
				int k = s.Receive(b);
				Console.WriteLine("Recieved...");
				string received = "";

				for (int i = 0; i < k; i++)
					received += Convert.ToChar(b[i]);
				Console.Write(received);
				#endregion

				if (received.Split("##")[0] == ("$GETWALLLETBALANCE"))
				{
					GetUserBalance(received.Split("##")[1], myList, s);
					continue;
				}

				if (received.Split("##")[0] == ("$GETBLOCKCHAIN"))
				{
					GetBlockchain(int.Parse(received.Split("##")[1]), myList, s);
					continue;
				}

				if (received.Split("##")[0] == ("$GETBLOCKCHAINLENGTH"))
				{
					if (Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly).Length > 0)
					{
						GetBlockchainLength(myList, s);
					}
					continue;
				}

				if (received.Split("##")[0] == ("$GETPENDINGLENGTH"))
				{
					if (Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly).Length > 0)
					{
						GetPendingLength(myList, s);
					}
					continue;
				}

				if (received.Split("##")[0] == ("$GETPENDINGBLOCKS"))
				{
					GetPendingBlocks(int.Parse(received.Split("##")[1]), myList, s);
					continue;
				}

				if (received.Split("##")[0] == ("$SUBMITMINE"))
				{
					SubmitMine(int.Parse(received.Split("##")[1]), received.Split("##")[2], int.Parse(received.Split("##")[3]), myList, s);
					continue;
				}

				try
				{
					float tester = float.Parse(received.Split("->")[0]);
				}
				catch (Exception)
				{
					continue;
				}
				float requestedMoney = float.Parse(received.Split("->")[0]);
				string walletAddress = received.Split("->")[1];
				float usersMoney = 0;
				string[] previousHash = new string[amountOfBlocks];
				string[] currentHash = new string[amountOfBlocks];
				string[] currentNonce = new string[amountOfBlocks];
				string[] everyTransaction = new string[amountOfBlocks];

				for (int c = 0; c < amountOfBlocks; c++)
				{
					StreamReader readAll = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\block" + (c+1) + ".txt");
					previousHash[c] = readAll.ReadLine();           //Gets the previous blocks hash
					currentHash[c] = readAll.ReadLine();            //Gets the current blocks hash
					currentNonce[c] = readAll.ReadLine();   
					if (previousHash[c] == null)
						previousHash[c] = "";
					if (currentHash[c] == null)
						currentHash[c] = "";
					if (currentNonce[c] == null)
						currentNonce[c] = "";
					everyTransaction = readAll.ReadToEnd().Split("\n");
					foreach (string usersTransactions in everyTransaction)
					{
						if (usersTransactions.Contains(walletAddress))
						{
							if (usersTransactions.Split("->").Length > 2)
							{
								if (usersTransactions.Split("->")[2] == walletAddress)
								{
									usersMoney += float.Parse(usersTransactions.Split("->")[0]);
								}
								else if (usersTransactions.Split("->")[1] == walletAddress)
								{
									usersMoney -= float.Parse(usersTransactions.Split("->")[0]);
								}
							}
							else
							{
								usersMoney += float.Parse(usersTransactions.Split("->")[0]);
							}
						}
					}
					readAll.Close();

					if (c > 1)
					{
						if (currentHash[c - 1].Trim() != previousHash[c].Trim())
						{
							Console.WriteLine("\nBlockchain altered or corrupted, exiting...\n");
							return;
						}
					}
				}

				if (usersMoney < requestedMoney)
				{
					ASCIIEncoding snd = new ASCIIEncoding();
					s.Send(snd.GetBytes("\nNot enough money, " + (usersMoney - requestedMoney).ToString() + " Short\n"));
					Console.WriteLine("\nNot enough money\n");
				}

				StreamReader readCurrentBlock = null;
				StreamReader lasthashget = null;
				string lastHash = null;
				string thisHash = null;
				string thisNonce = null;
				string hashHistory = null;
				//Reads current or most recently mined block

				if(File.Exists("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\block" + (amountOfBlocks + 1) + ".txt"))
				{
					readCurrentBlock = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\block" + (amountOfBlocks + 1) + ".txt");
					lasthashget = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\block" + (amountOfBlocks) + ".txt");
					string skipline = lasthashget.ReadLine();
					lastHash = lasthashget.ReadLine();       //Gets the previous blocks hash
					skipline = readCurrentBlock.ReadLine();
					thisHash = readCurrentBlock.ReadLine();       //Gets the current blocks hash
					thisNonce = readCurrentBlock.ReadLine();
					hashHistory = readCurrentBlock.ReadToEnd();   //Gets transaction history as a single string
																  //Console.WriteLine("HASH HISTORY: " + readCurrentBlock.ReadToEnd());
					readCurrentBlock.Close();
					lasthashget.Close();
				}
				else
				{
					lasthashget = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\block" + (amountOfBlocks) + ".txt");
					string skipline = lasthashget.ReadLine();
					lastHash = lasthashget.ReadLine();       //Gets the previous blocks hash

					lasthashget.Close();
				}

				string finalHash = sha256(lastHash + hashHistory + received);

				ASCIIEncoding sendbak = new ASCIIEncoding();
				s.Send(sendbak.GetBytes("\nCreated new as:" + finalHash + "\n"));
				Console.WriteLine("\nCreated new as:" + finalHash + "\n");
				StreamWriter sw = new StreamWriter("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\block" + (amountOfBlocks+1) + ".txt");
				if (hashHistory != null)
				{
					sw.Write(lastHash + "\n" + finalHash + "\n\n" + hashHistory + "\n" + received.Trim().Replace("\n", ""));
				}
				else
				{
					sw.Write(lastHash + "\n" + finalHash + "\n\n" + received.Trim().Replace("\n", ""));
				}
				sw.Close();

				ASCIIEncoding asen = new ASCIIEncoding();
				s.Send(asen.GetBytes("The server recieved: > " + received));

			}
			catch (Exception e)
			{
				Console.WriteLine("\nError, Try again later" + e.StackTrace);
			}
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

	static void GetUserBalance(string walletAddr, TcpListener myList, Socket s)
	{

		int amountOfBlocks = Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly).Length;
		float usersMoney = 0;
		string[] previousHash = new string[amountOfBlocks];
		string[] currentHash = new string[amountOfBlocks];
		string[] currentNonce = new string[amountOfBlocks];
		string[] everyTransaction = new string[amountOfBlocks];
		for (int c = 0; c < amountOfBlocks; c++)
		{
			StreamReader readAll = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\block" + (c + 1) + ".txt");
			previousHash[c] = readAll.ReadLine();           //Gets the previous blocks hash
			currentHash[c] = readAll.ReadLine();            //Gets the current blocks hash
			currentNonce[c] = readAll.ReadLine();            //Gets the current blocks hash
			if (previousHash[c] == null)
				previousHash[c] = "";
			if (currentHash[c] == null)
				currentHash[c] = "";
			if (currentNonce[c] == null)
				currentNonce[c] = "";
			everyTransaction = readAll.ReadToEnd().Split("\n");
			foreach (string usersTransactions in everyTransaction)
			{
				if (usersTransactions.Contains(walletAddr))
				{
					if (usersTransactions.Split("->").Length > 2)
					{
						if (usersTransactions.Split("->")[2] == walletAddr)
						{
							usersMoney += float.Parse(usersTransactions.Split("->")[0]);
						}
						else if (usersTransactions.Split("->")[1] == walletAddr)
						{
							usersMoney -= float.Parse(usersTransactions.Split("->")[0]);
						}
					}
					else
					{
						usersMoney += float.Parse(usersTransactions.Split("->")[0]);
					}
				}
			}
			readAll.Close();

		}
		ASCIIEncoding sendbak = new ASCIIEncoding();
		s.Send(sendbak.GetBytes(usersMoney.ToString() + "##" + amountOfBlocks.ToString()));
	}

	static void GetBlockchain(int whichBlock, TcpListener myList, Socket s)
	{
		string[] everyTransaction = new string[4000];
		StreamReader readAll = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\block" + whichBlock + ".txt");

		everyTransaction[0] = readAll.ReadToEnd();
		ASCIIEncoding sendbak = new ASCIIEncoding();
		s.Send(sendbak.GetBytes(everyTransaction[0]));
		readAll.Close();
	}

	static void GetPendingBlocks(int whichBlock, TcpListener myList, Socket s)
	{
		string[] everyTransaction = new string[4000];
		StreamReader readAll = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\block" + whichBlock + ".txt");

		everyTransaction[0] = readAll.ReadToEnd();
		ASCIIEncoding sendbak = new ASCIIEncoding();
		s.Send(sendbak.GetBytes(everyTransaction[0]));
		readAll.Close();
	}

	static void GetBlockchainLength(TcpListener myList, Socket s)
	{
		int chainLength = Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly).Length;
		ASCIIEncoding sendbak = new ASCIIEncoding();
		s.Send(sendbak.GetBytes(chainLength.ToString()));
	}

	static void GetPendingLength(TcpListener myList, Socket s)
	{
		int chainLength = Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\", "*.*", SearchOption.TopDirectoryOnly).Length;
		int pendingLength = Directory.GetFiles("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\", "*.*", SearchOption.TopDirectoryOnly).Length;
		ASCIIEncoding sendbak = new ASCIIEncoding();
		s.Send(sendbak.GetBytes(pendingLength.ToString() + "##" + chainLength.ToString()));
	}

	static void SubmitMine(int noncey, string rewardee, int blockNum, TcpListener myList, Socket s)
	{
		try
		{
			//Reads current block
			StreamReader readCurrentBlock = new StreamReader("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\block" + (blockNum) + ".txt");
			string lastHash = readCurrentBlock.ReadLine();       //Gets the previous blocks hash
			string thisHash = readCurrentBlock.ReadLine();       //Gets the current blocks hash
			string skipline = readCurrentBlock.ReadLine();       //Gets the current blocks hash
			if (thisHash == null)
			{
				thisHash = "";
			}
			string hashHistory = readCurrentBlock.ReadToEnd();   //Gets transaction history as a single string
			if (hashHistory == null)
			{
				hashHistory = "";
			}
			readCurrentBlock.Close();

			//Checks Hash
			if (!sha256(lastHash + hashHistory + noncey.ToString()).StartsWith("0000"))
			{
				Console.WriteLine("\nIncorrectly mined block: " + blockNum);
				string finalHash = sha256(lastHash + hashHistory + noncey.ToString());

				ASCIIEncoding sendbak = new ASCIIEncoding();
				s.Send(sendbak.GetBytes("\nIncorrectly mined block: " + blockNum));
			}
			else
			{
				string finalHash = sha256(lastHash + hashHistory + noncey.ToString());

				Console.WriteLine("\nMined new block: " + finalHash + " : " + noncey);
				ASCIIEncoding sendbak = new ASCIIEncoding();
				s.Send(sendbak.GetBytes("\nMined new block: " + finalHash + " : " + noncey));
				StreamWriter sw = new StreamWriter("D:\\Code\\Blockchain Main\\Blockchain Server\\blockchain\\block" + (blockNum) + ".txt");
				File.Delete("D:\\Code\\Blockchain Main\\Blockchain Server\\pendingblocks\\block" + (blockNum) + ".txt");
				sw.Write(lastHash + "\n" + finalHash + "\n" + noncey + "\n" + hashHistory);
				sw.WriteLine(2 + "->" + rewardee);
				sw.Close();
			}
		}
		catch (Exception)
		{
			Console.WriteLine("\nBlock has already been mined: " + blockNum);
			ASCIIEncoding sendbak = new ASCIIEncoding();
			s.Send(sendbak.GetBytes("\nBlock has already been mined: " + blockNum));
			return;
		}
	}
}
