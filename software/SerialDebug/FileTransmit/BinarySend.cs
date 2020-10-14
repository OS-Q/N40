using System;
using System.Collections.Generic;
using System.Text;
using XMX.FileTransmit;
using System.Threading;


namespace XMX.FileTransmit
{
    public class BinarySend : IFileTramsmit, ITransmitUart
    {
        private bool IsStart = false;
        Thread SendThread;

        private int DelayTime = 10;
        public BinarySend(int delayTime)
        {
            DelayTime = delayTime;
        }


        private void SendThreadHandler()
        {
            while (IsStart)
            {
                if (SendNextPacket != null)
                {
                    SendNextPacket(this, null);
                    Thread.Sleep(DelayTime);
                }
            }
        }

        #region IFileTramsmit ��Ա

        public event EventHandler StartSend = null;

        public event EventHandler StartReceive = null;

        public event EventHandler SendNextPacket;

        public event EventHandler ReSendPacket = null;

        public event EventHandler AbortTransmit = null;

        public event EventHandler TransmitTimeOut = null;

        public event EventHandler EndOfTransmit = null;

        public event PacketEventHandler ReceivedPacket = null;

        public void SendPacket(PacketEventArgs packet)
        {
            if (SendToUartEvent != null)
            {
                SendToUartEvent(null, new SendToUartEventArgs(packet.Packet));
            }
        }

        public void Start()
        {
            IsStart = true;
            SendThread = new Thread(new ThreadStart(SendThreadHandler));
            SendThread.IsBackground = true;
            SendThread.Start();
        }

        public void Stop()
        {
            IsStart = false;
            if (EndOfTransmit!=null)
            {
                EndOfTransmit(this, null);
            }
        }

        public void Abort()
        {
            IsStart = false;
            if (AbortTransmit!=null)
            {
                AbortTransmit(this, null);
            }
        }

        #endregion

        #region ITransmitUart ��Ա

        public event SendToUartEventHandler SendToUartEvent;

        public void ReceivedFromUart(byte[] data)
        {
            Console.WriteLine("�����Ʒ������账�����");
        }

        #endregion


    }
}
