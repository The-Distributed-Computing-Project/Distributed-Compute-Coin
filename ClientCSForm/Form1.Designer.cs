
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
			this.walletAddr = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.computeCoins = new System.Windows.Forms.Label();
			this.sendButton = new System.Windows.Forms.Button();
			this.panel1 = new System.Windows.Forms.Panel();
			this.fieldsNotFilledWarn = new System.Windows.Forms.Label();
			this.sendToAmount = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.sendToWallet = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.panel2 = new System.Windows.Forms.Panel();
			this.logInBtn = new System.Windows.Forms.Button();
			this.LoginWallet = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.aFRLogin = new System.Windows.Forms.Label();
			this.panel1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// walletAddr
			// 
			this.walletAddr.AutoSize = true;
			this.walletAddr.Location = new System.Drawing.Point(13, 13);
			this.walletAddr.Name = "walletAddr";
			this.walletAddr.Size = new System.Drawing.Size(50, 13);
			this.walletAddr.TabIndex = 0;
			this.walletAddr.Text = "Wallet: ";
			this.walletAddr.Click += new System.EventHandler(this.label1_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(1095, 315);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(41, 13);
			this.label1.TabIndex = 1;
			this.label1.Text = "label1";
			// 
			// computeCoins
			// 
			this.computeCoins.AutoSize = true;
			this.computeCoins.Location = new System.Drawing.Point(13, 37);
			this.computeCoins.Name = "computeCoins";
			this.computeCoins.Size = new System.Drawing.Size(111, 13);
			this.computeCoins.TabIndex = 2;
			this.computeCoins.Text = "Compute Coins: $";
			// 
			// sendButton
			// 
			this.sendButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
			this.sendButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.sendButton.ForeColor = System.Drawing.SystemColors.Control;
			this.sendButton.Location = new System.Drawing.Point(25, 229);
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
			this.panel1.Controls.Add(this.fieldsNotFilledWarn);
			this.panel1.Controls.Add(this.sendToAmount);
			this.panel1.Controls.Add(this.label4);
			this.panel1.Controls.Add(this.sendToWallet);
			this.panel1.Controls.Add(this.label3);
			this.panel1.Controls.Add(this.sendButton);
			this.panel1.Location = new System.Drawing.Point(411, 60);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(127, 273);
			this.panel1.TabIndex = 4;
			// 
			// fieldsNotFilledWarn
			// 
			this.fieldsNotFilledWarn.AutoSize = true;
			this.fieldsNotFilledWarn.ForeColor = System.Drawing.Color.Red;
			this.fieldsNotFilledWarn.Location = new System.Drawing.Point(8, 145);
			this.fieldsNotFilledWarn.Name = "fieldsNotFilledWarn";
			this.fieldsNotFilledWarn.Size = new System.Drawing.Size(0, 13);
			this.fieldsNotFilledWarn.TabIndex = 8;
			this.fieldsNotFilledWarn.Click += new System.EventHandler(this.fieldsNotFilledWarn_Click);
			// 
			// sendToAmount
			// 
			this.sendToAmount.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
			this.sendToAmount.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.sendToAmount.ForeColor = System.Drawing.SystemColors.Control;
			this.sendToAmount.Location = new System.Drawing.Point(3, 102);
			this.sendToAmount.Name = "sendToAmount";
			this.sendToAmount.Size = new System.Drawing.Size(119, 21);
			this.sendToAmount.TabIndex = 7;
			this.sendToAmount.TextChanged += new System.EventHandler(this.sendToAmount_TextChanged);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(4, 84);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(56, 13);
			this.label4.TabIndex = 6;
			this.label4.Text = "Amount:";
			// 
			// sendToWallet
			// 
			this.sendToWallet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
			this.sendToWallet.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.sendToWallet.ForeColor = System.Drawing.SystemColors.Control;
			this.sendToWallet.Location = new System.Drawing.Point(3, 48);
			this.sendToWallet.Name = "sendToWallet";
			this.sendToWallet.Size = new System.Drawing.Size(119, 21);
			this.sendToWallet.TabIndex = 5;
			this.sendToWallet.TextChanged += new System.EventHandler(this.sendToWallet_TextChanged);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(4, 30);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(46, 13);
			this.label3.TabIndex = 4;
			this.label3.Text = "Wallet:";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 13F);
			this.label2.Location = new System.Drawing.Point(445, 28);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(58, 22);
			this.label2.TabIndex = 4;
			this.label2.Text = "Trade";
			// 
			// panel2
			// 
			this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.panel2.Controls.Add(this.aFRLogin);
			this.panel2.Controls.Add(this.logInBtn);
			this.panel2.Controls.Add(this.LoginWallet);
			this.panel2.Controls.Add(this.label6);
			this.panel2.Controls.Add(this.label5);
			this.panel2.Location = new System.Drawing.Point(13, 72);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(205, 147);
			this.panel2.TabIndex = 5;
			// 
			// logInBtn
			// 
			this.logInBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
			this.logInBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.logInBtn.ForeColor = System.Drawing.SystemColors.Control;
			this.logInBtn.Location = new System.Drawing.Point(38, 104);
			this.logInBtn.Name = "logInBtn";
			this.logInBtn.Size = new System.Drawing.Size(75, 23);
			this.logInBtn.TabIndex = 11;
			this.logInBtn.Text = "Log in";
			this.logInBtn.UseVisualStyleBackColor = false;
			this.logInBtn.Click += new System.EventHandler(this.button1_Click_1);
			// 
			// LoginWallet
			// 
			this.LoginWallet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
			this.LoginWallet.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.LoginWallet.ForeColor = System.Drawing.SystemColors.Control;
			this.LoginWallet.Location = new System.Drawing.Point(13, 65);
			this.LoginWallet.Name = "LoginWallet";
			this.LoginWallet.Size = new System.Drawing.Size(119, 21);
			this.LoginWallet.TabIndex = 10;
			this.LoginWallet.TextChanged += new System.EventHandler(this.LoginWallet_TextChanged);
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(14, 47);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(46, 13);
			this.label6.TabIndex = 9;
			this.label6.Text = "Wallet:";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(10, 11);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(48, 13);
			this.label5.TabIndex = 0;
			this.label5.Text = "Sign In";
			// 
			// aFRLogin
			// 
			this.aFRLogin.AutoSize = true;
			this.aFRLogin.ForeColor = System.Drawing.Color.Red;
			this.aFRLogin.Location = new System.Drawing.Point(60, 47);
			this.aFRLogin.Name = "aFRLogin";
			this.aFRLogin.Size = new System.Drawing.Size(0, 13);
			this.aFRLogin.TabIndex = 12;
			// 
			// Form1
			// 
			this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
			this.ClientSize = new System.Drawing.Size(550, 354);
			this.Controls.Add(this.panel2);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.computeCoins);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.walletAddr);
			this.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.ForeColor = System.Drawing.SystemColors.Control;
			this.Name = "Form1";
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.panel2.ResumeLayout(false);
			this.panel2.PerformLayout();
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
		private System.Windows.Forms.TextBox sendToAmount;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox sendToWallet;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label fieldsNotFilledWarn;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.Button logInBtn;
		private System.Windows.Forms.TextBox LoginWallet;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label aFRLogin;
	}
}

