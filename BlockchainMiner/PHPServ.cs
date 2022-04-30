using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System;
using System.IO;
using System.Reflection;

public class PHPServ
{
    HttpListener HttpServ;
    string PhpCompilerPath = "./php/php.exe";

    private string ProcessPhpPage(string phpCompilerPath, string pageFileName, string getReq)
    {
        Process proc = new Process();
        proc.StartInfo.FileName = phpCompilerPath;
        proc.StartInfo.Arguments = "-f \"" + pageFileName + "\" " + getReq;
        Console.WriteLine(proc.StartInfo.Arguments);
        proc.StartInfo.CreateNoWindow = true;
        proc.StartInfo.UseShellExecute = false;
        proc.StartInfo.RedirectStandardOutput = true;
        proc.StartInfo.RedirectStandardError = true;
        proc.Start();
        string res = proc.StandardOutput.ReadToEnd();
        if (string.IsNullOrEmpty(res))
        {
            res = "<html><h2 style=\"color:red;\">Error!</h2><hr/> <h4>Error Details :</h4> <pre>No page output</pre></html>";
            proc.StandardError.Close();
        }
        if (res.StartsWith("\nParse error: syntax error"))
            res = "<html><h2 style=\"color:red;\">Error!</h2><hr/> <h4>Error Details :</h4> <pre>" + res + "</pre></html>";


        proc.StandardOutput.Close();

        proc.Close();
        return res;
    }

    public string Serve(string url)
    {
        Console.Write("Request: " + url + "\r\n");
        var page = AppDomain.CurrentDomain.BaseDirectory + @"wwwdata\" + url.Replace("/", @"\").Split('?')[0];
        Console.WriteLine("File Loc: " + page);
        //?query=getBalance&sendAmount=15&username=sam&password=gamecons12&fromAddress=dcc1f4e25f12ae7460b9f1430a5ed1858384368b70435855570cdacdc0338b708f4
        if (File.Exists(page))
        {
            string phpOut;
            var ext = new FileInfo(page);
            if (ext.Extension == ".php")
            {
                phpOut = ProcessPhpPage(PhpCompilerPath, page, url.Split('?')[1].Replace("&", " "));
                Console.Write(phpOut);
                Console.Write("Processing php page...\r\n");
                return phpOut;
            }
            else
            {
                phpOut = File.ReadAllText(page);
                return phpOut;
            }
        }
        else
        {
            return "ERR: error, page does not exist.";
        }
    }
}