using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Widget;
using Transducers; // PhoenixTransducer and transducer types are in this namespace
using System.IO;

namespace TransducerAppXA
{
    [Activity(Label = "TransducerAppXA", MainLauncher = true)]
    public class MainActivity : Activity
    {
        PhoenixTransducer Trans;

        // UI
        EditText txtIP;
        EditText txtPort;
        EditText txtIndex;
        Button btnConnectIP;
        Button btnDisconnect;
        Button btnInitRead;
        Button btnStartRead;
        Button btnStopRead;
        Button btnCopyLog;
        Button btnClearLog;

        EditText txtThresholdIniFree;
        EditText txtThresholdEndFree;
        EditText txtTimeoutFree;
        EditText txtNominalTorque;
        EditText txtMinimumTorque;
        EditText txtMaximoTorque;

        TextView tvTorque;
        TextView tvAngle;
        TextView tvResultsCounter;
        TextView tvUntighteningsCounter;
        TextView tvStatus;

        ListView lvLog;
        List<string> logItems = new List<string>();
        ArrayAdapter<string> logAdapter;

        // Counters (UI)
        int ResultsCounter = 0;
        int UntighteningsCounter = 0;

        // last received TesteResult list (kept boxed)
        List<object> lastTesteResult = new List<object>();

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);

            // Bind UI
            txtIP = FindViewById<EditText>(Resource.Id.txtIP);
            txtPort = FindViewById<EditText>(Resource.Id.txtPort);
            txtIndex = FindViewById<EditText>(Resource.Id.txtIndex);
            btnConnectIP = FindViewById<Button>(Resource.Id.btnConnectIP);
            btnDisconnect = FindViewById<Button>(Resource.Id.btnDisconnect);
            btnInitRead = FindViewById<Button>(Resource.Id.btnInitRead);
            btnStartRead = FindViewById<Button>(Resource.Id.btnStartRead);
            btnStopRead = FindViewById<Button>(Resource.Id.btnStopRead);
            btnCopyLog = FindViewById<Button>(Resource.Id.btnCopyLog);
            btnClearLog = FindViewById<Button>(Resource.Id.btnClearLog);

            txtThresholdIniFree = FindViewById<EditText>(Resource.Id.txtThresholdIniFree);
            txtThresholdEndFree = FindViewById<EditText>(Resource.Id.txtThresholdEndFree);
            txtTimeoutFree = FindViewById<EditText>(Resource.Id.txtTimeoutFree);
            txtNominalTorque = FindViewById<EditText>(Resource.Id.txtNominalTorque);
            txtMinimumTorque = FindViewById<EditText>(Resource.Id.txtMinimumTorque);
            txtMaximoTorque = FindViewById<EditText>(Resource.Id.txtMaximoTorque);

            tvTorque = FindViewById<TextView>(Resource.Id.tvTorque);
            tvAngle = FindViewById<TextView>(Resource.Id.tvAngle);
            tvResultsCounter = FindViewById<TextView>(Resource.Id.tvResultsCounter);
            tvUntighteningsCounter = FindViewById<TextView>(Resource.Id.tvUntighteningsCounter);
            tvStatus = FindViewById<TextView>(Resource.Id.tvStatus);

            lvLog = FindViewById<ListView>(Resource.Id.lvLog);
            logAdapter = new ArrayAdapter<string>(this, Android.Resource.Layout.SimpleListItem1, logItems);
            lvLog.Adapter = logAdapter;

            // Defaults
            txtPort.Text = "23";
            txtIndex.Text = "0";
            txtThresholdIniFree.Text = "1";
            txtThresholdEndFree.Text = "1";
            txtTimeoutFree.Text = "500";
            txtNominalTorque.Text = "4";
            txtMinimumTorque.Text = "1";
            txtMaximoTorque.Text = "10";

            // Events
            btnConnectIP.Click += BtnConnectIP_Click;
            btnDisconnect.Click += BtnDisconnect_Click;
            btnInitRead.Click += BtnInitRead_Click;
            btnStartRead.Click += BtnStartRead_Click;
            btnStopRead.Click += BtnStopRead_Click;
            btnCopyLog.Click += BtnCopyLog_Click;
            btnClearLog.Click += BtnClearLog_Click;

            AddLog("App initialized");

            // Note: Trans is created on Connect (to mimic your Form flow)
        }

        // ---------- UI helpers ----------
        private void AddLog(string s)
        {
            RunOnUiThread(() =>
            {
                string line = $"{DateTime.Now:HH:mm:ss.fff} - {s}";
                logItems.Insert(0, line);
                if (logItems.Count > 1000) logItems.RemoveAt(logItems.Count - 1);
                logAdapter.NotifyDataSetChanged();
            });
        }

        private void AddLogFile(string tag, string message, byte[] raw = null)
        {
            try
            {
                // also write to ProtocolFileLogger if available in your project (same as Form did)
                try
                {
                    if (raw != null)
                        ProtocolFileLogger.WriteProtocol(tag, message, raw);
                    else
                        ProtocolFileLogger.WriteProtocol(tag, message, null);
                }
                catch { /* don't fail UI because of file logging */ }
            }
            catch { }
        }

        private void BtnCopyLog_Click(object sender, EventArgs e)
        {
            try
            {
                StringBuilder sb = new StringBuilder();
                for (int i = logItems.Count - 1; i >= 0; i--)
                    sb.AppendLine(logItems[i]);

                var clipboard = (Android.Text.ClipboardManager)GetSystemService(ClipboardService);
                clipboard.Text = sb.ToString();
                Toast.MakeText(this, "Log copied to clipboard", ToastLength.Short).Show();
            }
            catch (Exception ex)
            {
                AddLog("Copy log error: " + ex.Message);
            }
        }

        private void BtnClearLog_Click(object sender, EventArgs e)
        {
            RunOnUiThread(() =>
            {
                logItems.Clear();
                logAdapter.NotifyDataSetChanged();
            });
            AddLog("Log cleared (UI)");
        }

        // ---------- Connection flow ----------
        private void BtnConnectIP_Click(object sender, EventArgs e)
        {
            try
            {
                string ip = txtIP.Text?.Trim() ?? "";
                int port = 23;
                int idx = 0;
                int.TryParse(txtPort.Text?.Trim(), out port);
                int.TryParse(txtIndex.Text?.Trim(), out idx);

                AddLog($"btnConnectIP_Click - starting IP connection flow to {ip}:{port} index={idx}");

                if (Trans != null)
                {
                    AddLog("btnConnectIP_Click - stopping existing transducer");
                    try
                    {
                        Trans.StopReadData();
                        Trans.StopService();
                    }
                    catch { }
                }

                Trans = new PhoenixTransducer();

                // keep protocol file logging enabled
                Trans.bPrintCommToFile = true;

                // subscribe events (same delegates expected as in your Form)
                Trans.DataResult += new DataResultReceiver(ResultReceiver);
                Trans.TesteResult += new DataTesteResultReceiver(TesteResultReceiver);
                Trans.DataInformation += new DataInformationReceiver(DataInformationReceiver);
                Trans.DebugInformation += new DebugInformationReceiver(DebugInformationReceiver);

                Trans.SetPerformance(ePCSpeed.Slow, eCharPoints.Many);
                Trans.Eth_IP = ip;
                Trans.Eth_Port = port;
                Trans.PortIndex = idx;

                AddLog($"Starting service and communication (IP={ip} port={port})");
                Trans.StartService();
                Trans.StartCommunication();
                Trans.RequestInformation();

                // log to protocol file that UI triggered connect
                try { ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: btnConnectIP clicked by user", null); } catch { }

                tvStatus.Text = "Status: Connecting";
            }
            catch (Exception ex)
            {
                AddLog("btnConnectIP_Click error: " + ex.Message);
            }
        }

        private void BtnDisconnect_Click(object sender, EventArgs e)
        {
            try
            {
                AddLog("btnDisconnect_Click - stopping service and timer");
                if (Trans != null)
                {
                    Trans.StopReadData();
                    Trans.StopService();
                    // unsubscribe to avoid leaks
                    try
                    {
                        Trans.DataResult -= new DataResultReceiver(ResultReceiver);
                        Trans.TesteResult -= new DataTesteResultReceiver(TesteResultReceiver);
                        Trans.DataInformation -= new DataInformationReceiver(DataInformationReceiver);
                        Trans.DebugInformation -= new DebugInformationReceiver(DebugInformationReceiver);
                    }
                    catch { }
                    Trans = null;
                }
                tvStatus.Text = "Status: Disconnected";
                try { ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: btnDisconnect clicked by user", null); } catch { }
            }
            catch (Exception ex)
            {
                AddLog("btnDisconnect_Click error: " + ex.Message);
            }
        }

        // ---------- InitRead (replica seu Form) ----------
        private void BtnInitRead_Click(object sender, EventArgs e)
        {
            // run async to avoid blocking UI (your Form used Thread.Sleep; here we use Task.Delay)
            _ = InitReadAsync();
        }

        private async Task InitReadAsync()
        {
            try
            {
                if (Trans == null)
                {
                    AddLog("InitRead: Transducer not connected. Press Connect first.");
                    return;
                }

                AddLog("InitRead: logging planned InitRead frames (pre-CRC) to protocol file");
                try
                {
                    var frames = Trans.GetInitReadFrames();
                    foreach (var f in frames)
                    {
                        // f.Item1 = ascii payload, f.Item2 = bytes (UTF8) used to build CRC
                        AddLog($"InitRead: planned payload: {f.Item1}");
                        AddLogFile("TX (pre-CRC)", f.Item1, f.Item2);
                    }
                }
                catch (Exception ex)
                {
                    AddLog("InitRead: failed to get init frames: " + ex.Message);
                }

                AddLog("InitRead: SetZeroTorque");
                Trans.SetZeroTorque();
                await Task.Delay(10);

                AddLog("InitRead: SetZeroAngle");
                Trans.SetZeroAngle();
                await Task.Delay(10);

                AddLog("InitRead: SetTestParameter_ClickWrench(30,30,20)");
                Trans.SetTestParameter_ClickWrench(30, 30, 20);
                await Task.Delay(10);

                // read params from UI (safe parsing)
                decimal thresholdIni = 1M, thresholdEnd = 1M;
                int timeoutEnd = 500;
                decimal nominal = 4M, minT = 1M, maxT = 10M;

                decimal.TryParse(txtThresholdIniFree.Text?.Trim(), out thresholdIni);
                decimal.TryParse(txtThresholdEndFree.Text?.Trim(), out thresholdEnd);
                int.TryParse(txtTimeoutFree.Text?.Trim(), out timeoutEnd);
                decimal.TryParse(txtNominalTorque.Text?.Trim(), out nominal);
                decimal.TryParse(txtMinimumTorque.Text?.Trim(), out minT);
                decimal.TryParse(txtMaximoTorque.Text?.Trim(), out maxT);

                AddLog("InitRead: SetTestParameter (full) - sending acquisition configuration");
                Trans.SetTestParameter(
                    new DataInformation(), // device info may be updated by DI; Form used Datainfo from earlier; here we send default - DI will update conversion factors
                    TesteType.TorqueOnly,
                    ToolType.ToolType1,
                    nominal,
                    thresholdIni,
                    thresholdEnd,
                    timeoutEnd,
                    1,
                    500,
                    eDirection.CW,
                    nominal,
                    minT,
                    maxT,
                    100M,
                    10M,
                    300M,
                    50,
                    50);

                await Task.Delay(100);

                AddLog("InitRead: StartReadData");
                Trans.StartReadData();
                tvStatus.Text = "Status: Acquisition started";
                AddLog("InitRead: StartReadData called");
            }
            catch (Exception ex)
            {
                AddLog("InitRead error: " + ex.Message);
            }
        }

        // BtnStartRead - simply call StartReadData (like your Form)
        private void BtnStartRead_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null)
                {
                    AddLog("StartRead: Transducer not connected");
                    return;
                }
                AddLog("StartRead: calling StartReadData");
                Trans.StartReadData();
                tvStatus.Text = "Status: Waiting for acquisition";
            }
            catch (Exception ex)
            {
                AddLog("StartRead_Click error: " + ex.Message);
            }
        }

        private void BtnStopRead_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null) return;
                AddLog("StopRead: calling StopReadData");
                Trans.StopReadData();
                tvStatus.Text = "Status: Stopped";
            }
            catch (Exception ex)
            {
                AddLog("StopRead_Click error: " + ex.Message);
            }
        }

        // ---------- Event handlers - mirror your Form logic ----------
        // TesteResultReceiver (List<DataResult>)
        private void TesteResultReceiver(List<DataResult> Result)
        {
            AddLog($"TesteResultReceiver called - results count: {Result?.Count ?? 0}");
            // Find FR
            DataResult Data = Result?.Find(x => x.Type == "FR");
            if (Data == null)
            {
                AddLog("TesteResultReceiver: no FR found - increment untightenings and re-init read");
                UntighteningsCounter++;
                RunOnUiThread(() => tvUntighteningsCounter.Text = UntighteningsCounter.ToString());
                _ = InitReadAsync();
                return;
            }

            AddLog($"TesteResultReceiver: FR result - Torque={Data.Torque} Angle={Data.Angle}");
            ResultsCounter++;
            RunOnUiThread(() =>
            {
                tvResultsCounter.Text = ResultsCounter.ToString();
                tvTorque.Text = $"{Data.Torque} Nm";
                tvAngle.Text = $"{Data.Angle} º";
            });

            // Save last results for possible export or further inspection
            lastTesteResult.Clear();
            foreach (var r in Result)
                lastTesteResult.Add(r);

            // Re-arm for next test as in your Form
            _ = InitReadAsync();
        }

        // ResultReceiver (DataResult - each TQ)
        private void ResultReceiver(DataResult Data)
        {
            AddLog($"ResultReceiver called - Torque={Data.Torque} Angle={Data.Angle}");

            RunOnUiThread(() =>
            {
                tvTorque.Text = $"{Data.Torque} Nm";
                tvAngle.Text = $"{Data.Angle} º";
            });
        }

        // DataInformation handler (DI)
        private void DataInformationReceiver(DataInformation info)
        {
            AddLog($"DataInformation received - HardID={info.HardID} FullScale={info.FullScale} TorqueLimit={info.TorqueLimit}");
            // Optionally store device info for use in SetTestParameter (not required)
        }

        private void DebugInformationReceiver(DebugInformation dbg)
        {
            AddLog($"DebugInformation - State={dbg.State} Error={dbg.Error}");
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            try
            {
                if (Trans != null)
                {
                    Trans.StopReadData();
                    Trans.StopService();
                    Trans = null;
                }
            }
            catch { }
        }
    }
}