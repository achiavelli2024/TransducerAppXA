using Android.App;
using Android.OS;
using Android.Widget;
//using TransducerLib;
using System;
using System.Collections.Generic;
using Android.Util;
using Transducers;
using System.IO;
using System.Text;
using System.Diagnostics;



namespace TransducerAppXA.Android
{
    [Activity(Label = "TransducerAppXA", MainLauncher = true)]
    public class MainActivity : Activity
    {
        PhoenixTransducer trans;
        EditText edtIP;
        EditText edtIndex;
        Button btnConnectIP;
        Button btnDisconnect;
        Button btnReadData;
        Button btnStop;
        TextView tvTorque;
        TextView tvAngle;
        TextView tvStatus;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            string logDir = Path.Combine(System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal), "transducerapp", "logs");

            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);

            edtIP = FindViewById<EditText>(Resource.Id.edtIP);
            edtIndex = FindViewById<EditText>(Resource.Id.edtIndex);
            btnConnectIP = FindViewById<Button>(Resource.Id.btnConnectIP);
            btnDisconnect = FindViewById<Button>(Resource.Id.btnDisconnect);
            btnReadData = FindViewById<Button>(Resource.Id.btnReadData);
            btnStop = FindViewById<Button>(Resource.Id.btnStop);
            tvTorque = FindViewById<TextView>(Resource.Id.tvTorque);
            tvAngle = FindViewById<TextView>(Resource.Id.tvAngle);
            tvStatus = FindViewById<TextView>(Resource.Id.tvStatus);

            TransducerLogger.Configure("TransducerAppXA", true);
            trans = new PhoenixTransducer();
            trans.DataResult += ResultReceiver;
            trans.TesteResult += TesteResultReceiver;
            trans.DataInformation += DataInformationReceiver;

            btnConnectIP.Click += BtnConnectIP_Click;
            btnDisconnect.Click += BtnDisconnect_Click;
            btnReadData.Click += BtnReadData_Click;
            btnStop.Click += BtnStop_Click;
        }

        private void BtnConnectIP_Click(object sender, EventArgs e)
        {
            string ip = edtIP.Text.Trim();
            int index = 0;
            int.TryParse(edtIndex.Text, out index);

            tvStatus.Text = "Connecting...";
            TransducerLogger.LogFmt("MainActivity - connecting to {0}", ip);

            trans = new PhoenixTransducer();
            trans.DataResult += ResultReceiver;
            trans.TesteResult += TesteResultReceiver;
            trans.DataInformation += DataInformationReceiver;

            trans.SetPerformance(ePCSpeed.Slow, eCharPoints.Many);
            trans.Eth_IP = ip;
            trans.Eth_Port = 23;
            trans.PortIndex = index;

            // Start service and communication
            try
            {
                trans.StartService();
                trans.StartCommunication();
                trans.RequestInformation();
                tvStatus.Text = "Connected (requested info)";
            }
            catch (Exception ex)
            {
                tvStatus.Text = "Connect error: " + ex.Message;
                TransducerLogger.LogException(ex, "BtnConnectIP_Click");
            }
        }

        private void BtnDisconnect_Click(object sender, EventArgs e)
        {
            try
            {
                trans?.StopReadData();
                trans?.StopService();
                tvStatus.Text = "Disconnected";
            }
            catch (Exception ex)
            {
                TransducerLogger.LogException(ex, "BtnDisconnect_Click");
            }
        }

        private void BtnReadData_Click(object sender, EventArgs e)
        {
            tvStatus.Text = "Reading...";
            try
            {
                // Optionally log planned frames
                try
                {
                    var frames = trans.GetInitReadFrames();
                    foreach (var f in frames)
                    {
                        ProtocolFileLogger.WriteProtocol("TX (pre-CRC)", f.Item1, f.Item2);
                    }
                }
                catch { }

                // Execute InitRead steps: here we call SetZero/SetTestParameter/StartReadData
                trans.SetZeroTorque();
                System.Threading.Thread.Sleep(10);
                trans.SetZeroAngle();
                System.Threading.Thread.Sleep(10);
                trans.SetTestParameter_ClickWrench(30, 30, 20);
                trans.SetTestParameter(new DataInformation(), TesteType.TorqueOnly, ToolType.ToolType1, 4M,
                    Convert.ToDecimal(0.1M), Convert.ToDecimal(0.05M), 5000, 1, 500, eDirection.CW,
                    Convert.ToDecimal(8.1M), Convert.ToDecimal(7M), Convert.ToDecimal(10M),
                    100M, 10M, 300M, 50, 50);
                System.Threading.Thread.Sleep(100);
                trans.StartReadData();
            }
            catch (Exception ex)
            {
                TransducerLogger.LogException(ex, "BtnReadData_Click");
                tvStatus.Text = "Read error: " + ex.Message;
            }
        }

        private void BtnStop_Click(object sender, EventArgs e)
        {
            try
            {
                trans.StopReadData();
                tvStatus.Text = "Stopped read";
            }
            catch (Exception ex)
            {
                TransducerLogger.LogException(ex, "BtnStop_Click");
            }
        }

        private void DataInformationReceiver(DataInformation info)
        {
            RunOnUiThread(() =>
            {
                tvStatus.Text = "Device info received: " + (info?.HardID ?? "");
            });
        }

        private void ResultReceiver(DataResult dr)
        {
            RunOnUiThread(() =>
            {
                tvTorque.Text = $"Torque: {dr.Torque} Nm";
                tvAngle.Text = $"Angle: {dr.Angle} º";
            });
        }

        private void TesteResultReceiver(List<DataResult> lst)
        {
            if (lst == null) return;
            // Find final FR
            var fr = lst.Find(x => x.Type == "FR");
            RunOnUiThread(() =>
            {
                if (fr != null)
                {
                    tvTorque.Text = $"Final Torque: {fr.Torque} Nm";
                    tvAngle.Text = $"Final Angle: {fr.Angle} º";
                    tvStatus.Text = "Test completed";
                }
                else
                {
                    tvStatus.Text = $"TesteResult count: {lst.Count}";
                }
            });
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            try
            {
                trans?.StopReadData();
                trans?.StopService();
                trans?.Dispose();
            }
            catch { }
        }
    }
}