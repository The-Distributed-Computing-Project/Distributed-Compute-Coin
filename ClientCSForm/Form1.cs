using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ClientCSForm
{
	public partial class Form1 : Form
	{
		clnt clnt = new clnt();
		string wlt = "";
		public Form1()
		{
			InitializeComponent();
			sendToWallet.Enabled = false;
			sendToAmount.Enabled = false;
			sendButton.Enabled = false;
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
					clnt.Trade(sendToWallet.Text, sendToAmount.Text, wlt);
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
			if (LoginWallet.TextLength > 0)
			{
				clnt.Client(LoginWallet.Text.Trim());
				if(clnt.balance != -1.0f)
				{
					walletAddr.Text = "Wallet: " + LoginWallet.Text;
					wlt = LoginWallet.Text;
					computeCoins.Text = "Compute Coins: $" + clnt.balance;
					LoginWallet.Text = "";
					sendToWallet.Enabled = true;
					sendToAmount.Enabled = true;
					sendButton.Enabled = true;
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
	}
}
