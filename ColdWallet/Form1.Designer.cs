
namespace ColdWallet
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.walletLabel = new System.Windows.Forms.Label();
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
            this.tradeCover = new System.Windows.Forms.Panel();
            this.compSliderTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.totalMinutesTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.zippedRustFolderTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.submitFileBtnTooltip = new System.Windows.Forms.ToolTip(this.components);
            this.pendingFunds = new System.Windows.Forms.Label();
            this.walletAddr = new System.Windows.Forms.TextBox();
            this.refreshButton = new System.Windows.Forms.Button();
            this.panel4 = new System.Windows.Forms.Panel();
            this.QRCodeWallet = new System.Windows.Forms.PictureBox();
            this.label16 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.sendToAmount)).BeginInit();
            this.panel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.QRCodeWallet)).BeginInit();
            this.SuspendLayout();
            // 
            // walletLabel
            // 
            this.walletLabel.AutoSize = true;
            this.walletLabel.Font = new System.Drawing.Font("Arial", 11.25F);
            this.walletLabel.Location = new System.Drawing.Point(13, 13);
            this.walletLabel.Name = "walletLabel";
            this.walletLabel.Size = new System.Drawing.Size(72, 22);
            this.walletLabel.TabIndex = 0;
            this.walletLabel.Text = "Wallet: ";
            this.walletLabel.Click += new System.EventHandler(this.label1_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial", 9.75F);
            this.label1.Location = new System.Drawing.Point(1095, 315);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(51, 19);
            this.label1.TabIndex = 1;
            this.label1.Text = "label1";
            // 
            // computeCoins
            // 
            this.computeCoins.AutoSize = true;
            this.computeCoins.Font = new System.Drawing.Font("Arial", 11.25F);
            this.computeCoins.Location = new System.Drawing.Point(13, 37);
            this.computeCoins.Name = "computeCoins";
            this.computeCoins.Size = new System.Drawing.Size(110, 22);
            this.computeCoins.TabIndex = 2;
            this.computeCoins.Text = "Balance: $0";
            // 
            // sendButton
            // 
            this.sendButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
            this.sendButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.sendButton.Font = new System.Drawing.Font("Arial", 9.75F);
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
            this.panel1.Location = new System.Drawing.Point(17, 101);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(317, 209);
            this.panel1.TabIndex = 4;
            // 
            // sendToAmount
            // 
            this.sendToAmount.BackColor = System.Drawing.SystemColors.MenuText;
            this.sendToAmount.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.sendToAmount.DecimalPlaces = 1;
            this.sendToAmount.Font = new System.Drawing.Font("Arial", 9.75F);
            this.sendToAmount.ForeColor = System.Drawing.SystemColors.Info;
            this.sendToAmount.Location = new System.Drawing.Point(14, 120);
            this.sendToAmount.Maximum = new decimal(new int[] {
            1000000,
            0,
            0,
            0});
            this.sendToAmount.Name = "sendToAmount";
            this.sendToAmount.Size = new System.Drawing.Size(120, 26);
            this.sendToAmount.TabIndex = 12;
            // 
            // fieldsNotFilledWarn
            // 
            this.fieldsNotFilledWarn.AutoSize = true;
            this.fieldsNotFilledWarn.Font = new System.Drawing.Font("Arial", 9.75F);
            this.fieldsNotFilledWarn.ForeColor = System.Drawing.Color.Red;
            this.fieldsNotFilledWarn.Location = new System.Drawing.Point(8, 145);
            this.fieldsNotFilledWarn.Name = "fieldsNotFilledWarn";
            this.fieldsNotFilledWarn.Size = new System.Drawing.Size(0, 19);
            this.fieldsNotFilledWarn.TabIndex = 8;
            this.fieldsNotFilledWarn.Click += new System.EventHandler(this.fieldsNotFilledWarn_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Arial", 9.75F);
            this.label4.Location = new System.Drawing.Point(12, 98);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(69, 19);
            this.label4.TabIndex = 6;
            this.label4.Text = "Amount:";
            // 
            // sendToWallet
            // 
            this.sendToWallet.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(20)))), ((int)(((byte)(20)))), ((int)(((byte)(20)))));
            this.sendToWallet.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.sendToWallet.Font = new System.Drawing.Font("Arial", 9.75F);
            this.sendToWallet.ForeColor = System.Drawing.SystemColors.Control;
            this.sendToWallet.Location = new System.Drawing.Point(11, 62);
            this.sendToWallet.MaxLength = 68;
            this.sendToWallet.Name = "sendToWallet";
            this.sendToWallet.Size = new System.Drawing.Size(119, 26);
            this.sendToWallet.TabIndex = 5;
            this.sendToWallet.TextChanged += new System.EventHandler(this.sendToWallet_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Arial", 9.75F);
            this.label3.Location = new System.Drawing.Point(12, 44);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(58, 19);
            this.label3.TabIndex = 4;
            this.label3.Text = "Wallet:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Arial", 9.75F);
            this.label2.Location = new System.Drawing.Point(8, 11);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 19);
            this.label2.TabIndex = 4;
            this.label2.Text = "Trade";
            // 
            // tradeCover
            // 
            this.tradeCover.Location = new System.Drawing.Point(312, 79);
            this.tradeCover.Name = "tradeCover";
            this.tradeCover.Size = new System.Drawing.Size(22, 16);
            this.tradeCover.TabIndex = 6;
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
            this.pendingFunds.Font = new System.Drawing.Font("Arial", 11.25F);
            this.pendingFunds.Location = new System.Drawing.Point(13, 59);
            this.pendingFunds.Name = "pendingFunds";
            this.pendingFunds.Size = new System.Drawing.Size(112, 22);
            this.pendingFunds.TabIndex = 2;
            this.pendingFunds.Text = "Pending: $0";
            // 
            // walletAddr
            // 
            this.walletAddr.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.walletAddr.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.walletAddr.Font = new System.Drawing.Font("Consolas", 9.75F);
            this.walletAddr.ForeColor = System.Drawing.SystemColors.Info;
            this.walletAddr.Location = new System.Drawing.Point(79, 12);
            this.walletAddr.Name = "walletAddr";
            this.walletAddr.ReadOnly = true;
            this.walletAddr.Size = new System.Drawing.Size(484, 27);
            this.walletAddr.TabIndex = 7;
            this.walletAddr.WordWrap = false;
            // 
            // refreshButton
            // 
            this.refreshButton.BackColor = System.Drawing.Color.Transparent;
            this.refreshButton.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("refreshButton.BackgroundImage")));
            this.refreshButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.refreshButton.Cursor = System.Windows.Forms.Cursors.Hand;
            this.refreshButton.FlatAppearance.BorderSize = 0;
            this.refreshButton.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.refreshButton.FlatAppearance.MouseOverBackColor = System.Drawing.Color.Black;
            this.refreshButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.refreshButton.Font = new System.Drawing.Font("Consolas", 9.75F);
            this.refreshButton.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.refreshButton.Location = new System.Drawing.Point(569, 12);
            this.refreshButton.Name = "refreshButton";
            this.refreshButton.Size = new System.Drawing.Size(22, 22);
            this.refreshButton.TabIndex = 14;
            this.refreshButton.UseVisualStyleBackColor = false;
            this.refreshButton.Click += new System.EventHandler(this.refreshButton_Click);
            // 
            // panel4
            // 
            this.panel4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel4.Controls.Add(this.QRCodeWallet);
            this.panel4.Controls.Add(this.label16);
            this.panel4.Location = new System.Drawing.Point(372, 57);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(219, 253);
            this.panel4.TabIndex = 16;
            // 
            // QRCodeWallet
            // 
            this.QRCodeWallet.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.QRCodeWallet.Location = new System.Drawing.Point(18, 48);
            this.QRCodeWallet.Name = "QRCodeWallet";
            this.QRCodeWallet.Size = new System.Drawing.Size(185, 185);
            this.QRCodeWallet.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.QRCodeWallet.TabIndex = 1;
            this.QRCodeWallet.TabStop = false;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(14, 12);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(191, 19);
            this.label16.TabIndex = 0;
            this.label16.Text = "Your wallet as a QR code";
            // 
            // Form1
            // 
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.ClientSize = new System.Drawing.Size(603, 322);
            this.Controls.Add(this.panel4);
            this.Controls.Add(this.refreshButton);
            this.Controls.Add(this.walletAddr);
            this.Controls.Add(this.tradeCover);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.pendingFunds);
            this.Controls.Add(this.computeCoins);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.walletLabel);
            this.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ForeColor = System.Drawing.SystemColors.Control;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "Cold Wallet";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.sendToAmount)).EndInit();
            this.panel4.ResumeLayout(false);
            this.panel4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.QRCodeWallet)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button tradeBtn;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Label wallet;
		private System.Windows.Forms.Label ownedAmount;
		private System.Windows.Forms.Label walletLabel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label computeCoins;
		private System.Windows.Forms.Button sendButton;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox sendToWallet;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label fieldsNotFilledWarn;
        private System.Windows.Forms.Panel tradeCover;
        private System.Windows.Forms.NumericUpDown sendToAmount;
        private System.Windows.Forms.ToolTip compSliderTooltip;
        private System.Windows.Forms.ToolTip totalMinutesTooltip;
        private System.Windows.Forms.ToolTip zippedRustFolderTooltip;
        private System.Windows.Forms.ToolTip submitFileBtnTooltip;
        private System.Windows.Forms.Label pendingFunds;
        private System.Windows.Forms.TextBox walletAddr;
        private System.Windows.Forms.Button refreshButton;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.PictureBox QRCodeWallet;
        private System.Windows.Forms.Label label16;
    }
}

