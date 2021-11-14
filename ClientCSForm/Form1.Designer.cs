
namespace ClientCSForm
{
	partial class Form1
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            this.walletAddr = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.computeCoins = new System.Windows.Forms.Label();
            this.sendButton = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.sendToAmount = new System.Windows.Forms.NumericUpDown();
            this.fieldsNotFilledWarn = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.sendToWallet = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.stayLoggedIn = new System.Windows.Forms.CheckBox();
            this.passwordBox = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.aFRLogin = new System.Windows.Forms.Label();
            this.logInBtn = new System.Windows.Forms.Button();
            this.usernameBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.tradeCover = new System.Windows.Forms.Panel();
            this.SelectFileButton = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.TotalMinutesBox = new System.Windows.Forms.NumericUpDown();
            this.panel3 = new System.Windows.Forms.Panel();
            this.estimatedCostNumber = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.uploadErrorText = new System.Windows.Forms.Label();
            this.submitFileButton = new System.Windows.Forms.Button();
            this.learnMore = new System.Windows.Forms.LinkLabel();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.ComputationPowerSlider = new System.Windows.Forms.TrackBar();
            this.fileLocation = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.compSliderTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.totalMinutesTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.zippedRustFolderTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.submitFileBtnTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.pendingFunds = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.sendToAmount)).BeginInit();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.TotalMinutesBox)).BeginInit();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ComputationPowerSlider)).BeginInit();
            this.SuspendLayout();
            // 
            // walletAddr
            // 
            this.walletAddr.AutoSize = true;
            this.walletAddr.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.walletAddr.Font = new System.Drawing.Font("Verdana", 10.25F);
            this.walletAddr.Location = new System.Drawing.Point(13, 13);
            this.walletAddr.Name = "walletAddr";
            this.walletAddr.Size = new System.Drawing.Size(82, 24);
            this.walletAddr.TabIndex = 0;
            this.walletAddr.Text = "Wallet: ";
            this.walletAddr.Click += new System.EventHandler(this.label1_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(1095, 315);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(48, 17);
            this.label1.TabIndex = 1;
            this.label1.Text = "label1";
            // 
            // computeCoins
            // 
            this.computeCoins.AutoSize = true;
            this.computeCoins.Font = new System.Drawing.Font("Verdana", 10.25F);
            this.computeCoins.Location = new System.Drawing.Point(13, 37);
            this.computeCoins.Name = "computeCoins";
            this.computeCoins.Size = new System.Drawing.Size(116, 22);
            this.computeCoins.TabIndex = 2;
            this.computeCoins.Text = "Balance: $0";
            // 
            // sendButton
            // 
            this.sendButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
            this.sendButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sendButton.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.sendButton.ForeColor = System.Drawing.SystemColors.Control;
            this.sendButton.Location = new System.Drawing.Point(35, 173);
            this.sendButton.Name = "sendButton";
            this.sendButton.Size = new System.Drawing.Size(75, 23);
            this.sendButton.TabIndex = 3;
            this.sendButton.Text = "Send";
            this.sendButton.UseVisualStyleBackColor = false;
            this.sendButton.Click += new System.EventHandler(this.sendButton_Click);
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.sendToAmount);
            this.panel1.Controls.Add(this.fieldsNotFilledWarn);
            this.panel1.Controls.Add(this.label4);
            this.panel1.Controls.Add(this.sendToWallet);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.sendButton);
            this.panel1.Location = new System.Drawing.Point(631, 101);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(152, 209);
            this.panel1.TabIndex = 4;
            // 
            // sendToAmount
            // 
            this.sendToAmount.BackColor = System.Drawing.SystemColors.MenuText;
            this.sendToAmount.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.sendToAmount.DecimalPlaces = 1;
            this.sendToAmount.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.sendToAmount.ForeColor = System.Drawing.SystemColors.Info;
            this.sendToAmount.Location = new System.Drawing.Point(14, 120);
            this.sendToAmount.Name = "sendToAmount";
            this.sendToAmount.Size = new System.Drawing.Size(120, 24);
            this.sendToAmount.TabIndex = 12;
            // 
            // fieldsNotFilledWarn
            // 
            this.fieldsNotFilledWarn.AutoSize = true;
            this.fieldsNotFilledWarn.ForeColor = System.Drawing.Color.Red;
            this.fieldsNotFilledWarn.Location = new System.Drawing.Point(8, 145);
            this.fieldsNotFilledWarn.Name = "fieldsNotFilledWarn";
            this.fieldsNotFilledWarn.Size = new System.Drawing.Size(0, 17);
            this.fieldsNotFilledWarn.TabIndex = 8;
            this.fieldsNotFilledWarn.Click += new System.EventHandler(this.fieldsNotFilledWarn_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 98);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 17);
            this.label4.TabIndex = 6;
            this.label4.Text = "Amount:";
            // 
            // sendToWallet
            // 
            this.sendToWallet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
            this.sendToWallet.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.sendToWallet.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.sendToWallet.ForeColor = System.Drawing.SystemColors.Control;
            this.sendToWallet.Location = new System.Drawing.Point(11, 62);
            this.sendToWallet.MaxLength = 68;
            this.sendToWallet.Name = "sendToWallet";
            this.sendToWallet.Size = new System.Drawing.Size(119, 24);
            this.sendToWallet.TabIndex = 5;
            this.sendToWallet.TextChanged += new System.EventHandler(this.sendToWallet_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label3.Location = new System.Drawing.Point(12, 44);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(56, 17);
            this.label3.TabIndex = 4;
            this.label3.Text = "Wallet:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Verdana", 9.25F);
            this.label2.Location = new System.Drawing.Point(8, 11);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(52, 18);
            this.label2.TabIndex = 4;
            this.label2.Text = "Trade";
            // 
            // panel2
            // 
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel2.Controls.Add(this.stayLoggedIn);
            this.panel2.Controls.Add(this.passwordBox);
            this.panel2.Controls.Add(this.label7);
            this.panel2.Controls.Add(this.aFRLogin);
            this.panel2.Controls.Add(this.logInBtn);
            this.panel2.Controls.Add(this.usernameBox);
            this.panel2.Controls.Add(this.label6);
            this.panel2.Controls.Add(this.label5);
            this.panel2.Location = new System.Drawing.Point(13, 101);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(170, 209);
            this.panel2.TabIndex = 5;
            // 
            // stayLoggedIn
            // 
            this.stayLoggedIn.AutoSize = true;
            this.stayLoggedIn.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.stayLoggedIn.Location = new System.Drawing.Point(17, 139);
            this.stayLoggedIn.Name = "stayLoggedIn";
            this.stayLoggedIn.Size = new System.Drawing.Size(138, 21);
            this.stayLoggedIn.TabIndex = 15;
            this.stayLoggedIn.Text = "Stay Logged In";
            this.stayLoggedIn.UseVisualStyleBackColor = true;
            // 
            // passwordBox
            // 
            this.passwordBox.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
            this.passwordBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.passwordBox.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.passwordBox.ForeColor = System.Drawing.SystemColors.Control;
            this.passwordBox.Location = new System.Drawing.Point(13, 108);
            this.passwordBox.Name = "passwordBox";
            this.passwordBox.PasswordChar = '*';
            this.passwordBox.Size = new System.Drawing.Size(119, 24);
            this.passwordBox.TabIndex = 14;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label7.Location = new System.Drawing.Point(14, 90);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(81, 17);
            this.label7.TabIndex = 13;
            this.label7.Text = "Password:";
            // 
            // aFRLogin
            // 
            this.aFRLogin.AutoSize = true;
            this.aFRLogin.ForeColor = System.Drawing.Color.Red;
            this.aFRLogin.Location = new System.Drawing.Point(60, 47);
            this.aFRLogin.Name = "aFRLogin";
            this.aFRLogin.Size = new System.Drawing.Size(0, 17);
            this.aFRLogin.TabIndex = 12;
            // 
            // logInBtn
            // 
            this.logInBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
            this.logInBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.logInBtn.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.logInBtn.ForeColor = System.Drawing.SystemColors.Control;
            this.logInBtn.Location = new System.Drawing.Point(33, 170);
            this.logInBtn.Name = "logInBtn";
            this.logInBtn.Size = new System.Drawing.Size(75, 23);
            this.logInBtn.TabIndex = 11;
            this.logInBtn.Text = "Log in";
            this.logInBtn.UseVisualStyleBackColor = false;
            this.logInBtn.Click += new System.EventHandler(this.logInBtn_Click_1);
            // 
            // usernameBox
            // 
            this.usernameBox.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
            this.usernameBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.usernameBox.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.usernameBox.ForeColor = System.Drawing.SystemColors.Control;
            this.usernameBox.Location = new System.Drawing.Point(13, 65);
            this.usernameBox.Name = "usernameBox";
            this.usernameBox.Size = new System.Drawing.Size(119, 24);
            this.usernameBox.TabIndex = 10;
            this.usernameBox.TextChanged += new System.EventHandler(this.LoginWallet_TextChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label6.Location = new System.Drawing.Point(14, 47);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(84, 17);
            this.label6.TabIndex = 9;
            this.label6.Text = "Username:";
            this.label6.Click += new System.EventHandler(this.label6_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Verdana", 9.25F);
            this.label5.Location = new System.Drawing.Point(10, 11);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(66, 18);
            this.label5.TabIndex = 0;
            this.label5.Text = "Sign In";
            // 
            // tradeCover
            // 
            this.tradeCover.Location = new System.Drawing.Point(578, 13);
            this.tradeCover.Name = "tradeCover";
            this.tradeCover.Size = new System.Drawing.Size(206, 46);
            this.tradeCover.TabIndex = 6;
            // 
            // SelectFileButton
            // 
            this.SelectFileButton.BackColor = System.Drawing.Color.Black;
            this.SelectFileButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.SelectFileButton.ForeColor = System.Drawing.SystemColors.Control;
            this.SelectFileButton.Location = new System.Drawing.Point(16, 111);
            this.SelectFileButton.Name = "SelectFileButton";
            this.SelectFileButton.Size = new System.Drawing.Size(124, 23);
            this.SelectFileButton.TabIndex = 7;
            this.SelectFileButton.Text = "Select File";
            this.SelectFileButton.UseVisualStyleBackColor = false;
            this.SelectFileButton.Click += new System.EventHandler(this.SelectFileButton_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label8.Location = new System.Drawing.Point(13, 139);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(106, 17);
            this.label8.TabIndex = 9;
            this.label8.Text = "Total Minutes:";
            // 
            // TotalMinutesBox
            // 
            this.TotalMinutesBox.BackColor = System.Drawing.SystemColors.MenuText;
            this.TotalMinutesBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.TotalMinutesBox.DecimalPlaces = 1;
            this.TotalMinutesBox.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.TotalMinutesBox.ForeColor = System.Drawing.SystemColors.Info;
            this.TotalMinutesBox.Location = new System.Drawing.Point(16, 160);
            this.TotalMinutesBox.Maximum = new decimal(new int[] {
            120,
            0,
            0,
            0});
            this.TotalMinutesBox.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.TotalMinutesBox.Name = "TotalMinutesBox";
            this.TotalMinutesBox.Size = new System.Drawing.Size(120, 24);
            this.TotalMinutesBox.TabIndex = 11;
            this.TotalMinutesBox.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.TotalMinutesBox.ValueChanged += new System.EventHandler(this.TotalMinutesBox_TextChanged);
            // 
            // panel3
            // 
            this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel3.Controls.Add(this.estimatedCostNumber);
            this.panel3.Controls.Add(this.label14);
            this.panel3.Controls.Add(this.uploadErrorText);
            this.panel3.Controls.Add(this.submitFileButton);
            this.panel3.Controls.Add(this.learnMore);
            this.panel3.Controls.Add(this.label13);
            this.panel3.Controls.Add(this.label12);
            this.panel3.Controls.Add(this.ComputationPowerSlider);
            this.panel3.Controls.Add(this.fileLocation);
            this.panel3.Controls.Add(this.SelectFileButton);
            this.panel3.Controls.Add(this.label10);
            this.panel3.Controls.Add(this.label11);
            this.panel3.Controls.Add(this.label8);
            this.panel3.Controls.Add(this.TotalMinutesBox);
            this.panel3.Controls.Add(this.label9);
            this.panel3.Location = new System.Drawing.Point(254, 101);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(298, 402);
            this.panel3.TabIndex = 12;
            // 
            // estimatedCostNumber
            // 
            this.estimatedCostNumber.AutoSize = true;
            this.estimatedCostNumber.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.estimatedCostNumber.ForeColor = System.Drawing.Color.ForestGreen;
            this.estimatedCostNumber.Location = new System.Drawing.Point(127, 366);
            this.estimatedCostNumber.Name = "estimatedCostNumber";
            this.estimatedCostNumber.Size = new System.Drawing.Size(26, 17);
            this.estimatedCostNumber.TabIndex = 9;
            this.estimatedCostNumber.Text = "$0";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label14.Location = new System.Drawing.Point(10, 366);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(122, 17);
            this.label14.TabIndex = 9;
            this.label14.Text = "Estimated Cost:";
            // 
            // uploadErrorText
            // 
            this.uploadErrorText.AutoSize = true;
            this.uploadErrorText.ForeColor = System.Drawing.Color.Red;
            this.uploadErrorText.Location = new System.Drawing.Point(13, 304);
            this.uploadErrorText.Name = "uploadErrorText";
            this.uploadErrorText.Size = new System.Drawing.Size(0, 17);
            this.uploadErrorText.TabIndex = 16;
            // 
            // submitFileButton
            // 
            this.submitFileButton.BackColor = System.Drawing.Color.Black;
            this.submitFileButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.submitFileButton.ForeColor = System.Drawing.SystemColors.Control;
            this.submitFileButton.Location = new System.Drawing.Point(171, 338);
            this.submitFileButton.Name = "submitFileButton";
            this.submitFileButton.Size = new System.Drawing.Size(103, 23);
            this.submitFileButton.TabIndex = 7;
            this.submitFileButton.Text = "Submit File";
            this.submitFileButton.UseVisualStyleBackColor = false;
            this.submitFileButton.Click += new System.EventHandler(this.submitFileButton_Click);
            // 
            // learnMore
            // 
            this.learnMore.ActiveLinkColor = System.Drawing.Color.LightCoral;
            this.learnMore.AutoSize = true;
            this.learnMore.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.learnMore.LinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(128)))));
            this.learnMore.Location = new System.Drawing.Point(10, 44);
            this.learnMore.MaximumSize = new System.Drawing.Size(280, 0);
            this.learnMore.Name = "learnMore";
            this.learnMore.Size = new System.Drawing.Size(86, 17);
            this.learnMore.TabIndex = 15;
            this.learnMore.TabStop = true;
            this.learnMore.Text = "Learn More";
            this.learnMore.VisitedLinkColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(64)))), ((int)(((byte)(0)))));
            this.learnMore.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LearnMore_LinkClicked);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label13.Location = new System.Drawing.Point(167, 225);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(21, 17);
            this.label13.TabIndex = 9;
            this.label13.Text = "Hi";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label12.Location = new System.Drawing.Point(9, 225);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(25, 17);
            this.label12.TabIndex = 9;
            this.label12.Text = "Lo";
            // 
            // ComputationPowerSlider
            // 
            this.ComputationPowerSlider.LargeChange = 2;
            this.ComputationPowerSlider.Location = new System.Drawing.Point(27, 216);
            this.ComputationPowerSlider.Maximum = 5;
            this.ComputationPowerSlider.Minimum = 1;
            this.ComputationPowerSlider.Name = "ComputationPowerSlider";
            this.ComputationPowerSlider.Size = new System.Drawing.Size(137, 56);
            this.ComputationPowerSlider.TabIndex = 14;
            this.ComputationPowerSlider.Value = 1;
            this.ComputationPowerSlider.ValueChanged += new System.EventHandler(this.ComputationPowerSlider_TextChanged);
            // 
            // fileLocation
            // 
            this.fileLocation.AutoSize = true;
            this.fileLocation.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.fileLocation.ForeColor = System.Drawing.SystemColors.ControlDark;
            this.fileLocation.Location = new System.Drawing.Point(155, 114);
            this.fileLocation.MaximumSize = new System.Drawing.Size(118, 0);
            this.fileLocation.Name = "fileLocation";
            this.fileLocation.Size = new System.Drawing.Size(118, 17);
            this.fileLocation.TabIndex = 12;
            this.fileLocation.Text = "No File Selected";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label10.Location = new System.Drawing.Point(13, 91);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(148, 17);
            this.label10.TabIndex = 9;
            this.label10.Text = "Zipped Rust Folder:";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.label11.Location = new System.Drawing.Point(13, 196);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(151, 17);
            this.label11.TabIndex = 9;
            this.label11.Text = "How Computational:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Verdana", 9.25F);
            this.label9.Location = new System.Drawing.Point(10, 11);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(138, 18);
            this.label9.TabIndex = 0;
            this.label9.Text = "Upload Program";
            // 
            // compSliderTooltip
            // 
            this.compSliderTooltip.AutoPopDelay = 15000;
            this.compSliderTooltip.InitialDelay = 500;
            this.compSliderTooltip.ReshowDelay = 100;
            // 
            // totalMinutesTooltip
            // 
            this.totalMinutesTooltip.AutoPopDelay = 15000;
            this.totalMinutesTooltip.InitialDelay = 500;
            this.totalMinutesTooltip.ReshowDelay = 100;
            // 
            // zippedRustFolderTooltip
            // 
            this.zippedRustFolderTooltip.AutoPopDelay = 15000;
            this.zippedRustFolderTooltip.InitialDelay = 500;
            this.zippedRustFolderTooltip.ReshowDelay = 100;
            // 
            // submitFileBtnTooltip
            // 
            this.submitFileBtnTooltip.AutoPopDelay = 15000;
            this.submitFileBtnTooltip.InitialDelay = 500;
            this.submitFileBtnTooltip.ReshowDelay = 100;
            // 
            // pendingFunds
            // 
            this.pendingFunds.AutoSize = true;
            this.pendingFunds.Font = new System.Drawing.Font("Verdana", 10.25F);
            this.pendingFunds.Location = new System.Drawing.Point(13, 59);
            this.pendingFunds.Name = "pendingFunds";
            this.pendingFunds.Size = new System.Drawing.Size(117, 22);
            this.pendingFunds.TabIndex = 2;
            this.pendingFunds.Text = "Pending: $0";
            // 
            // Form1
            // 
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.ClientSize = new System.Drawing.Size(796, 624);
            this.Controls.Add(this.tradeCover);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.pendingFunds);
            this.Controls.Add(this.computeCoins);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.walletAddr);
            this.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ForeColor = System.Drawing.SystemColors.Control;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "Form1";
            this.Text = "Compute Coin Client";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.sendToAmount)).EndInit();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.TotalMinutesBox)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ComputationPowerSlider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button tradeBtn;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Label wallet;
		private System.Windows.Forms.Label ownedAmount;
		private System.Windows.Forms.Label walletAddr;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label computeCoins;
		private System.Windows.Forms.Button sendButton;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox sendToWallet;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label fieldsNotFilledWarn;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.Button logInBtn;
		private System.Windows.Forms.TextBox usernameBox;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label aFRLogin;
        private System.Windows.Forms.TextBox passwordBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.CheckBox stayLoggedIn;
        private System.Windows.Forms.Panel tradeCover;
        private System.Windows.Forms.Button SelectFileButton;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.NumericUpDown TotalMinutesBox;
        private System.Windows.Forms.NumericUpDown sendToAmount;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label fileLocation;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TrackBar ComputationPowerSlider;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ToolTip compSliderTooltip;
        private System.Windows.Forms.ToolTip totalMinutesTooltip;
        private System.Windows.Forms.LinkLabel learnMore;
        private System.Windows.Forms.ToolTip zippedRustFolderTooltip;
        private System.Windows.Forms.Button submitFileButton;
        private System.Windows.Forms.ToolTip submitFileBtnTooltip;
        private System.Windows.Forms.Label uploadErrorText;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label estimatedCostNumber;
        private System.Windows.Forms.Label pendingFunds;
    }
}

