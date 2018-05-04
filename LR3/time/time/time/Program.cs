using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace time
{
    class Program
    {
        static string remote_address;
        const int remotePort = 8002;
        const int localPort = 8001;
        static void Main(string[] args)
        {
            remote_address = "127.0.0.1";
            //235.5.5.11
            Thread receiveThread = new Thread(new ThreadStart(ReceiveMessage));
            receiveThread.Start();
            SendMessage();
        }

        private static void SendMessage()
        {
            UdpClient sender = new UdpClient();
            try
            {
                while (true)
                {
                    string message = "";
                    while (message!="time")
                        message = Console.ReadLine();
                    //message = "";
                    byte[] data = Encoding.Unicode.GetBytes(message);
                    sender.Send(data, data.Length,remote_address,remotePort);
                }
            }
            catch (Exception ex)
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
            IPEndPoint remoteIP = null;
            try
            {
                while (true)
                {
                    byte[] data = receiver.Receive(ref remoteIP);
                    string message = Encoding.Unicode.GetString(data);
                    Console.WriteLine(message);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
                receiver.Close();
            }
        }
    }
}
