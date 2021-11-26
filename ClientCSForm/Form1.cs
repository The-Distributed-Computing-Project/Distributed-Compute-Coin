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

        private List<Control> programSyncButtons = new List<Control>();

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
            compSliderTooltip.SetToolTip(ComputationPowerSlider, "This effects which miners this program will be run on. Accurately describing this \nhelps lower the amount of minutes required to compute the same amount of data.");
            totalMinutesTooltip.SetToolTip(TotalMinutesBox, "The amount of minutes your program will run, uninterrupted on mining computers. \nKeep in mind, these are total minutes distributed across machines. You can buy a minimum of 1 minute.");
            zippedRustFolderTooltip.SetToolTip(SelectFileButton, "The Distributed Computing network uses programs written in Rust for ease of use. \nThese must be packaged in .ZIP files for better upload/download time and security. \nEnsure this file is less than 20 MB, and you have done \"cargo clean\" in the project folder to remove uneeded files.");
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
                if ((float)sendToAmount.Value <= clnt.walletInfo.Balance)
                {
                    UpdateUI();
                    if (clnt.connectionStatus == 1)
                    {
                        string status = clnt.Trade(sendToWallet.Text, (float)sendToAmount.Value);
                        UpdateUI();
                        if (status.Contains("success"))
                        {
                            fieldsNotFilledWarn.ForeColor = Color.Green;
                            fieldsNotFilledWarn.Text = "Successfully sent";
                        }
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
                if (clnt.connectionStatus == 1)
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

                    long fileSize = GetFileSize(file) / (1024 * 1024);

                    if (fileSize > 20)
                    {
                        fileLocation.ForeColor = Color.Red;
                        fileLocation.Text = "File is larger than 20MB. Try doing \"cargo clean\"";
                    }
                    else
                    {
                        fileLocation.ForeColor = Color.Gray;
                        fileLocation.Text = file;
                        selectedFile = file;
                    }

                    EstimateCost();
                    Console.WriteLine(fileLocation);
                }
                catch (IOException)
                {
                }
            }

        }

        static long GetFileSize(string FilePath)
        {
            if (File.Exists(FilePath))
            {
                return new FileInfo(FilePath).Length;
            }
            return 0;
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

            UpdateUI();
            if (clnt.connectionStatus == 1)
            {
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
            }

            UpdateUI();
        }

        void UpdateUI()
        {
            clnt.Client(usernameBox.Text.Trim(), passwordBox.Text.Trim(), stayLoggedIn.Checked);

            if (clnt.connectionStatus == 0)
            {
                CannotConnectPanel.Visible = true;
                CannotConnectPanel.Enabled = true;
            }
            else
            {
                CannotConnectPanel.Visible = false;
                CannotConnectPanel.Enabled = false;
                EstimateCost();
            }
            walletAddr.Text = clnt.walletInfo.Address;
            computeCoins.Text = "Balance: $" + Math.Round(clnt.walletInfo.Balance, 4);
            pendingFunds.Text = "Pending: $" + Math.Round(clnt.walletInfo.PendingBalance, 4);
            QRCodeWallet.Image = clnt.qrCodeAsBitmap;

            for (int i = 0; i < 5; i++)
            {
                if (i < clnt.ownedPrograms.Count())
                {
                    if (i == 0)
                    {
                        ProgramStatusPanel1.Visible = true;
                        ProgramName1.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum1.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 1)
                    {
                        ProgramStatusPanel2.Visible = true;
                        ProgramName2.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum2.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 2)
                    {
                        ProgramStatusPanel3.Visible = true;
                        ProgramName3.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum3.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 3)
                    {
                        ProgramStatusPanel4.Visible = true;
                        ProgramName4.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum4.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 4)
                    {
                        ProgramStatusPanel5.Visible = true;
                        ProgramName5.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum5.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                }
                else
                {
                    if (i == 0)
                    {
                        ProgramStatusPanel1.Visible = false;
                        ProgramName1.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum1.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 1)
                    {
                        ProgramStatusPanel2.Visible = false;
                        ProgramName2.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum2.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 2)
                    {
                        ProgramStatusPanel3.Visible = false;
                        ProgramName3.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum3.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 3)
                    {
                        ProgramStatusPanel4.Visible = false;
                        ProgramName4.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum4.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                    if (i == 4)
                    {
                        ProgramStatusPanel5.Visible = false;
                        ProgramName5.Text = clnt.ownedPrograms[i].Zip;
                        ActiveTimeNum5.Text = clnt.ownedPrograms[i].MinutesLeft.ToString();
                    }
                }
            }
        }

        void EstimateCost()
        {
            if (clnt.connectionStatus == 1)
                estimatedCostNumber.Text = "$" + (clnt.walletInfo.CostPerMinute + (ComputationPowerSlider.Value / 10f) * clnt.walletInfo.CostPerMinute) * (float)TotalMinutesBox.Value;
        }

        private void refreshButton_Click(object sender, EventArgs e)
        {
            UpdateUI();

            //for (int i = 0; i < 5; i++)
            //{
            //    int num = ProgramList.Controls.Count;
            //    Control prev = ProgramList.Controls[num - 1];

            //    //uxMsgTxt.Text += "Adding Panel" +i.ToString() + "rn";
            //    Panel pnl = new Panel();
            //    pnl.Location = new Point(prev.Location.X, prev.Location.Y + 43);
            //    pnl.BorderStyle = BorderStyle.FixedSingle;
            //    pnl.BackColor = prev.BackColor;
            //    pnl.Size = prev.Size;
            //    pnl.Visible = true;

            //    foreach (Control c in prev.Controls)
            //    {
            //        Control c2 = new Control();
            //        if (c.GetType() == typeof(TextBox))
            //            c2 = new TextBox();
            //        if (c.GetType() == typeof(Label))
            //            c2 = new Label();
            //        if (c.GetType() == typeof(CheckBox))
            //            c2 = new CheckBox();
            //        if (c.GetType() == typeof(DataGridView))
            //            c2 = new DataGridView();
            //        if (c.GetType() == typeof(Button))
            //        {
            //            c2 = new Button();
            //            programSyncButtons.Add(c2);
            //            programSyncButtons[programSyncButtons.Count - 1].Click += delegate (object s, EventArgs a) { ProgramSyncButtonClick(s, a, i); };
            //        }
            //        c2.Location = c.Location;
            //        c2.Size = c.Size;
            //        c2.Text = c.Text;
            //        pnl.Controls.Add(c2);
            //    }

            //    ProgramList.Controls.Add(pnl);
            //}
        }


        //private void ProgramSyncButtonClick(object sender, EventArgs e, int id)
        //{
        //    Console.WriteLine(id);
        //}
    }
}