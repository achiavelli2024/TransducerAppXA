using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Android.App;
using Android.Content;
using Android.Graphics;
using Android.OS;
using Android.Views;
using Android.Widget;
using Transducers;

namespace TransducerAppXA
{
    [Activity(Label = "TransducerAppXA", MainLauncher = true)]
    public class MainActivity : Activity
    {
        PhoenixTransducer Trans;

        // UI controls (ids must match activity_main.axml)
        EditText txtIP;
        EditText txtPort;
        EditText txtIndex;
        Button btnConnectIP;
        Button btnDisconnect;
        Button btnInitRead;   // Set PSet + Start
        Button btnStartRead;  // Start
        Button btnStopRead;   // Stop
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

        // Lists + adapters
        ListView lvLog;
        List<string> logItems = new List<string>();
        CustomStringAdapter logAdapter;

        ListView lvResults;
        List<string> resultItems = new List<string>();
        CustomStringAdapter resultAdapter;

        int ResultsCounter = 0;
        int UntighteningsCounter = 0;

        List<object> lastTesteResult = new List<object>();

        const int FIXED_PORT = 23;

        // Dedup state & tuning
        DateTime lastAddedResultTime = DateTime.MinValue;
        decimal lastAddedTorque = decimal.MinValue;
        decimal lastAddedAngle = decimal.MinValue;

        readonly TimeSpan dedupeTimeWindow = TimeSpan.FromSeconds(2);
        readonly decimal dedupeTorqueThreshold = 0.05M;
        readonly decimal dedupeAngleThreshold = 0.5M;

        readonly int rearmCooldownMs = 800;
        readonly SemaphoreSlim rearmLock = new SemaphoreSlim(1, 1);

        // Logging buffer
        readonly ConcurrentQueue<string> pendingLogs = new ConcurrentQueue<string>();
        volatile bool logFlushScheduled = false;
        readonly int LOG_FLUSH_MS = 250;
        Handler uiHandler;

        // Throttle toasts
        DateTime lastToastTime = DateTime.MinValue;
        readonly TimeSpan toastThrottle = TimeSpan.FromSeconds(1);

        // Limits
        const int MAX_LOG_ITEMS = 4000;
        const int MAX_RESULT_ITEMS = 2000;

        // Protocol logger reflection
        Delegate protocolHandlerDelegate = null;
        Type protocolLoggerType = null;

        // Error handling counters
        int er01Count = 0;
        int er02Count = 0;
        int er03Count = 0;
        int er04Count = 0;
        int er04Retries = 0;
        const int ER04_MAX_RETRIES = 5;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);

            uiHandler = new Handler(Looper.MainLooper);

            // Bind UI
            txtIP = FindViewById<EditText>(Resource.Id.txtIP);
            txtPort = FindViewById<EditText>(Resource.Id.txtPort);
            //txtIndex = FindViewById<EditText>(Resource.Id.txtIndex);
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
            logAdapter = new CustomStringAdapter(this, logItems, textColor: Color.ParseColor("#212121"), textSizeSp: 13);
            lvLog.Adapter = logAdapter;

            lvResults = FindViewById<ListView>(Resource.Id.lvResults);
            resultAdapter = new CustomStringAdapter(this, resultItems, textColor: Color.ParseColor("#212121"), textSizeSp: 14, bold: true);
            lvResults.Adapter = resultAdapter;

            // Defaults aligned to the Windows Form you provided
            if (string.IsNullOrWhiteSpace(txtIP?.Text)) txtIP.Text = "192.168.4.1";
            if (txtThresholdIniFree != null) txtThresholdIniFree.Text = "1";
            if (txtThresholdEndFree != null) txtThresholdEndFree.Text = "0.5";
            if (txtTimeoutFree != null) txtTimeoutFree.Text = "400";
            if (txtNominalTorque != null) txtNominalTorque.Text = "4";
            if (txtMinimumTorque != null) txtMinimumTorque.Text = "2";
            if (txtMaximoTorque != null) txtMaximoTorque.Text = "6";

            // Wire events (buttons with labels matching the Form)
            btnConnectIP.Click += BtnConnectIP_Click;
            btnDisconnect.Click += BtnDisconnect_Click;
            btnInitRead.Click += BtnInitRead_Click; // Set PSet + Start
            btnStartRead.Click += BtnStartRead_Click;
            btnStopRead.Click += BtnStopRead_Click;
            btnCopyLog.Click += BtnCopyLog_Click;
            btnClearLog.Click += BtnClearLog_Click;

            TrySubscribeProtocolLogger();

            QueueLog($"TEST LOG: UI initialized at {DateTime.Now:HH:mm:ss}");
            QueueResultItem($"TEST ITEM: Result #TEST {DateTime.Now:yyyy-MM-dd HH:mm:ss}  5.000 Nm  0.00 º");
            ScheduleLogFlush();
            QueueLog("App initialized (diagnostic entries added).");
        }

        // ---------- Layout / UI helpers and logging (unchanged) ----------
        void TrySubscribeProtocolLogger()
        {
            try
            {
                protocolLoggerType = Type.GetType("ProtocolFileLogger") ?? Type.GetType("Transducer_Estudo.ProtocolFileLogger, Transducer_Estudo") ?? Type.GetType("ProtocolFileLogger, Transducer_Estudo");
                if (protocolLoggerType != null)
                {
                    var ev = protocolLoggerType.GetEvent("OnLogWritten", System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static);
                    if (ev != null)
                    {
                        Action<string, string, byte[]> handler = ProtocolFileLogger_OnLogWritten;
                        protocolHandlerDelegate = handler;
                        ev.AddEventHandler(null, handler);
                        QueueLog("DIAG: Subscribed to ProtocolFileLogger.OnLogWritten (reflection).");
                        return;
                    }
                    else QueueLog("DIAG: ProtocolFileLogger found but OnLogWritten event not present.");
                }
                else QueueLog("DIAG: ProtocolFileLogger type not found (driver may not expose it).");
            }
            catch (Exception ex) { QueueLog("DIAG: TrySubscribeProtocolLogger error: " + ex.Message); }
        }

        void ProtocolFileLogger_OnLogWritten(string direction, string text, byte[] raw)
        {
            try
            {
                var sb = new StringBuilder();
                sb.AppendFormat("[{0}] {1}", direction ?? "LOG", text ?? "");
                if (raw != null && raw.Length > 0)
                {
                    sb.Append(" | HEX: ");
                    sb.Append(ByteArrayToHex(raw));
                }
                QueueLog("PROTO: " + sb.ToString());
            }
            catch (Exception ex) { QueueLog("PROTO HANDLER ERROR: " + ex.Message); }
        }

        static string ByteArrayToHex(byte[] bytes)
        {
            if (bytes == null) return null;
            var sb = new StringBuilder(bytes.Length * 3);
            foreach (var b in bytes) sb.AppendFormat("{0:X2} ", b);
            return sb.ToString().Trim();
        }

        void QueueLog(string s)
        {
            try
            {
                var line = $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - {s}";
                pendingLogs.Enqueue(line);
                if (pendingLogs.Count > 20000) pendingLogs.TryDequeue(out _);
                ScheduleLogFlush();
            }
            catch { }
        }

        void ScheduleLogFlush()
        {
            if (logFlushScheduled) return;
            logFlushScheduled = true;
            uiHandler.PostDelayed(new Java.Lang.Runnable(() =>
            {
                try { FlushPendingLogs(); } finally { logFlushScheduled = false; }
            }), LOG_FLUSH_MS);
        }

        void FlushPendingLogs()
        {
            int batch = 200;
            int processed = 0;
            while (processed < batch && pendingLogs.TryDequeue(out var line))
            {
                logItems.Insert(0, line);
                processed++;
            }

            if (logItems.Count > MAX_LOG_ITEMS) logItems.RemoveRange(MAX_LOG_ITEMS, logItems.Count - MAX_LOG_ITEMS);

            RunOnUiThread(() =>
            {
                try { logAdapter.NotifyDataSetChanged(); lvLog.InvalidateViews(); lvLog.RequestLayout(); } catch { }
            });

            if (!pendingLogs.IsEmpty) ScheduleLogFlush();
        }

        void QueueResultItem(string s)
        {
            RunOnUiThread(() =>
            {
                resultItems.Insert(0, s);
                if (resultItems.Count > MAX_RESULT_ITEMS) resultItems.RemoveRange(MAX_RESULT_ITEMS, resultItems.Count - MAX_RESULT_ITEMS);
                resultAdapter.NotifyDataSetChanged();
                lvResults.InvalidateViews();
                try { lvResults.SmoothScrollToPosition(0); } catch { }
            });
        }

        void AddLog(string s) => QueueLog(s);

        void AddResultHistory(int count, DateTime dt, decimal torque, decimal angle)
        {
            var line = $"{count}  {dt:yyyy-MM-dd HH:mm:ss}  {torque:F3} Nm  {angle:F2} º";
            QueueResultItem(line);
        }

        // ---------- Button handlers implementing sequence like the Form ----------
        private void BtnConnectIP_Click(object sender, EventArgs e)
        {
            try
            {
                string ip = txtIP.Text?.Trim() ?? "";
                if (string.IsNullOrEmpty(ip))
                {
                    QueueLog("Connect: IP empty");
                    if (DateTime.Now - lastToastTime > toastThrottle) { lastToastTime = DateTime.Now; Toast.MakeText(this, "Informe IP válido", ToastLength.Short).Show(); }
                    return;
                }

                QueueLog($"Connect -> {ip}:{FIXED_PORT}");

                if (Trans != null) { try { Trans.StopReadData(); Trans.StopService(); } catch { } Trans = null; }

                Trans = new PhoenixTransducer();
                Trans.bPrintCommToFile = true;

                // subscribe handlers
                Trans.DataResult += ResultReceiver;
                Trans.TesteResult += TesteResultReceiver;
                Trans.DataInformation += DataInformationReceiver;
                Trans.DebugInformation += DebugInformationReceiver;
                try { Trans.RaiseError += TransducerErrorReceiver; } catch { QueueLog("DIAG: RaiseError subscribe failed"); }

                Trans.SetPerformance(ePCSpeed.Medium, eCharPoints.Many);
                Trans.Eth_IP = ip;
                Trans.Eth_Port = FIXED_PORT;

                Task.Run(() =>
                {
                    try
                    {
                        Trans.StartService();
                        Thread.Sleep(50);
                        Trans.StartCommunication();
                        Trans.RequestInformation();
                        QueueLog("StartCommunication & RequestInformation invoked");
                    }
                    catch (Exception ex) { QueueLog("StartService error: " + ex.Message); }
                });

                RunOnUiThread(() => tvStatus.Text = "Status: Connecting");
            }
            catch (Exception ex) { QueueLog("BtnConnectIP_Click error: " + ex.Message); }
        }

        private void BtnDisconnect_Click(object sender, EventArgs e)
        {
            try
            {
                QueueLog("Disconnect requested");
                if (Trans != null)
                {
                    try { Trans.RaiseError -= TransducerErrorReceiver; } catch { }
                    var t = Trans;
                    Task.Run(() =>
                    {
                        try { t.StopReadData(); t.StopService(); } catch (Exception ex) { QueueLog("Stop error: " + ex.Message); }
                    });

                    try
                    {
                        Trans.DataResult -= ResultReceiver;
                        Trans.TesteResult -= TesteResultReceiver;
                        Trans.DataInformation -= DataInformationReceiver;
                        Trans.DebugInformation -= DebugInformationReceiver;
                    }
                    catch { }
                    Trans = null;
                }
                RunOnUiThread(() => tvStatus.Text = "Status: Disconnected");
            }
            catch (Exception ex) { QueueLog("BtnDisconnect_Click error: " + ex.Message); }
        }

        // "Set PSet + Start" button -> runs InitRead sequence (ZO, ZA, ClickWrench, SetTestParameter, StartReadData)
        private void BtnInitRead_Click(object sender, EventArgs e) => _ = InitReadAsync();

        private async Task InitReadAsync()
        {
            if (Trans == null) { QueueLog("InitRead: Trans not connected"); return; }

            QueueLog("InitRead: preparing (Set PSet + Start)");

            await Task.Run(() =>
            {
                try
                {
                    try
                    {
                        var frames = Trans.GetInitReadFrames();
                        if (frames != null)
                        {
                            foreach (var f in frames) QueueLog($"InitRead pre-CRC: {f.Item1}");
                        }
                    }
                    catch (Exception ex) { QueueLog("InitRead: failed to get init frames: " + ex.Message); }

                    QueueLog("InitRead: SetZeroTorque");
                    Trans.SetZeroTorque();
                    Thread.Sleep(10);

                    QueueLog("InitRead: SetZeroAngle");
                    Trans.SetZeroAngle();
                    Thread.Sleep(10);

                    QueueLog("InitRead: SetTestParameter_ClickWrench");
                    Trans.SetTestParameter_ClickWrench(30, 30, 20);
                    Thread.Sleep(10);

                    // read UI parameters snapshot
                    decimal thresholdIni = 1M, thresholdEnd = 0.5M, nominal = 4M, minT = 2M, maxT = 6M;
                    int timeoutEnd = 400;
                    RunOnUiThread(() =>
                    {
                        decimal.TryParse(txtThresholdIniFree.Text?.Trim(), out thresholdIni);
                        decimal.TryParse(txtThresholdEndFree.Text?.Trim(), out thresholdEnd);
                        int.TryParse(txtTimeoutFree.Text?.Trim(), out timeoutEnd);
                        decimal.TryParse(txtNominalTorque.Text?.Trim(), out nominal);
                        decimal.TryParse(txtMinimumTorque.Text?.Trim(), out minT);
                        decimal.TryParse(txtMaximoTorque.Text?.Trim(), out maxT);
                    });

                    QueueLog($"InitRead: SetTestParameter full (nominal={nominal} min={minT} max={maxT} thrIni={thresholdIni} thrEnd={thresholdEnd} timeout={timeoutEnd})");
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

                    Thread.Sleep(100);
                    QueueLog("InitRead: StartReadData");
                    Trans.StartReadData();
                }
                catch (Exception ex) { QueueLog("InitRead (background) error: " + ex.Message); }
            }).ConfigureAwait(false);

            RunOnUiThread(() => tvStatus.Text = "Status: Acquisition started");
        }

        private void BtnStartRead_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null) { QueueLog("StartRead: Trans not connected"); return; }
                QueueLog("StartRead: StartReadData called");
                Task.Run(() =>
                {
                    try { Trans.StartReadData(); QueueLog("StartReadData invoked"); } catch (Exception ex) { QueueLog("StartReadData error: " + ex.Message); }
                });
                RunOnUiThread(() => tvStatus.Text = "Status: Waiting for acquisition");
            }
            catch (Exception ex) { QueueLog("StartRead_Click error: " + ex.Message); }
        }

        private void BtnStopRead_Click(object sender, EventArgs e)
        {
            try
            {
                if (Trans == null) return;
                QueueLog("StopRead: StopReadData called");
                Task.Run(() =>
                {
                    try { Trans.StopReadData(); QueueLog("StopReadData invoked"); } catch (Exception ex) { QueueLog("StopReadData error: " + ex.Message); }
                });
                RunOnUiThread(() => tvStatus.Text = "Status: Stopped");
            }
            catch (Exception ex) { QueueLog("BtnStopRead_Click error: " + ex.Message); }
        }

        private void BtnCopyLog_Click(object sender, EventArgs e)
        {
            try
            {
                var sb = new StringBuilder();
                for (int i = logItems.Count - 1; i >= 0; i--) sb.AppendLine(logItems[i]);
                var cm = (ClipboardManager)GetSystemService(ClipboardService);
                var clip = ClipData.NewPlainText("TransducerLog", sb.ToString());
                cm.PrimaryClip = clip;
                if (DateTime.Now - lastToastTime > toastThrottle) { lastToastTime = DateTime.Now; Toast.MakeText(this, "Log copied to clipboard", ToastLength.Short).Show(); }
            }
            catch (Exception ex) { QueueLog("Copy log error: " + ex.Message); }
        }

        private void BtnClearLog_Click(object sender, EventArgs e)
        {
            RunOnUiThread(() => { logItems.Clear(); logAdapter.NotifyDataSetChanged(); lvLog.InvalidateViews(); });
            QueueLog("Log cleared (UI)");
        }

        // Handlers for results and data
        private void ResultReceiver(DataResult Data)
        {
            QueueLog($"ResultReceiver - Torque={Data?.Torque} Angle={Data?.Angle}");
            RunOnUiThread(() =>
            {
                try
                {
                    tvTorque.Text = $"{Data.Torque:F3} Nm";
                    tvAngle.Text = $"{Data.Angle:F2} º";
                }
                catch (Exception ex) { QueueLog("ResultReceiver UI update error: " + ex.Message); }
            });
        }

        private async void TesteResultReceiver(List<DataResult> Result)
        {
            try
            {
                QueueLog($"TesteResultReceiver - count: {Result?.Count ?? 0}");
                if (Result == null || Result.Count == 0) { QueueLog("TesteResultReceiver: empty list"); return; }

                var fr = Result.Find(x => x.Type == "FR");
                if (fr == null)
                {
                    QueueLog("TesteResultReceiver: FR not found -> untighten");
                    UntighteningsCounter++;
                    RunOnUiThread(() => tvUntighteningsCounter.Text = UntighteningsCounter.ToString());
                    await TryRearmAsync();
                    return;
                }

                var torqueVal = fr.Torque;
                var angleVal = fr.Angle;
                var now = DateTime.Now;

                bool isDuplicate = false;
                if (lastAddedResultTime != DateTime.MinValue)
                {
                    var dt = now - lastAddedResultTime;
                    var dTorque = Math.Abs(torqueVal - lastAddedTorque);
                    var dAngle = Math.Abs(angleVal - lastAddedAngle);
                    if (dt <= dedupeTimeWindow && dTorque <= dedupeTorqueThreshold && dAngle <= dedupeAngleThreshold) isDuplicate = true;
                }

                if (isDuplicate) { QueueLog($"Ignored duplicate FR"); await TryRearmAsync(); return; }

                ResultsCounter++;
                lastAddedResultTime = now;
                lastAddedTorque = torqueVal;
                lastAddedAngle = angleVal;

                RunOnUiThread(() =>
                {
                    tvResultsCounter.Text = ResultsCounter.ToString();
                    tvTorque.Text = $"{torqueVal:F3} Nm";
                    tvAngle.Text = $"{angleVal:F2} º";
                    tvStatus.Text = "Test completed";
                });

                QueueLog($"TesteResultReceiver: FR - Torque={torqueVal:F3} Angle={angleVal} (ResultsCounter={ResultsCounter})");
                AddResultHistory(ResultsCounter, now, torqueVal, angleVal);

                if (DateTime.Now - lastToastTime > toastThrottle)
                {
                    lastToastTime = DateTime.Now;
                    try { RunOnUiThread(() => Toast.MakeText(this, $"Result #{ResultsCounter}: {torqueVal:F3} Nm", ToastLength.Short).Show()); } catch { }
                }

                lastTesteResult.Clear();
                foreach (var r in Result) lastTesteResult.Add(r);

                await TryRearmAsync();
            }
            catch (Exception ex) { QueueLog("TesteResultReceiver error: " + ex.Message); }
        }

        // Error handler: subscribe to Trans.RaiseError
        private void TransducerErrorReceiver(int err)
        {
            QueueLog($"EVENT: RaiseError invoked - err={err}");
            Task.Run(() => HandleTransducerError(err));
        }

        private void HandleTransducerError(int err)
        {
            // same tratativa as earlier (ER01..ER04) - kept conservative
            try
            {
                switch (err)
                {
                    case 1:
                        er01Count++; QueueLog($"ERROR ER01: CRC invalid ({er01Count})"); RunOnUiThread(() => tvStatus.Text = "Error: ER01 CRC");
                        if (er01Count % 10 == 0)
                        {
                            Task.Run(() => { try { Trans?.StopReadData(); Thread.Sleep(50); Trans?.StopService(); Thread.Sleep(100); Trans?.StartService(); Thread.Sleep(50); Trans?.StartCommunication(); Trans?.RequestInformation(); QueueLog("ER01: restart executed"); } catch (Exception ex) { QueueLog("ER01 restart error: " + ex.Message); } });
                        }
                        break;
                    case 2:
                        er02Count++; QueueLog($"ERROR ER02: Syntax invalid ({er02Count})"); RunOnUiThread(() => { tvStatus.Text = "Error: ER02 Syntax"; if (DateTime.Now - lastToastTime > toastThrottle) { lastToastTime = DateTime.Now; Toast.MakeText(this, "ER02: Parâmetro inválido", ToastLength.Long).Show(); } });
                        Task.Run(() => { try { Trans?.StopReadData(); QueueLog("ER02: StopReadData called"); } catch (Exception ex) { QueueLog("ER02 StopReadData error: " + ex.Message); } });
                        break;
                    case 3:
                        er03Count++; QueueLog($"ERROR ER03: Invalid command ({er03Count})"); RunOnUiThread(() => { tvStatus.Text = "Error: ER03"; if (DateTime.Now - lastToastTime > toastThrottle) { lastToastTime = DateTime.Now; Toast.MakeText(this, "ER03: Comando inválido. Resync.", ToastLength.Long).Show(); } });
                        Task.Run(() => { try { Trans?.RequestInformation(); QueueLog("ER03: RequestInformation called"); } catch (Exception ex) { QueueLog("ER03 RequestInformation error: " + ex.Message); } });
                        if (er03Count >= 5) Task.Run(() => { try { Trans?.StopReadData(); } catch { } });
                        break;
                    case 4:
                        er04Count++; QueueLog($"ERROR ER04: Device not ready ({er04Count})"); RunOnUiThread(() => tvStatus.Text = "Error: ER04");
                        if (er04Retries < ER04_MAX_RETRIES)
                        {
                            er04Retries++; int delayMs = 1000 * er04Retries;
                            QueueLog($"ER04 retry #{er04Retries} in {delayMs}ms");
                            Task.Run(async () => { await Task.Delay(delayMs); try { Trans?.RequestInformation(); await InitReadAsync().ConfigureAwait(false); } catch (Exception ex) { QueueLog("ER04 retry error: " + ex.Message); } });
                        }
                        else
                        {
                            RunOnUiThread(() => { if (DateTime.Now - lastToastTime > toastThrottle) { lastToastTime = DateTime.Now; Toast.MakeText(this, "ER04: dispositivo não pronto. Verifique.", ToastLength.Long).Show(); } tvStatus.Text = "Error: ER04 - stopped"; });
                            Task.Run(() => { try { Trans?.StopReadData(); } catch (Exception ex) { QueueLog("ER04 StopReadData error: " + ex.Message); } });
                        }
                        break;
                    default:
                        QueueLog($"ERROR Unknown: code={err}");
                        RunOnUiThread(() => tvStatus.Text = $"Error: code={err}");
                        break;
                }
            }
            catch (Exception ex) { QueueLog("HandleTransducerError error: " + ex.Message); }
        }

        // Re-arm helper
        private async Task TryRearmAsync()
        {
            bool entered = await rearmLock.WaitAsync(0);
            if (!entered) return;
            try
            {
                await Task.Delay(rearmCooldownMs).ConfigureAwait(false);
                if (Trans == null) return;
                await InitReadAsync().ConfigureAwait(false);
            }
            finally { try { rearmLock.Release(); } catch { } }
        }

        private void DataInformationReceiver(DataInformation info) { QueueLog($"DataInformation - HardID={info?.HardID} FullScale={info?.FullScale} TorqueLimit={info?.TorqueLimit}"); }
        private void DebugInformationReceiver(DebugInformation dbg) { QueueLog($"DebugInformation - State={dbg?.State} Error={dbg?.Error}"); }

        protected override void OnDestroy()
        {
            base.OnDestroy();
            try
            {
                try { if (protocolLoggerType != null && protocolHandlerDelegate != null) { var ev = protocolLoggerType.GetEvent("OnLogWritten", System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.Static); if (ev != null) ev.RemoveEventHandler(null, protocolHandlerDelegate); } } catch { }
                if (Trans != null) { try { Trans.RaiseError -= TransducerErrorReceiver; Trans.StopReadData(); Trans.StopService(); } catch { } Trans = null; }
            }
            catch { }
        }

        // Custom adapter
        class CustomStringAdapter : BaseAdapter<string>
        {
            readonly Activity context;
            readonly IList<string> items;
            readonly Color textColor;
            readonly float textSizeSp;
            readonly bool bold;
            public CustomStringAdapter(Activity ctx, IList<string> items, Color textColor, float textSizeSp = 13f, bool bold = false)
            {
                this.context = ctx; this.items = items ?? new List<string>(); this.textColor = textColor; this.textSizeSp = textSizeSp; this.bold = bold;
            }
            public override string this[int position] => items[position];
            public override int Count => items.Count;
            public override long GetItemId(int position) => position;
            public override View GetView(int position, View convertView, ViewGroup parent)
            {
                TextView tv = convertView as TextView;
                if (tv == null)
                {
                    tv = new TextView(context);
                    int pad = (int)(8 * context.Resources.DisplayMetrics.Density);
                    tv.SetPadding(pad, pad / 2, pad, pad / 2);
                    tv.SetTextColor(textColor);
                    tv.SetTextSize(Android.Util.ComplexUnitType.Sp, textSizeSp);
                    if (bold) tv.SetTypeface(tv.Typeface, TypefaceStyle.Bold);
                    tv.SetSingleLine(false);
                }
                tv.Text = items[position];
                return tv;
            }
        }
    }
}