using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace PsDeskController
{
    public class StatusUpdateEventArgs : EventArgs
    {
        public string NewStatus { get; private set; }
        public bool IsConnected { get; private set; }

        public StatusUpdateEventArgs(string newStatus, bool isConnected)
        {
            NewStatus = newStatus;
            IsConnected = isConnected;
        }
    }
    
    public class DeskController
    {
        public event EventHandler<StatusUpdateEventArgs> OnStatusUpdate;

        private readonly SerialCommunicator _communicator;

        public DeskController()
        {
            _communicator = new SerialCommunicator("PSDESK");

            var controllerThread = new Thread(ControllerThreadStart) {IsBackground = true};
            controllerThread.Start();

            var heartbeatThread = new Thread(HeartbeatThreadStart) {IsBackground = true};
            heartbeatThread.Start();
        }

        private void ControllerThreadStart()
        {
            var portIndex = -1;

            while (true)
            {
                if (!_communicator.IsConnected)
                {
                    var ports = SerialCommunicator.AvailablePorts;
                    if (ports.Length == 0)
                    {
                        Thread.Sleep(1000);
                        continue;
                    }

                    portIndex++;
                    if (portIndex < 0 || portIndex >= ports.Length) portIndex = 0;
                    var portToAttempt = ports[portIndex];

                    FireStatusUpdateEvent("Connecting (" + portToAttempt + ")...");
                    if (_communicator.AttemptConnect(portToAttempt))
                    {
                        FireStatusUpdateEvent("Connected on " + portToAttempt);
                    }
                    else
                    {
                        FireStatusUpdateEvent("Failed (" + portToAttempt + ").");
                        Thread.Sleep(1000);
                    }
                }

                Thread.Sleep(1000);
            }
        }

        private void FireStatusUpdateEvent(string newStatus)
        {
            if (OnStatusUpdate != null)
            {
                try
                {
                    OnStatusUpdate(this, new StatusUpdateEventArgs(newStatus, _communicator.IsConnected));
                }
                catch (Exception e)
                {
                    Debug.WriteLine(e);
                }
            }
        }

        private void HeartbeatThreadStart()
        {
            while (true)
            {
                if (_communicator.IsConnected && _communicator.LastHeartbeatAt < DateTime.Now.AddSeconds(-2))
                {
                    _communicator.Disconnect();
                }
                else
                {
                    Thread.Sleep(1000);
                }
            }
        }

        public void MoveUp()
        {
            _communicator.Send("M10000");
        }

        public void StopImmediately()
        {
            _communicator.Send("!");
        }

        public void MoveDown()
        {
            _communicator.Send("M0");
        }
    }
}
