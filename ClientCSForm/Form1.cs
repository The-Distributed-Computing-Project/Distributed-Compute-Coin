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
        string selectedFile;

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
            compSliderTooltip.SetToolTip(ComputationPowerSlider, "This effects which miners this program will be run on. Accurately describing this \nhelps lower the amount of minutes required to compute the same amount of data.");
            totalMinutesTooltip.SetToolTip(TotalMinutesBox, "The amount of minutes your program will run, uninterrupted on mining computers. \nKeep in mind, these are total minutes distributed across machines. You can buy a minimum of 1 minute.");
            zippedRustFolderTooltip.SetToolTip(SelectFileButton, "The Distributed Computing network uses programs written in Rust for ease of use. \nThese must be packaged in .ZIP files for better upload/download time and security.");
            submitFileBtnTooltip.SetToolTip(submitFileButton, "In order to keep malicious or un-wanted software from entering the network and \nthe network's computers, all programs will be screened by trusted users before getting released");

            if (!File.Exists("./config.cfg"))
            {
                StreamWriter configFile = new StreamWriter("./config.cfg");
                configFile.Write("");
                configFile.Close();
            }

            string configFileRead = File.ReadAllText("./config.cfg");
            if (configFileRead.Length > 4)
            {
                username = configFileRead.Split('\n')[0].Trim();
                password = configFileRead.Split('\n')[1].Trim();
                usernameBox.Text = username;
                passwordBox.Text = password;
                tradeCover.Enabled = false;
                clnt.Client(username, password, true);
                UpdateUI(); 
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
            if (sendToWallet.TextLength > 0 && (float)sendToAmount.Value > 0)
            {
                if ((float)sendToAmount.Value <= clnt.balance)
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

        private void logInBtn_Click_1(object sender, EventArgs e) //Sign in button
        {
            if (usernameBox.TextLength > 0)
            {
                clnt.Client(usernameBox.Text.Trim(), passwordBox.Text.Trim(), stayLoggedIn.Checked);
                if (clnt.balance != -1.0f)
                {
                    UpdateUI(); 
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

        private void SelectFileButton_Click(object sender, EventArgs e)
        {
            int size = -1;
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.Filter = "zip files (*.zip)|*.zip";
            DialogResult result = openFileDialog1.ShowDialog(); // Show the dialog.
            if (result == DialogResult.OK) // Test result.
            {
                string file = openFileDialog1.FileName;
                try
                {
                    string text = File.ReadAllText(file);
                    size = text.Length;
                    fileLocation.Text = file;
                    selectedFile = file;
                    EstimateCost();
                    Console.WriteLine(fileLocation); // <-- For debugging use.
                }
                catch (IOException)
                {
                }
            }

        }

        private void TotalMinutesBox_TextChanged(object sender, EventArgs e)
        {
            EstimateCost();
        }

        private void ComputationPowerSlider_TextChanged(object sender, EventArgs e)
        {
            EstimateCost();
        }

        private void LearnMore_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            // Navigate to a URL.
            System.Diagnostics.Process.Start("https://github.com/sam-astro/DC-Blockchain-Cryptocurrency/wiki");
        }

        private void submitFileButton_Click(object sender, EventArgs e)
        {
            string file = selectedFile;
            float minutes = (float)TotalMinutesBox.Value;
            int computationLevel = ComputationPowerSlider.Value;

            if (selectedFile == null || selectedFile == "")
            {
                uploadErrorText.ForeColor = Color.Red;
                uploadErrorText.Text = "All fields must be filled";
            }
            else
            {
                string uploadStatus = clnt.UploadProgram(file, minutes, computationLevel);

                if (uploadStatus.Contains("success"))
                {
                    uploadErrorText.ForeColor = Color.Green;
                    uploadErrorText.Text = "Successfully uploaded new program";
                }
            }

            UpdateUI();
        }

        void UpdateUI()
        {
            clnt.Client(usernameBox.Text.Trim(), passwordBox.Text.Trim(), stayLoggedIn.Checked);
            walletAddr.Text = "Wallet: " + clnt.wallet;
            computeCoins.Text = "Balance: $" + clnt.balance;
            pendingFunds.Text = "Pending: $" + clnt.pendingBalance;
            EstimateCost();
        }

        void EstimateCost()
        {
            estimatedCostNumber.Text = "$" + (clnt.costPerMinute + (ComputationPowerSlider.Value / 10f) * clnt.costPerMinute) * (float)TotalMinutesBox.Value;
        }
    }
}
