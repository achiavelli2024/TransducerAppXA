using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Widget;
using Transducers; // PhoenixTransducer and related types
//using ITransducers; // delegate and DataResult definitions

namespace TransducerAppXA
{
    [Activity(Label = "TransducerAppXA", MainLauncher = true)]
    public class MainActivity : Activity
    {
        PhoenixTransducer Trans;

        // UI
        EditText txtIP;
        Button btnConnect;
        Button btnDisconnect;
        Button btnStartRead;
        Button btnStop;
        Button btnCopyLog;
        Button btnClearLog;

        EditText txtThresholdIniFree;
        EditText txtThresholdEndFree;
        EditText txtTimeoutFree;
        EditText txtNominalTorque;
        EditText txtMinimumTorque;
        EditText txtMaximoTorque;

        TextView tvTorque, tvAngle, tvResultsCounter, tvUntighteningsCounter, tvStatus;
        ScrollView scrollLogContainer;
        TextView tvLog;

        // internal counters
        int ResultsCounter = 0;
        int UntighteningsCounter = 0;

        List<object> lastTesteResult = new List<object>();
        CancellationTokenSource tailerCts;

        const int FIXED_PORT = 23;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);

            // Bind UI using the IDs that exist in layout (edtIP, btnConnect, etc.)
            txtIP = FindViewById<EditText>(Resource.Id.edtIP);
            btnConnect = FindViewById<Button>(Resource.Id.btnConnect);
            btnDisconnect = FindViewById<Button>(Resource.Id.btnDisconnect);
            btnStartRead = FindViewById<Button>(Resource.Id.btnStartRead);
            btnStop = FindViewById<Button>(Resource.Id.btnStop);
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

            scrollLogContainer = FindViewById<ScrollView>(Resource.Id.scrollLog);
            tvLog = FindViewById<TextView>(Resource.Id.tvLog);

            // defaults
            if (string.IsNullOrWhiteSpace(txtIP?.Text)) txtIP.Text = "192.168.4.1";
            if (txtThresholdIniFree != null) txtThresholdIniFree.Text = "1";
            if (txtThresholdEndFree != null) txtThresholdEndFree.Text = "1";
            if (txtTimeoutFree != null) txtTimeoutFree.Text = "500";
            if (txtNominalTorque != null) txtNominalTorque.Text = "4";
            if (txtMinimumTorque != null) txtMinimumTorque.Text = "1";
            if (txtMaximoTorque != null) txtMaximoTorque.Text = "10";

            // wire events
            if (btnConnect != null) btnConnect.Click += BtnConnect_Click;
            if (btnDisconnect != null) btnDisconnect.Click += BtnDisconnect_Click;
            if (btnStartRead != null) btnStartRead.Click += BtnStartRead_Click;
            if (btnStop != null) btnStop.Click += BtnStop_Click;
            if (btnCopyLog != null) btnCopyLog.Click += BtnCopyLog_Click;
            if (btnClearLog != null) btnClearLog.Click += BtnClearLog_Click;

            // subscribe to ProtocolFileLogger BEFORE doing test writes
            ProtocolFileLogger.OnLogWritten += ProtocolFileLogger_OnLogWritten;

            // best-effort subscribe to internal Transducer logger
            TrySubscribeTransducerLogger();

            AddLog("App initialized. Port fixed to 23; preserving transducer logic.");

            // quick test
            try
            {
                ProtocolFileLogger.WriteProtocol("SYS", "UI: ProtocolFileLogger test (startup)", null);
                var p = ProtocolFileLogger.FilePath;
                AddLog(!string.IsNullOrEmpty(p) ? $"ProtocolFileLogger.FilePath = {p}" : "ProtocolFileLogger.FilePath = (null)");
            }
            catch (Exception ex) { AddLog("ProtocolFileLogger test write failed: " + ex.Message); }

            // start tailer
            StartLogTailer();
        }

        // ProtocolFileLogger handler
        private void ProtocolFileLogger_OnLogWritten(string direction, string text, byte[] raw)
        {
            try
            {
                var sb = new StringBuilder();
                sb.AppendFormat("{0:yyyy-MM-dd HH:mm:ss.fff} [{1}] {2}", DateTime.Now, direction ?? "LOG", text ?? "");
                sb.AppendLine();
                if (raw != null && raw.Length > 0)
                {
                    sb.Append("HEX: ");
                    sb.Append(ProtocolFileLogger.ByteArrayToHexString(raw, " "));
                    sb.AppendLine();
                }
                sb.AppendLine(new string('-', 60));
                string formatted = sb.ToString();

                AppendToUI_Log(formatted);

                // detect bracketed telegram in text or decoded raw
                if (!string.IsNullOrEmpty(text))
                {
                    int a = text.IndexOf('[');
                    int b = text.LastIndexOf(']');
                    if (a >= 0 && b > a)
                    {
                        string telegram = text.Substring(a, b - a + 1);
                        AppendToUI_Log("TELEGRAMA: " + telegram + System.Environment.NewLine);
                    }
                }
                else if (raw != null && raw.Length > 0)
                {
                    string decoded = null;
                    try { decoded = Encoding.UTF8.GetString(raw); } catch { try { decoded = Encoding.ASCII.GetString(raw); } catch { decoded = null; } }
                    if (!string.IsNullOrEmpty(decoded))
                    {
                        int a = decoded.IndexOf('[');
                        int b = decoded.LastIndexOf(']');
                        if (a >= 0 && b > a)
                        {
                            string telegram = decoded.Substring(a, b - a + 1);
                            AppendToUI_Log("TELEGRAMA: " + telegram + System.Environment.NewLine);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                AppendToUI_Log("ProtocolFileLogger_OnLogWritten error: " + ex.Message + System.Environment.NewLine);
            }
        }

        void AppendToUI_Log(string text)
        {
            RunOnUiThread(() =>
            {
                try
                {
                    if (tvLog != null)
                    {
                        tvLog.Append(text);
                        scrollLogContainer?.Post(() =>
                        {
                            try { scrollLogContainer.ScrollTo(0, tvLog.Bottom); } catch { }
                        });
                    }
                }
                catch { }
            });
        }

        void AddLog(string s)
        {
            AppendToUI_Log($"{DateTime.Now:HH:mm:ss.fff} - {s}{System.Environment.NewLine}");
        }

        private void TrySubscribeTransducerLogger()
        {
            try
            {
                var type = Type.GetType("Transducer_Estudo.TransducerLogger, Transducer_Estudo") ?? Type.GetType("TransducerLogger");
                if (type != null)
                {
                    var ev = type.GetEvent("OnLogWritten", System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static);
                    if (ev != null)
                    {
                        Action<string> handler = (s) => AddLog("TransducerLogger: " + s);
                        ev.AddEventHandler(null, handler);
                        AddLog("Subscribed to TransducerLogger.OnLogWritten (reflection).");
                    }
                }
            }
            catch { }
        }

        void BtnCopyLog_Click(object sender, EventArgs e)
        {
            try
            {
                var text = tvLog?.Text ?? "";
                var clipboard = (ClipboardManager)GetSystemService(ClipboardService) as ClipboardManager;
                var clip = ClipData.NewPlainText("TransducerLog", text);
                clipboard.PrimaryClip = clip;
                Toast.MakeText(this, "Log copied to clipboard", ToastLength.Short).Show();
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: COPY LOG clicked", null);
            }
            catch (Exception ex) { AddLog("Copy log error: " + ex.Message); }
        }

        void BtnClearLog_Click(object sender, EventArgs e)
        {
            RunOnUiThread(() => { if (tvLog != null) tvLog.Text = ""; });
            ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: CLEAR LOG clicked", null);
            AddLog("Log cleared (UI)");
        }

        // Tailer
        void StartLogTailer()
        {
            StopLogTailer();
            tailerCts = new CancellationTokenSource();
            var ct = tailerCts.Token;

            Task.Run(async () =>
            {
                var candidates = new List<string>();
                try
                {
                    var appFiles = FilesDir?.AbsolutePath;
                    if (!string.IsNullOrEmpty(appFiles)) candidates.Add(Path.Combine(appFiles, "Logs"));

                    try
                    {
                        var ext = Android.OS.Environment.ExternalStorageDirectory?.AbsolutePath;
                        if (!string.IsNullOrEmpty(ext)) candidates.Add(Path.Combine(ext, "Logs"));
                    }
                    catch { }

                    if (!string.IsNullOrEmpty(appFiles)) candidates.Add(appFiles);

                    AddLog("LogTailer: scanning candidates: " + string.Join(" ; ", candidates));
                }
                catch (Exception ex) { AddLog("LogTailer init error: " + ex.Message); }

                string currentFile = null;
                FileStream fs = null;
                StreamReader sr = null;

                try
                {
                    while (!ct.IsCancellationRequested)
                    {
                        string newest = null;
                        DateTime newestTime = DateTime.MinValue;
                        foreach (var dir in candidates)
                        {
                            try
                            {
                                if (!Directory.Exists(dir)) continue;
                                var files = Directory.GetFiles(dir, "*.*", SearchOption.TopDirectoryOnly);
                                foreach (var f in files)
                                {
                                    var fi = new FileInfo(f);
                                    if (fi.LastWriteTimeUtc > newestTime)
                                    {
                                        newestTime = fi.LastWriteTimeUtc;
                                        newest = f;
                                    }
                                }
                            }
                            catch { }
                        }

                        if (newest == null)
                        {
                            await Task.Delay(800, ct).ConfigureAwait(false);
                            continue;
                        }

                        if (currentFile == null || !string.Equals(currentFile, newest, StringComparison.OrdinalIgnoreCase))
                        {
                            try { sr?.Dispose(); fs?.Dispose(); } catch { }
                            currentFile = newest;
                            AddLog("LogTailer: tailing file: " + currentFile);
                            try
                            {
                                fs = new FileStream(currentFile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                                sr = new StreamReader(fs, Encoding.UTF8);
                                fs.Seek(0, SeekOrigin.End);
                            }
                            catch (Exception ex)
                            {
                                AddLog("LogTailer open error: " + ex.Message);
                                await Task.Delay(1000, ct).ConfigureAwait(false);
                                continue;
                            }
                        }

                        bool had = false;
                        string line;
                        while ((line = await sr.ReadLineAsync().ConfigureAwait(false)) != null)
                        {
                            had = true;
                            AppendToUI_Log(line + System.Environment.NewLine);
                        }

                        if (!had) await Task.Delay(250, ct).ConfigureAwait(false);
                    }
                }
                catch (System.OperationCanceledException) { }
                catch (Exception ex) { AddLog("LogTailer error: " + ex.Message); }
                finally { try { sr?.Dispose(); fs?.Dispose(); } catch { } }
            }, ct);
        }

        void StopLogTailer()
        {
            try { tailerCts?.Cancel(); tailerCts = null; } catch { }
        }

        // Connection / control
        private void BtnConnect_Click(object sender, EventArgs e)
        {
            try
            {
                string ip = txtIP.Text?.Trim() ?? "";
                if (string.IsNullOrWhiteSpace(ip))
                {
                    Toast.MakeText(this, "Informe IP válido", ToastLength.Short).Show();
                    return;
                }

                AddLog($"UI: CONNECT -> {ip}:{FIXED_PORT}");
                ProtocolFileLogger.WriteProtocol("SYS", $"UI BUTTON: CONNECT {ip}:{FIXED_PORT}", null);

                if (Trans != null)
                {
                    AddLog("Stopping previous transducer instance");
                    try { Trans.StopReadData(); Trans.StopService(); } catch { }
                    Trans = null;
                }

                Trans = new PhoenixTransducer();
                Trans.bPrintCommToFile = true;

                // subscribe to events using ITransducers delegates
                Trans.DataResult += new DataResultReceiver(ResultReceiver);
                Trans.TesteResult += new DataTesteResultReceiver(TesteResultReceiver);
                Trans.DataInformation += new DataInformationReceiver(DataInformationReceiver);
                Trans.DebugInformation += new DebugInformationReceiver(DebugInformationReceiver);

                Trans.SetPerformance(ePCSpeed.Slow, eCharPoints.Many);
                Trans.Eth_IP = ip;
                Trans.Eth_Port = FIXED_PORT;

                AddLog("Starting service and communication");
                Task.Run(() =>
                {
                    try
                    {
                        Trans.StartService();
                        Thread.Sleep(50);
                        Trans.StartCommunication();
                        Trans.RequestInformation();
                        ProtocolFileLogger.WriteProtocol("SYS", "StartCommunication & RequestInformation invoked", null);
                    }
                    catch (Exception ex)
                    {
                        ProtocolFileLogger.WriteProtocol("SYS", "StartService error: " + ex.Message, null);
                    }
                });

                RunOnUiThread(() => tvStatus.Text = "Status: Connecting");
            }
            catch (Exception ex)
            {
                AddLog("BtnConnect_Click error: " + ex.Message);
                ProtocolFileLogger.WriteProtocol("SYS", "BtnConnect_Click error: " + ex.Message, null);
            }
        }

        private void BtnDisconnect_Click(object sender, EventArgs e)
        {
            try
            {
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: DISCONNECT clicked", null);
                Trans?.StopReadData();
                Trans?.StopService();
                Trans = null;
                RunOnUiThread(() => tvStatus.Text = "Status: Disconnected");
                AddLog("Disconnected");
            }
            catch (Exception ex) { AddLog("BtnDisconnect_Click error: " + ex.Message); }
        }

        private void BtnStartRead_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null) { AddLog("StartRead: Trans not connected"); return; }
                AddLog("UI: START READ");
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: START READ clicked", null);
                Trans.StartReadData();
                RunOnUiThread(() => tvStatus.Text = "Status: Waiting for acquisition");
            }
            catch (Exception ex) { AddLog("BtnStartRead_Click error: " + ex.Message); }
        }

        private void BtnStop_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null) return;
                AddLog("UI: STOP READ");
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: STOP clicked", null);
                Trans.StopReadData();
                RunOnUiThread(() => tvStatus.Text = "Status: Stopped");
            }
            catch (Exception ex) { AddLog("BtnStop_Click error: " + ex.Message); }
        }

        // Event handlers
        private void ResultReceiver(DataResult Data)
        {
            AddLog($"EVENT: ResultReceiver invoked - Type={Data?.Type} Torque={Data?.Torque} Angle={Data?.Angle}");
            RunOnUiThread(() =>
            {
                try
                {
                    if (tvTorque != null) tvTorque.Text = $"{Data.Torque} Nm";
                    if (tvAngle != null) tvAngle.Text = $"{Data.Angle} º";
                }
                catch { }
            });
        }

        private void TesteResultReceiver(List<DataResult> Result)
        {
            AddLog($"EVENT: TesteResultReceiver invoked - count {Result?.Count ?? 0}");
            if (Result == null || Result.Count == 0)
            {
                AddLog("TesteResultReceiver: empty list received");
                return;
            }

            var fr = Result.Find(x => x.Type == "FR");
            if (fr == null)
            {
                AddLog("TesteResultReceiver: FR not found -> treating as untighten");
                UntighteningsCounter++;
                RunOnUiThread(() => tvUntighteningsCounter.Text = UntighteningsCounter.ToString());
                _ = InitReadInternalAsync();
                return;
            }

            ResultsCounter++;
            RunOnUiThread(() =>
            {
                tvResultsCounter.Text = ResultsCounter.ToString();
                tvTorque.Text = $"{fr.Torque} Nm";
                tvAngle.Text = $"{fr.Angle} º";
                tvStatus.Text = "Test completed";
            });

            AddLog($"TesteResultReceiver: FR - Torque={fr.Torque} Angle={fr.Angle} (ResultsCounter={ResultsCounter})");

            lastTesteResult.Clear();
            foreach (var r in Result) lastTesteResult.Add(r);

            _ = InitReadInternalAsync();
        }

        private void DataInformationReceiver(DataInformation info)
        {
            AddLog($"EVENT: DataInformation received - HardID={info?.HardID} FullScale={info?.FullScale} TorqueLimit={info?.TorqueLimit}");
        }

        private void DebugInformationReceiver(DebugInformation dbg)
        {
            AddLog($"EVENT: DebugInformation - State={dbg?.State} Error={dbg?.Error}");
        }

        private async Task InitReadInternalAsync()
        {
            try
            {
                if (Trans == null) { AddLog("InitRead internal called but Trans null"); return; }

                try
                {
                    var frames = Trans.GetInitReadFrames();
                    foreach (var f in frames)
                    {
                        AddLog($"InitRead planned payload: {f.Item1}");
                        try { ProtocolFileLogger.WriteProtocol("TX (pre-CRC)", f.Item1, f.Item2); } catch { }
                    }
                }
                catch (Exception ex) { AddLog("InitRead: GetInitReadFrames failed: " + ex.Message); }

                AddLog("InitRead: SetZeroTorque");
                ProtocolFileLogger.WriteProtocol("SYS", "InitRead: SetZeroTorque", null);
                Trans.SetZeroTorque();
                await Task.Delay(10);

                AddLog("InitRead: SetZeroAngle");
                ProtocolFileLogger.WriteProtocol("SYS", "InitRead: SetZeroAngle", null);
                Trans.SetZeroAngle();
                await Task.Delay(10);

                AddLog("InitRead: SetTestParameter_ClickWrench(30,30,20)");
                ProtocolFileLogger.WriteProtocol("SYS", "InitRead: SetTestParameter_ClickWrench(30,30,20)", null);
                Trans.SetTestParameter_ClickWrench(30, 30, 20);
                await Task.Delay(10);

                decimal thresholdIni = 1M, thresholdEnd = 1M;
                int timeoutEnd = 500;
                decimal nominal = 4M, minT = 1M, maxT = 10M;
                decimal.TryParse(txtThresholdIniFree.Text?.Trim(), out thresholdIni);
                decimal.TryParse(txtThresholdEndFree.Text?.Trim(), out thresholdEnd);
                int.TryParse(txtTimeoutFree.Text?.Trim(), out timeoutEnd);
                decimal.TryParse(txtNominalTorque.Text?.Trim(), out nominal);
                decimal.TryParse(txtMinimumTorque.Text?.Trim(), out minT);
                decimal.TryParse(txtMaximoTorque.Text?.Trim(), out maxT);

                AddLog("InitRead: SetTestParameter (full)");
                ProtocolFileLogger.WriteProtocol("SYS", $"InitRead: SetTestParameter nominal={nominal} min={minT} max={maxT}", null);
                Trans.SetTestParameter(
                    new DataInformation(),
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
                ProtocolFileLogger.WriteProtocol("SYS", "InitRead: StartReadData", null);
                Trans.StartReadData();
                RunOnUiThread(() => tvStatus.Text = "Status: Acquisition started");
            }
            catch (Exception ex) { AddLog("InitReadInternal error: " + ex.Message); }
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            try
            {
                StopLogTailer();
                ProtocolFileLogger.OnLogWritten -= ProtocolFileLogger_OnLogWritten;
                if (Trans != null)
                {
                    try { Trans.StopReadData(); Trans.StopService(); } catch { }
                    Trans = null;
                }
            }
            catch { }
        }
    }
}