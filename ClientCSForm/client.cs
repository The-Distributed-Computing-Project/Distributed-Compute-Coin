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

	public void Client(string usrn, string pswd, bool stayLoggedIn)
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
		

		httpServ.startHttpServ();

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
		pendingBalance = GetPendingBalance(wallet);
		costPerMinute = GetCostPerMinute();
	}
	
	public string Trade(String recipient, float sendAmount)
	{
		string html = string.Empty;
		string url = @"http://api.achillium.us.to/dcc/?query=sendToAddress&sendAmount=" + sendAmount + "&username=" + username + "&password=" + password + "&fromAddress="+wallet+"&recipientAddress=" + recipient;
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
		return float.Parse(html.Trim());
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
