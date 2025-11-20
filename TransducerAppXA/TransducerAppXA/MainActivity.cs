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

namespace TransducerAppXA
{
    [Activity(Label = "TransducerAppXA", MainLauncher = true)]
    public class MainActivity : Activity
    {
        PhoenixTransducer Trans;

        // UI elements
        EditText txtIP;
        Button btnConnectIP;
        Button btnDisconnect;
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

        TextView tvTorque, tvAngle, tvResultsCounter, tvUntighteningsCounter, tvStatus;
        ListView lvLog;
        List<string> logItems = new List<string>();
        ArrayAdapter<string> logAdapter;

        CancellationTokenSource tailerCts;
        List<object> lastTesteResult = new List<object>();

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);

            // Bind UI (same IDs as you have in layout)
            txtIP = FindViewById<EditText>(Resource.Id.txtIP);
            btnConnectIP = FindViewById<Button>(Resource.Id.btnConnectIP);
            btnDisconnect = FindViewById<Button>(Resource.Id.btnDisconnect);
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

            // defaults
            txtThresholdIniFree.Text = "1";
            txtThresholdEndFree.Text = "1";
            txtTimeoutFree.Text = "500";
            txtNominalTorque.Text = "4";
            txtMinimumTorque.Text = "1";
            txtMaximoTorque.Text = "10";

            // wire events
            btnConnectIP.Click += BtnConnectIP_Click;
            btnDisconnect.Click += BtnDisconnect_Click;
            btnStartRead.Click += BtnStartRead_Click;
            btnStopRead.Click += BtnStopRead_Click;
            btnCopyLog.Click += BtnCopyLog_Click;
            btnClearLog.Click += BtnClearLog_Click;

            // subscribe ProtocolFileLogger to show file logs in UI
            ProtocolFileLogger.OnLogWritten += ProtocolFileLogger_OnLogWritten;

            // Try to subscribe TransducerLogger as well (best-effort)
            TrySubscribeTransducerLogger();

            AddLog("App initialized. Port fixed to 23; Index and INIT READ hidden from UI.");

            // START - TEST that ProtocolFileLogger is working: write a test entry and show FilePath
            try
            {
                ProtocolFileLogger.WriteProtocol("SYS", "UI: ProtocolFileLogger test (startup)", null);
                string fp = ProtocolFileLogger.FilePath;
                if (!string.IsNullOrEmpty(fp))
                {
                    AddLog($"ProtocolFileLogger.FilePath = {fp}");
                }
                else
                {
                    AddLog("ProtocolFileLogger.FilePath is null or not available (file writes will go to Debug output).");
                }
            }
            catch (Exception ex)
            {
                AddLog("ProtocolFileLogger test write failed: " + ex.Message);
            }
            // END TEST

            // start tailer to show protocol files (if exist)
            StartLogTailer();
        }

        // ---------- ProtocolFileLogger handler (consistent name) ----------
        private void ProtocolFileLogger_OnLogWritten(string direction, string text, byte[] raw)
        {
            try
            {
                // Build formatted "raw" text similar to file so UI matches file content
                var sb = new StringBuilder();
                sb.AppendFormat("{0:yyyy-MM-dd HH:mm:ss.fff} [{1}] {2}", DateTime.Now, direction ?? "LOG", text ?? "");
                sb.AppendLine();
                if (raw != null && raw.Length > 0)
                {
                    sb.Append("HEX: ");
                    sb.Append(ProtocolFileLogger.ByteArrayToHexString(raw, " "));
                    sb.AppendLine();
                }
                sb.AppendLine(new string('-', 50));
                string formatted = sb.ToString();

                // Always show formatted raw line first (so UI contains exactly what was written)
                AddLogRawToUI(formatted);

                // Try to extract telegram in brackets and show highlighted
                if (!string.IsNullOrEmpty(text))
                {
                    int a = text.IndexOf('[');
                    int b = text.LastIndexOf(']');
                    if (a >= 0 && b > a)
                    {
                        string telegram = text.Substring(a + 1, b - a - 1);
                        AddLogTelegram(direction ?? "LOG", telegram, text);
                        return;
                    }
                }

                if (raw != null && raw.Length > 0)
                {
                    string decoded = null;
                    try { decoded = Encoding.UTF8.GetString(raw); } catch { try { decoded = Encoding.ASCII.GetString(raw); } catch { decoded = null; } }
                    if (!string.IsNullOrEmpty(decoded))
                    {
                        int a = decoded.IndexOf('[');
                        int b = decoded.LastIndexOf(']');
                        if (a >= 0 && b > a)
                        {
                            string telegram = decoded.Substring(a + 1, b - a - 1);
                            AddLogTelegram(direction ?? "RAW", telegram, decoded);
                            return;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                AddLog("ProtocolFileLogger_OnLogWritten error: " + ex.Message);
            }
        }

        // Insert the formatted/raw line into the ListView adapter (UI)
        void AddLogRawToUI(string formattedLine)
        {
            RunOnUiThread(() =>
            {
                // Split into lines and insert at top preserving order
                var lines = formattedLine.Split(new[] { System.Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries);
                for (int i = lines.Length - 1; i >= 0; i--)
                    logItems.Insert(0, lines[i]);

                // Trim
                while (logItems.Count > 5000) logItems.RemoveAt(logItems.Count - 1);

                logAdapter.NotifyDataSetChanged();
                try { lvLog.InvalidateViews(); lvLog.SetSelection(0); } catch { }
            });
        }

        void AddLogTelegram(string direction, string telegram, string contextText = null)
        {
            RunOnUiThread(() =>
            {
                string header = $"{DateTime.Now:HH:mm:ss.fff} - {direction}: [{telegram}]";
                logItems.Insert(0, header);
                if (!string.IsNullOrEmpty(contextText))
                    logItems.Insert(0, $"    context: {contextText}");
                if (logItems.Count > 5000) logItems.RemoveAt(logItems.Count - 1);
                logAdapter.NotifyDataSetChanged();
                try { lvLog.InvalidateViews(); lvLog.SetSelection(0); } catch { }
            });
        }

        void AddLog(string s)
        {
            RunOnUiThread(() =>
            {
                string line = $"{DateTime.Now:HH:mm:ss.fff} - {s}";
                logItems.Insert(0, line);
                if (logItems.Count > 5000) logItems.RemoveAt(logItems.Count - 1);
                logAdapter.NotifyDataSetChanged();
                try { lvLog.InvalidateViews(); lvLog.SetSelection(0); } catch { }
            });
        }

        private void TrySubscribeTransducerLogger()
        {
            try
            {
                // best-effort reflection hook if TransducerLogger exposes a static event OnLogWritten(Action<string>)
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
            catch { /* best-effort */ }
        }

        void BtnCopyLog_Click(object sender, EventArgs e)
        {
            try
            {
                var sb = new StringBuilder();
                for (int i = logItems.Count - 1; i >= 0; i--) sb.AppendLine(logItems[i]);

                var clipboard = (ClipboardManager)GetSystemService(ClipboardService) as ClipboardManager;
                ClipData clip = ClipData.NewPlainText("TransducerLog", sb.ToString());
                clipboard.PrimaryClip = clip;

                Toast.MakeText(this, "Log copied to clipboard", ToastLength.Short).Show();
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: COPY LOG clicked", null);
            }
            catch (Exception ex) { AddLog("Copy log error: " + ex.Message); }
        }

        void BtnClearLog_Click(object sender, EventArgs e)
        {
            RunOnUiThread(() => { logItems.Clear(); logAdapter.NotifyDataSetChanged(); });
            ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: CLEAR LOG clicked", null);
            AddLog("Log cleared (UI)");
        }

        // Tailer: reads files from Logs/ and pushes lines into UI (keeps behaviour)
        void StartLogTailer()
        {
            StopLogTailer();
            tailerCts = new CancellationTokenSource();
            var ct = tailerCts.Token;

            Task.Run(async () =>
            {
                List<string> candidates = new List<string>();
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
                            await Task.Delay(800, ct).ContinueWith(t => { });
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
                                await Task.Delay(1000, ct).ContinueWith(t => { });
                                continue;
                            }
                        }

                        bool had = false;
                        string line;
                        while ((line = await sr.ReadLineAsync()) != null)
                        {
                            had = true;
                            int f = line.IndexOf('[');
                            int l = line.LastIndexOf(']');
                            if (f >= 0 && l > f)
                            {
                                string telegram = line.Substring(f + 1, l - f - 1);
                                string dir = "FILE";
                                if (line.IndexOf("TX", StringComparison.OrdinalIgnoreCase) >= 0) dir = "TX";
                                else if (line.IndexOf("RX", StringComparison.OrdinalIgnoreCase) >= 0) dir = "RX";

                                // Insert raw file line and highlighted telegram
                                AddLogRawToUI(line);
                                AddLogTelegram(dir, telegram, line);
                            }
                            else
                            {
                                AddLog("FILE: " + line);
                            }
                        }

                        if (!had) await Task.Delay(250, ct).ContinueWith(t => { });
                    }
                }
                catch (System.OperationCanceledException) { }
                catch (Exception ex) { AddLog("LogTailer error: " + ex.Message); }
                finally
                {
                    try { sr?.Dispose(); fs?.Dispose(); } catch { }
                }
            }, ct);
        }

        void StopLogTailer()
        {
            try { tailerCts?.Cancel(); tailerCts = null; } catch { }
        }

        // UI actions (logic unchanged)
        private void BtnConnectIP_Click(object sender, EventArgs e)
        {
            try
            {
                string ip = txtIP.Text?.Trim() ?? "";
                int port = 23; // fixed
                AddLog($"btnConnectIP_Click - connecting to {ip}:{port}");
                ProtocolFileLogger.WriteProtocol("SYS", $"UI BUTTON: CONNECT {ip}:{port}", null);

                if (Trans != null)
                {
                    AddLog("Stopping previous transducer");
                    try { Trans.StopReadData(); Trans.StopService(); } catch { }
                }

                Trans = new PhoenixTransducer();
                Trans.bPrintCommToFile = true;

                Trans.DataResult += new DataResultReceiver(ResultReceiver);
                Trans.TesteResult += new DataTesteResultReceiver(TesteResultReceiver);
                Trans.DataInformation += new DataInformationReceiver(DataInformationReceiver);
                Trans.DebugInformation += new DebugInformationReceiver(DebugInformationReceiver);

                Trans.SetPerformance(ePCSpeed.Slow, eCharPoints.Many);
                Trans.Eth_IP = ip;
                Trans.Eth_Port = port;

                AddLog("Starting service and communication");
                Trans.StartService();
                Trans.StartCommunication();
                Trans.RequestInformation();

                tvStatus.Text = "Status: Connecting";
            }
            catch (Exception ex) { AddLog("BtnConnectIP_Click error: " + ex.Message); ProtocolFileLogger.WriteProtocol("SYS", "btnConnectIP_Click error: " + ex.Message, null); }
        }

        private void BtnDisconnect_Click(object sender, EventArgs e)
        {
            try
            {
                AddLog("btnDisconnect_Click - stopping service");
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: DISCONNECT clicked", null);
                if (Trans != null)
                {
                    try { Trans.StopReadData(); Trans.StopService(); } catch { }
                    Trans = null;
                }
                tvStatus.Text = "Status: Disconnected";
            }
            catch (Exception ex) { AddLog("BtnDisconnect_Click error: " + ex.Message); }
        }

        private void BtnStartRead_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null) { AddLog("StartRead: Trans not connected"); return; }
                AddLog("BtnStartRead_Click - calling StartReadData");
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: START READ clicked", null);
                Trans.StartReadData();
                tvStatus.Text = "Status: Waiting for acquisition";
            }
            catch (Exception ex) { AddLog("BtnStartRead_Click error: " + ex.Message); }
        }

        private void BtnStopRead_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null) return;
                AddLog("BtnStopRead_Click - calling StopReadData");
                ProtocolFileLogger.WriteProtocol("SYS", "UI BUTTON: STOP clicked", null);
                Trans.StopReadData();
                tvStatus.Text = "Status: Stopped";
            }
            catch (Exception ex) { AddLog("BtnStopRead_Click error: " + ex.Message); }
        }

        // Event handlers (kept)
        private void TesteResultReceiver(List<DataResult> Result)
        {
            AddLog($"TesteResultReceiver called - count {Result?.Count ?? 0}");
            DataResult Data = Result?.Find(x => x.Type == "FR");
            if (Data == null)
            {
                AddLog("TesteResultReceiver: FR not found => increment untightenings and re-init");
                RunOnUiThread(() => tvUntighteningsCounter.Text = (int.Parse(tvUntighteningsCounter.Text) + 1).ToString());
                _ = InitReadInternalAsync();
                return;
            }

            AddLog($"TesteResultReceiver: FR - Torque={Data.Torque} Angle={Data.Angle}");
            RunOnUiThread(() =>
            {
                tvResultsCounter.Text = (int.Parse(tvResultsCounter.Text) + 1).ToString();
                tvTorque.Text = $"{Data.Torque} Nm";
                tvAngle.Text = $"{Data.Angle} º";
            });

            lastTesteResult.Clear();
            foreach (var r in Result) lastTesteResult.Add(r);

            _ = InitReadInternalAsync();
        }

        private void ResultReceiver(DataResult Data)
        {
            AddLog($"ResultReceiver called - Torque={Data.Torque} Angle={Data.Angle}");
            RunOnUiThread(() =>
            {
                tvTorque.Text = $"{Data.Torque} Nm";
                tvAngle.Text = $"{Data.Angle} º";
            });
        }

        private void DataInformationReceiver(DataInformation info)
        {
            AddLog($"DataInformation received - HardID={info.HardID} FullScale={info.FullScale} TorqueLimit={info.TorqueLimit}");
        }

        private void DebugInformationReceiver(DebugInformation dbg)
        {
            AddLog($"DebugInformation - State={dbg.State} Error={dbg.Error}");
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
                tvStatus.Text = "Status: Acquisition started";
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