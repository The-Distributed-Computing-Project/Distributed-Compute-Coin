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
	public int blockchainlength = 0;
	public float balance;
	public string username;
	public string password;
	public string wallet;
	public void Client(string usrn, string pswd, bool stayLoggedIn)
	{
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

		wallet = "dcc" + sha256(username + password);

		balance = GetBalance(wallet);
	}
	
	public void Trade(String recipient, String sendAmount)
	{
		string html = string.Empty;
		string url = @"http://api.achillium.us.to/dcc/?query=sendToAddress&sendAmount=" + sendAmount + "&username=" + username + "&password=" + password + "&fromAddress="+wallet+"&recipientAddress=" + recipient;
		Console.WriteLine(url);
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
	}

	public void Help()
	{
		Console.WriteLine("Possible Actions:\n");
		Console.WriteLine("trade: send tokens to anybody");
	}

	public float GetBalance(string walletAddress)
	{
		string html = string.Empty;
		string url = @"http://api.achillium.us.to/dcc/?query=getBalance&fromAddress=" + wallet + "&username=" + username + "&password=" + password;

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

	//public void Sync(int whichBlock)
	//{
	//	try
	//	{
	//		tcpclnt = new TcpClient();
	//		tcpclnt.Connect("192.168.0.15", 8001);

	//		String str = "$GETBLOCKCHAIN##" + whichBlock;
	//		Stream stm = tcpclnt.GetStream();

	//		ASCIIEncoding asen = new ASCIIEncoding();
	//		byte[] ba = asen.GetBytes(str);

	//		stm.Write(ba, 0, ba.Length);

	//		byte[] bb = new byte[500];
	//		int k = stm.Read(bb, 0, 500);
	//		string received = "";
	//		for (int recv = 0; recv < k; recv++)
	//		{
	//			try
	//			{
	//				received += Convert.ToChar(bb[recv]).ToString();
	//			}
	//			catch (Exception)
	//			{
	//				Console.WriteLine("==EndOfBlock==");
	//			}
	//		}

	//		tcpclnt.Close();
	//		Console.WriteLine(received.Replace("##DIVIDE$LINE##", "\n"));
	//		StreamWriter writeBlock = new StreamWriter("./blockchain\\block" + whichBlock + ".txt");
	//		writeBlock.Write(received.Replace("##DIVIDE$LINE##", "\n"));
	//		writeBlock.Close();
	//	}
	//	catch (Exception e)
	//	{
	//		tcpclnt.Close();
	//		Console.WriteLine("Error, Try again later" + e.StackTrace);
	//	}

	//	int waitTime = 0;
	//	while(waitTime < 10000000)
	//	{
	//		waitTime++;
	//	}
	//}

	//public int GetBlockChainLength()
	//{
	//	try
	//	{
	//		tcpclnt = new TcpClient();
	//		Console.WriteLine("\nGetting blockchain length...");
	//		tcpclnt.Connect("192.168.0.15", 8001);

	//		String str = "$GETBLOCKCHAINLENGTH##";
	//		Stream stm = tcpclnt.GetStream();

	//		ASCIIEncoding asen = new ASCIIEncoding();
	//		byte[] ba = asen.GetBytes(str);

	//		stm.Write(ba, 0, ba.Length);

	//		byte[] bb = new byte[100];
	//		int k = stm.Read(bb, 0, 100);
	//		string received = "";
	//		for (int i = 0; i < k; i++)
	//			received += Convert.ToChar(bb[i]).ToString();

	//		tcpclnt.Close();
	//		return int.Parse(received.Trim());

	//	}
	//	catch (Exception e)
	//	{
	//		tcpclnt.Close();
	//		Console.WriteLine("Error, Try again later" + e.StackTrace);
	//		return 0;
	//	}
	//}

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
