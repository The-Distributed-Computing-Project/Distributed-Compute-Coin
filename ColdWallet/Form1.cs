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
using System.Security.Cryptography;


namespace ColdWallet
{
    public partial class Form1 : Form
    {
        clnt clnt = new clnt();
        string username;
        string password;
        string selectedFile;

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool AllocConsole();

        public Form1()
        {
            InitializeComponent();
            AllocConsole();
            clnt.httpServ = new http();
            sendToWallet.Enabled = false;
            sendToAmount.Enabled = false;
            sendButton.Enabled = false;

            if (!File.Exists("./config.cfg"))
            {
                StreamWriter configFile = new StreamWriter("./config.cfg");
                configFile.Write("");
                configFile.Close();
            }
            if (!File.Exists("./cold-wallet.dccwallet"))
            {
                StreamWriter configFile = new StreamWriter("./cold-wallet.dccwallet");
                configFile.Write("");
                configFile.Close();
            }

            clnt.Client();
            UpdateUI();
            sendToWallet.Enabled = true;
            sendToAmount.Enabled = true;
            sendButton.Enabled = true;
            tradeCover.Enabled = false;
            tradeCover.Visible = false;
        }

        private void button1_Click(object sender, EventArgs e)
        {
        }

        private void label1_Click(object sender, EventArgs e)
        {
        }

        private void sendButton_Click(object sender, EventArgs e)
        {
            if (sendToWallet.TextLength > 0 && (float)sendToAmount.Value > 0)
            {
                if ((float)sendToAmount.Value <= clnt.walletInfo.Balance)
                {
                    string status = clnt.Trade(sendToWallet.Text, (float)sendToAmount.Value);
                    UpdateUI();
                    if (status.Contains("success"))
                    {
                        fieldsNotFilledWarn.ForeColor = Color.Green;
                        fieldsNotFilledWarn.Text = "Successfully sent";
                    }
                }
                else
                {
                    fieldsNotFilledWarn.ForeColor = Color.Red;
                    fieldsNotFilledWarn.Text = "You don't have that much";
                }
            }
            else
            {
                fieldsNotFilledWarn.ForeColor = Color.Red;
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

        void UpdateUI()
        {
            clnt.Client();
            walletAddr.Text = clnt.walletInfo.Address;
            computeCoins.Text = "Balance: $" + clnt.walletInfo.Balance;
            pendingFunds.Text = "Pending: $" + clnt.walletInfo.PendingBalance;
            QRCodeWallet.Image = clnt.qrCodeAsBitmap;
        }


        private void refreshButton_Click(object sender, EventArgs e)
        {
            UpdateUI();
        }
    }
}
