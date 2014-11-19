using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace PsDeskController
{
    class SerialCommunicator : IDisposable
    {
        private readonly string _heartbeatPhrase;
        private readonly SerialPort _port;

        public bool IsConnected { get; private set; }
        public Queue<string> ReceiveQueue { get; private set; }
        public DateTime LastHeartbeatAt { get; private set; }

        public static string[] AvailablePorts
        {
            get { return SerialPort.GetPortNames(); }
        }

        public SerialCommunicator(string heartbeatPhrase)
        {
            _heartbeatPhrase = heartbeatPhrase;
            _port = new SerialPort();
            var portThread = new Thread(PortThread) {IsBackground = true};
            portThread.Start();
            ReceiveQueue = new Queue<string>();
        }

        private void PortThread()
        {
            while (true)
            {
                if (_port.IsOpen)
                {
                    try
                    {
                        var receive = _port.ReadLine().Replace("\r", "").Replace("\n", "");
                        if (receive == _heartbeatPhrase)
                        {
                            LastHeartbeatAt = DateTime.Now;
                        }
                        else
                        {
                            ReceiveQueue.Enqueue(receive);
                        }
                    }
                    catch (Exception e)
                    {
                        Debug.WriteLine("PortThread exception: {0}", e);
                        Thread.Sleep(1);
                    }
                }
                else
                {
                    Thread.Sleep(1);
                }
            }
        }
        
        public bool AttemptConnect(string portName)
        {
            try
            {
                Disconnect();

                _port.PortName = portName;
                _port.BaudRate = 19200;
                _port.Parity = Parity.None;
                _port.DataBits = 8;
                _port.StopBits = StopBits.One;
                _port.Open();
                Thread.Sleep(2000);
                if (LastHeartbeatAt < DateTime.Now.AddSeconds(-2))
                {
                    throw new Exception("This is not a PsDesk.");
                }

                IsConnected = true;
                return true;
            }
            catch (Exception e)
            {
                Debug.WriteLine("AttemptConnect exception: {0}", e);
                return false;
            }
        }

        public void Disconnect()
        {
            if (_port.IsOpen)
            {
                _port.Close();
            }
            IsConnected = false;
            ReceiveQueue.Clear();
        }

        public void Send(string message)
        {
            _port.WriteLine(message);
        }

        public string Expect(string phrase, int timeout = 500)
        {
            while (ReceiveQueue.Count == 0)
            {
                Thread.Sleep(10);
            }

            var readline = ReceiveQueue.Dequeue();
            if (readline != phrase)
            {
                throw new Exception("Expected " + phrase + " but got " + readline + ".");
            }

            return readline;
        }

        public void Dispose()
        {
            IsConnected = false;
            _port.Dispose();
        }
    }
}
