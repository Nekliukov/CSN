using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;

namespace server
{
    class Program
    {
        static string remote_address;
        const int remotePort = 8001;
        const int localPort = 8002;
        //static string username;
        static bool flag;
        static void Main(string[] args)
        {
            remote_address = "127.0.0.1";
            //235.5.5.11
            Thread receiveThread = new Thread(new ThreadStart(ReceiveMessage));
            receiveThread.Start();
            SendMessage();
        }

        private static Int32 time32()
        {
            DateTime date = DateTime.Now;
            DateTime date2 = new DateTime(1970, 1, 1, 0, 0, 0);
            long temp = date.Ticks - date2.Ticks;
            TimeSpan temp2 = new TimeSpan(temp);
            Int32 temp3 = Convert.ToInt32(temp2.TotalSeconds);
            return temp3;
        }

        private static void SendMessage()
        {
            UdpClient sender = new UdpClient();
            try
            {
                while (true)
                {
                    if (flag)
                    {
                        Int32 message = time32();
                        string temp = message.ToString();
                        byte[] data = Encoding.Unicode.GetBytes(temp);
                        sender.Send(data, data.Length, remote_address, remotePort);
                        flag = false;
                    }
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
                    if (message == "time")
                        flag = true;
                    else
                        flag = false;
                    
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
