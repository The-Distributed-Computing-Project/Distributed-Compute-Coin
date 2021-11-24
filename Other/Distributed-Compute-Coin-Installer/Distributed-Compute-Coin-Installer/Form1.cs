using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Distributed_Compute_Coin_Installer
{
    public partial class Form1 : Form
    {
        public static string installDirectory = "";

        public Form1()
        {
            InitializeComponent();
        }

        private void browseButton_Click(object sender, System.EventArgs e)
        {
            using (var fbd = new FolderBrowserDialog())
            {
                fbd.RootFolder = Environment.SpecialFolder.ProgramFiles;
                DialogResult result = fbd.ShowDialog();

                if (!string.IsNullOrWhiteSpace(fbd.SelectedPath))
                {
                    LocationBox.Text = FormatPath(fbd.SelectedPath, "DC-Coin");
                }
            }
        }

        private void CancelButton_Click(object sender, System.EventArgs e)
        {
            Application.Exit();
        }

        string FormatPath(string bigone, string addon)
        {
            if (!bigone.EndsWith(@"\" + addon + @"\"))
            {
                if (bigone.EndsWith(@"\"))
                    return bigone + addon + @"\";
                else
                    return bigone + @"\" + addon + @"\";
            }
            else
                return bigone;
        }
    }
}
