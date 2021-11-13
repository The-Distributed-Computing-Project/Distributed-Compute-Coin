using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;

namespace ClientCSForm
{
	public partial class Form1 : Form
	{
		clnt clnt = new clnt();
		string username;
		string password;

		[DllImport("kernel32.dll", SetLastError = true)]
		[return: MarshalAs(UnmanagedType.Bool)]
		static extern bool AllocConsole();

		public Form1()
		{
			InitializeComponent();
			AllocConsole();
			sendToWallet.Enabled = false;
			sendToAmount.Enabled = false;
			sendButton.Enabled = false;

			string configFileRead = File.ReadAllText("./config.cfg");
			if (configFileRead.Length > 4)
			{
				username = configFileRead.Split('\n')[0].Trim();
				password = configFileRead.Split('\n')[1].Trim();
				usernameBox.Text = username;
				passwordBox.Text = password;
				tradeCover.Enabled = false;
				clnt.Client(username, password, true);
				walletAddr.Text = "Wallet: " + clnt.wallet;
				computeCoins.Text = "Compute Coins: $" + clnt.balance;
				sendToWallet.Enabled = true;
				sendToAmount.Enabled = true;
				sendButton.Enabled = true;
				tradeCover.Enabled = false;
				tradeCover.Visible = false;
			}
		}

		private void button1_Click(object sender, EventArgs e)
		{
		}

		private void label1_Click(object sender, EventArgs e)
		{
		}

		private void sendButton_Click(object sender, EventArgs e)
		{
			if (fieldsNotFilledWarn.Text == "All fields must be filled" || fieldsNotFilledWarn.Text == "You don't have that much")
				fieldsNotFilledWarn.Text = "";
			if (sendToWallet.TextLength > 0 && sendToAmount.TextLength > 0)
			{
				if(float.Parse(sendToAmount.Text) <= clnt.balance)
				{
					clnt.Trade(sendToWallet.Text, sendToAmount.Text);
					computeCoins.Text = "Compute Coins: $" + clnt.balance;
				}
				else
					fieldsNotFilledWarn.Text = "You don't have that much";
			}
			else
			{
				fieldsNotFilledWarn.Text = "All fields must be filled";
			}
		}

		private void sendToWallet_TextChanged(object sender, EventArgs e)
		{
		}

		private void sendToAmount_TextChanged(object sender, EventArgs e)
		{
		}

		private void fieldsNotFilledWarn_Click(object sender, EventArgs e)
		{
		}

		private void button1_Click_1(object sender, EventArgs e) //Sign in button
		{
			if (aFRLogin.Text == "All fields must be filled" || aFRLogin.Text == "Error connecting to server")
				aFRLogin.Text = "";
			if (usernameBox.TextLength > 0)
			{
				clnt.Client(usernameBox.Text.Trim(), passwordBox.Text.Trim(), stayLoggedIn.Checked);
				if(clnt.balance != -1.0f)
				{
					walletAddr.Text = "Wallet: " + clnt.wallet;
					computeCoins.Text = "Compute Coins: $" + clnt.balance;
					sendToWallet.Enabled = true;
					sendToAmount.Enabled = true;
					sendButton.Enabled = true;
					tradeCover.Enabled = false;
					tradeCover.Visible = false;
				}
				else
					aFRLogin.Text = "Error connecting to server";
			}
			else
				aFRLogin.Text = "All fields must be filled";
		}

		private void LoginWallet_TextChanged(object sender, EventArgs e)
		{

		}

        private void label6_Click(object sender, EventArgs e)
        {

        }
    }
}
