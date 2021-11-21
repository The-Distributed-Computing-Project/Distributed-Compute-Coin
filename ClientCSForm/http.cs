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
using System.Windows.Forms;
using NATUPNPLib;
using Mono.Nat;

public class http
{
    HttpListener HttpServ;
    string PhpCompilerPath = "./php/php.exe";
    string ListeningPort = "9090";

    public string ProcessPhpPage(string phpCompilerPath, string pageFileName, string getReq)
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

    public async void startHttpServ()
    {
        if (HttpServ == null)
            await StartServer();
        else if (!HttpServ.IsListening)
            await StartServer();
    }

    public async Task StartServer()
    {
        //NatUtility.DeviceFound += DeviceFound;
        //NatUtility.DeviceLost += DeviceLost;
        //NatUtility.StartDiscovery();



        OpenPort(9090, Protocol.Tcp);


        HttpServ = new HttpListener();
        HttpServ.Prefixes.Add("http://192.168.0.21:" + ListeningPort + "/");
        HttpServ.Start();
        Console.Write("Start Listining on Port :" + ListeningPort + "\r\n");
        Console.Write("Server is Running...\r\n\r\n");
        while (true)
        {
            try
            {
                var ctx = await HttpServ.GetContextAsync();


                Console.Write("Request: " + ctx.Request.Url.AbsoluteUri + "\r\n");
                var page = Application.StartupPath + @"\wwwdata" + ctx.Request.Url.LocalPath.Replace("/", @"\");
                //?query=getBalance&sendAmount=15&username=sam&password=gamecons12&fromAddress=dcc1f4e25f12ae7460b9f1430a5ed1858384368b70435855570cdacdc0338b708f4
                Console.Write("Page: " + page + "\r\n");
                if (File.Exists(page))
                {
                    string file;
                    var ext = new FileInfo(page);
                    if (ext.Extension == ".php")
                    {
                        file = ProcessPhpPage(PhpCompilerPath, page, ctx.Request.Url.AbsoluteUri.Split('?')[1].Replace("&", " "));
                        Console.Write("Processing php page...\r\n");
                    }
                    else
                    {
                        file = File.ReadAllText(page);
                    }

                    await ctx.Response.OutputStream.WriteAsync(ASCIIEncoding.UTF8.GetBytes(file), 0, file.Length);
                    ctx.Response.OutputStream.Close();
                    ctx.Response.Close();
                    Console.Write("Response: 200 OK\r\n\r\n");
                }
                else
                {
                    ctx.Response.StatusCode = 404;
                    var file = "<html><h2 style=\"color:red;\">404 File Not Found !!!</h2></html>";
                    await ctx.Response.OutputStream.WriteAsync(ASCIIEncoding.UTF8.GetBytes(file), 0, file.Length);
                    ctx.Response.OutputStream.Close();

                    ctx.Response.Close();
                    Console.Write("Response: 404 Not Found\r\n\r\n");

                }
            }
            catch (Exception ex)
            {
                Console.Write("\r\nException: Server Stopped:" + ex + "\r\n\r\n");
                break;
            }
        }


    }


    /// <summary>
    /// Opens a port with the UPnP protocol.
    /// </summary>
    /// <param name="PortNumber">The number of the port to add.</param>
    /// <param name="protocol">The protocol used for the port.</param>
    public static void OpenPort(int PortNumber, Protocol protocol)
    {
        NATUPNPLib.UPnPNAT upnpnat = new NATUPNPLib.UPnPNAT();

        NATUPNPLib.IStaticPortMappingCollection mappings = upnpnat.StaticPortMappingCollection;

        if (mappings != null)
        {
            mappings.Add(PortNumber, protocol.ToString(), PortNumber, GetLocalIPAddress(), true, "iKiwi");

            Console.WriteLine("Opened the port " + PortNumber + " with the UPnP protocol");
        }
    }

    public static string GetLocalIPAddress()
    {
        var host = Dns.GetHostEntry(Dns.GetHostName());
        foreach (var ip in host.AddressList)
        {
            if (ip.AddressFamily == AddressFamily.InterNetwork)
            {
                return ip.ToString();
            }
        }
        throw new Exception("No network adapters with an IPv4 address in the system!");
    }

    private void DeviceFound(object sender, DeviceEventArgs args)
    {
        INatDevice device = args.Device;
        device.CreatePortMap(new Mapping(Protocol.Tcp, 9090, 9090));
    }

    private void DeviceLost(object sender, DeviceEventArgs args)
    {
        INatDevice device = args.Device;
    }
}