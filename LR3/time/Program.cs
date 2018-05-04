using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace udp_chat
{
    class Program
    {
        static IPAddress remote_address;
        const int remotePort = 8001;
        const int localPort = 8002;
        static string username;
        static void Main(string[] args)
        {
            Console.WriteLine("Введите своё имя: ");
            username = Console.ReadLine();
            remote_address = IPAddress.Parse("235.5.5.11");
            Thread receiveThread = new Thread(new ThreadStart(ReceiveMessage));
            receiveThread.Start();
            SendMessage();
        }

        private static void SendMessage()
        {
            UdpClient sender = new UdpClient();
            IPEndPoint endPoint = new IPEndPoint(remote_address, remotePort);
            try
            {
                while (true)
                {
                    string message = Console.ReadLine();
                    message = string.Format("{0} : {1}",username,message);
                    byte[] data = Encoding.Unicode.GetBytes(message);
                    sender.Send(data,data.Length,endPoint);
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                sender.Close();
            }
        }

        private static void ReceiveMessage()
        {
            UdpClient receiver = new UdpClient(localPort);
            receiver.JoinMulticastGroup(remote_address, 50);
            IPEndPoint remoteIP = null;
            string localAddress = LocalIpAddress();
            try
            {
                while (true)
                {
                    byte[] data = receiver.Receive(ref remoteIP);
                    //if (remoteIP.Address.ToString().Equals(localAddress))
                    //    continue;
                    string message = Encoding.Unicode.GetString(data);
                    Console.WriteLine(message);
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                receiver.Close();
            }
        }

        private static string LocalIpAddress()
        {
            string localIP = "";
            IPHostEntry host = Dns.GetHostEntry(Dns.GetHostName());
            foreach (IPAddress ip in host.AddressList)
            {
                if (ip.AddressFamily == AddressFamily.InterNetwork)
                {
                    localIP = ip.ToString();
                    break;
                }
            }
            return localIP;

        }
    }

    
}
