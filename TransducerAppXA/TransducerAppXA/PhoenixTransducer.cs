#define DEF_DEBUG_MSGBOXES

//using ITransducers;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Reflection;
//using System.Windows.Forms;
using System.Linq;
using System.Net.Sockets;

// Logger namespace (assegure que TransducerLogger.cs esteja neste namespace)
//using Transducer_Estudo;
using TransducerAppXA;

namespace Transducers
{
    // PhoenixTransducer - versão instrumentada com TransducerLogger
    // - Mantive lógica original; adicionei logs não-invasivos em pontos estratégicos
    // - Comentários explicam cada inserção
    public class PhoenixTransducer : ITransducer
    {
        DebugInformation debug = new DebugInformation();

        public event EventReceiver RaiseEvent; ///ok
        public event ErrorReceiver RaiseError; ///ok
        public event DataInformationReceiver DataInformation; //não da para tirar
        public event DataResultReceiver DataResult; //Não da para tirar
        public event DataTesteResultReceiver TesteResult;
        public event DebugInformationReceiver DebugInformation; //ok
        public event CountersInformationReceiver CountersInformation;

        private const int BAUDRATE_USB = 115200;
        private const int BAUDRATE_BLUETOOTH = 38400;

        private System.Timers.Timer TimerItem = null;

        private string _PortName = String.Empty;
        private string _Eth_IP = String.Empty;
        private int _Eth_Port = 23;

        private int _PortIndex = 0;
        private bool _IsConnected = false;

        private bool simulateChartBlockFail = false;
        private int countSimulateGoodChartBlocksBeforeFail = 3;

        private int iConsecErrs = 0;
        private int iConsecTimeout_OR_InvalidAnswer = 0;
        private int iConsecErrsUnknown = 0;
        private enum eState
        {
            eIdle,
            eMustSendGetID,
            eWaitingID,
            eMustSendReadCommand,
            eWaitingAnswerReadCommand,
            eWaitBetweenReads,
            eMustSendRequestInformation,
            eWaitAnswerRequestInformation,
            eMustSendAquisitionConfig,
            eWaitingAquisitionConfig,
            eMustSendGetStatus,
            eWaitingGetStatus,
            eMustSendGetChartBlock,
            eWaitingChartBlock,
            eMustSendZeroTorque,
            eWaitingZeroTorque,
            eMustSendZeroAngle,
            eWaitingZeroAngle,
            eMustConfigure,
            eWaitingConfigure,
            eMustGetDeviceStatus,
            eWaitingDeviceStatus,
            eMustSendCalibrate,
            eWaitingCalibrate,
            eMustSendGetCounters,
            eWaitingCounters,

            eMustSendAquisitionClickWrenchConfig,
            eWaitingAquisitionClickWrenchConfig,
            eMustSendAquisitionAdditionalConfig,
            eWaitingAquisitionAdditionalConfig,
            eMustSendAquisitionAdditional2Config,
            eWaitingAquisitionAdditional2Config,
            eMustSendTorqueOffset,
            eWaitingTorqueOffset
        }
        private eState _state = eState.eIdle;
        private object locker_State = new object();
        private object locker_log = new object();
        private int TrySendCmd = 0;
        private int TickTXCommand = System.Environment.TickCount;
        private int TickRXCommand = System.Environment.TickCount;
        private string _id = "000000000000";

        private const int DEF_MAX_ERRS = 60;

        private int DEF_SLOW_TIMER_INTERVAL = 40;
        private const int DEF_FAST_TIMER_INTERVAL = 1;

        private const int TRIES_BAUD = 1;
        private const int DEF_TIMESPAN_TIMEOUT_ID = 500;
        private const int DEF_TIMESPAN_TIMEOUT_READ = 400;
        private int DEF_TIMESPAN_BETWEENREADS = 100;
        private int DEF_TIMESPAN_BETWEENREADS_TRACING = 100;
        private const int DEF_TIMESPAN_TIMEOUT_REQUESTINFORMATION = 500;
        private const int DEF_TIMESPAN_ABORTGARBAGE = 300;
        private const int DEF_TIMESPAN_AQUISITIONCONFIG = 200;
        private const int DEF_TIMESPAN_GETSTATUS = 1200;
        private const int DEF_TIMESPAN_WAITCHARTBLOCK = 500;
        private const int DEF_TIMESPAN_TIMEOUT_ZERO = 300;
        private const int DEF_TIMESPAN_TIMEOUT_CONFIGURATION = 500;
        private const int DEF_TIMESPAN_GETDEVICESTATUS = 1200;
        private const int DEF_TIMESPAN_TIMEOUT_DEVICESTATUS = 400;
        private const int DEF_TIMESPAN_TIMEOUT_CALIBRATE = 400;
        private const int DEF_TIMESPAN_TIMEOUT_COUNTERS = 300;


        private const int DEF_MIN_THRESHOLD = 0;
        private const int DEF_MAX_THRESHOLD = 0x7FFFFF;
        private const int DEF_MIN_THRESHOLDEND = 0;
        private const int DEF_MAX_THRESHOLDEND = 0x7FFFFF;
        private const int DEF_MIN_TIMEOUTEND_MS = 1;
        private const int DEF_MAX_TIMEOUTEND_MS = 10000;
        private const int DEF_MIN_TIMESTEP_MS = 1;
        private const int DEF_MAX_TIMESTEP_MS = 100;
        private const int DEF_MIN_FILTERFREQUENCY = 500;
        private const int DEF_MAX_FILTERFREQUENCY = 20000;
        private const int DEF_MAX_BLOCKSIZE = 100;
        private int DEF_MAX_GRAPHSIZE = 2000;


        private const int DEF_MIN_CLICKFALL = 1;
        private const int DEF_MAX_CLICKFALL = 99;
        private const int DEF_MIN_CLICKRISE = 1;
        private const int DEF_MAX_CLICKRISE = 150;
        private const int DEF_MIN_CLICKWIDTH_MS = 3;
        private const int DEF_MAX_CLICKWIDTH_MS = 250;


        private const int DEF_MIN_TORQUEOFFSET = 0;
        private const int DEF_MAX_TORQUEOFFSET = 0x7FFFFF;
        private const int DEF_MIN_TORQUETARGET = 0;
        private const int DEF_MAX_TORQUETARGET = 0x7FFFFF;
        private const int DEF_MIN_TORQUEMAX = 0;
        private const int DEF_MAX_TORQUEMAX = 0x7FFFFF;
        private const int DEF_MIN_TORQUEMIN = 0;
        private const int DEF_MAX_TORQUEMIN = 0x7FFFFF;
        private const int DEF_MIN_ANGLETARGET = 0;
        private const int DEF_MAX_ANGLETARGET = 0x7FFFFF;
        private const int DEF_MIN_ANGLEMAX = 0;
        private const int DEF_MAX_ANGLEMAX = 0x7FFFFF;
        private const int DEF_MIN_ANGLEMIN = 0;
        private const int DEF_MAX_ANGLEMIN = 0x7FFFFF;

        private const int DEF_MIN_DelayToDetectFirstPeak_ms = 0;
        private const int DEF_MAX_DelayToDetectFirstPeak_ms = 0x7FFF;

        private const int DEF_MIN_LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms = 0;
        private const int DEF_MAX_LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms = 0x7FFF;



        private const int DEF_NVRAM_MAX_PAGE_SIZE = 64;
        private const int DEF_NVRAM_MIN_PAGE_SIZE = 1;
        private const int DEF_NVRAM_MAX_ADDRESS = 0x1E0B;
        private const int DEF_NVRAM_MIN_ADDRESS = 0x0000;

        private const int DEF_CW = 0;
        private const int DEF_CCW = 1;

        private const string DEVTYPE_TORQUEANGLE_ACQ = "01";
        private const string DEVTYPE_TORQUE_ACQ = "00";


        private double TorqueConversionFactor = 1;
        private double AngleConversionFactor = 1;


        private const int DEF_COLUMN_STARTINDEX = 0;
        private const int DEF_COLUMN_SIZE = 1;



        private const int RastSize = 30;
        private string[] RastCommands = new string[RastSize];
        private int[] RastTX = new int[RastSize];
        private int[] RastRX = new int[RastSize];
        private int[] RastAdditional = new int[RastSize];
        private double[] RastAdditionalD = new double[RastSize];
        private string[] RastAdditionalS = new string[RastSize];
        private int countGraphsComplete = 0;


        private struct sFlags
        {
            public bool MustSendGetID;
            public bool MustSendRequestInformation;
            public bool MustSendReadData;
            public bool MustSendAquisitionConfig;
            public bool MustSendGetStatus;
            public bool MustSendGetChartBlock;
            public bool CaptureNewTightening;
            public bool MustSendZeroTorque;
            public bool MustSendZeroAngle;
            public bool MustConfigure;
            public bool NewConfiguragion;
            public bool MustCalibrate;
            public bool MustSendGetCounters;

            public string Configuration;
            public int BufferSize;
            public int tickDeviceStatus;
            public decimal Calibrate_AppliedTorque;
            public decimal Calibrate_CurrentTorque;
            public decimal Calibrate_AppliedAngle;
            public decimal Calibrate_CurrentAngle;

            public bool MustSendTorqueOffset;
            public bool MustSendAquisitionClickWrenchConfig;
            public bool MustSendAquisitionAdditionalConfig;
            public bool MustSendAquisitionAdditional2Config;
            public decimal TorqueOffset;

            public bool AvoidSendTorqueOffset;
            public bool AvoidSendAquisitionClickWrenchConfig;
            public bool AvoidSendAquisitionAdditionalConfig;
            public bool AvoidSendAquisitionAdditional2Config;
        }
        sFlags flags;


        private struct sAquisitionConfig
        {
            public ToolType ToolType;
            public decimal Threshold;
            public decimal ThresholdEnd;
            public int TimeoutEnd_ms;
            public int TimeStep_ms;
            public int FilterFrequency;
            public eDirection Dir;

            public decimal TorqueTarget;
            public decimal TorqueMin;
            public decimal TorqueMax;
            public decimal AngleTarget;
            public decimal AngleMin;
            public decimal AngleMax;
            public int DelayToDetectFirstPeak_ms;
            public int TimeToIgnoreNewPeak_AfterFinalThreshold_ms;
        }
        sAquisitionConfig AquisitionConfig;


        private struct sAquisitionClickWrenchConfig
        {
            public ushort FallPercentage;
            public ushort RisePercentage;
            public ushort MinTimeBetweenPulses_ms;
        }
        sAquisitionClickWrenchConfig AquisitionClickWrenchConfig;


        enum eAquisitionState
        {
            Idle = 0,
            Threshold,
            Timeout,
            Finished
        }


        struct sAquisitionStatus
        {
            public eAquisitionState state;
            public eDirection dir;
            public int size;
            public int indexini;
            public int indexend;
            public int peakindex;
            public int indextht;
            public decimal TorqueResult;
            public decimal AngleResult;
            public int ResultIndexInOutputSamplesArray;
        }
        private sAquisitionStatus _aquisitionstatus;

        struct sBlocks
        {
            public int[,] block;
            public int count;
            public int indextoask;
            public int step;
            public int samples;
        }
        sBlocks _blocks;
        int _awaitedsize = 0;
        public const bool DEF_IGNOREGRAPHCRC = true;

        private int ixHigherTQ;
        private decimal higherTQ;

        private int iChart = 0;


        struct eCheckTimeSpans
        {
            public int tick_tighteningfinished;
            public int tick_endcalcs;
            public int tick_tx;
            public int tick_datareceived;
            public int tick_initproc;
            public int tick_endproc;
            public int tick_endall;
        }
        eCheckTimeSpans ticks;

        bool waitans = false;
        private int TryCout = 0;
        private string LastPackage = "";

        //private SerialPort SerialPort;


        private TcpClient sck;
        NetworkStream sockStream = null;
        BinaryWriter swrite;
        private List<DataResult> TesteResultsList = new List<DataResult>();
        private decimal FatAD = 0;
        private decimal TolCel = 0;

        private const int DEF_MEASURE_TIMEOUT = 0;
        private const int DEF_MEASURE_OK = 1;
        private const int DEF_MEASURE_ERR = 2;
        private const int DEF_MEASURE_GARBAGE = 3;
        private const int DEF_MAX_SIGNALMEASURES = 50;
        private struct sSignalMeasure
        {
            public int iTickTX;
            public List<int> lmeasure;
            public List<int> lticktx;
            public int laststatetimeout;
            public int laststateerr;
        }
        sSignalMeasure signalmeasure;

        private bool _bUserStartService = false;
        private bool _bPortOpen = false;

        //System.IO.Ports.SerialDataReceivedEventHandler HandlerDataReceiver;
        public PhoenixTransducer()
        {
            _aquisitionstatus.state = eAquisitionState.Idle;

            //SerialPort = new SerialPort();

            sck = new TcpClient();

            //if (HandlerDataReceiver == null)
            //HandlerDataReceiver = new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort_DataReceived);

            SetTimer();


            //SerialPort.ReadTimeout = SerialPort.WriteTimeout = 7000;

            _IsConnected = false;
            flags.tickDeviceStatus = System.Environment.TickCount;

            signalmeasure.lmeasure = new List<int>();
            signalmeasure.lticktx = new List<int>();
        }
        private bool bStart = false;
        private void StartLog()
        {

            if (bStart)
                return;
            bStart = true;

            try
            {
                if (!Directory.Exists("Logs"))
                    Directory.CreateDirectory("Logs");
            }
            catch { }

            string slook = this._PortName;
            try
            {
                DirectoryInfo di = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory + "\\Logs");
                FileSystemInfo[] files = di.GetFileSystemInfos();

                files.Where(f => f.Name.StartsWith(slook))
                    .OrderByDescending(x => x.CreationTime)
                    .Skip(15)
                    .ToList()
                    .ForEach(x => x.Delete());
            }
            catch { }
        }
        private void Write2Log(string msg, bool force = false, bool append = true)
        {
            Debug.Print(msg);

            if (bPrintCommToFile || force)
            {
                try
                {
                    lock (locker_log)
                    {
                        StartLog();

                        string slook = this._PortName;
                        string sname = "Logs\\" + slook + "-" + DateTime.Now.ToString("yyyyMMdd") + ".dat";
                        using (StreamWriter writetext = new StreamWriter(sname, append))
                        {
                            writetext.WriteLine(msg + (force ? " " + System.Environment.TickCount : ""));
                            writetext.Flush();
                        }
                    }
                }
                catch { }
            }
        }

        private bool bUserStartService
        {
            get { return _bUserStartService; }
            set
            {
                if (_bUserStartService != value)
                {
                    _bUserStartService = value;

                    Write2Log("-- USER START SERVICE: " + value);
                    // LOG adicional
                    try { TransducerLogger.LogFmt("USER START SERVICE: {0}", value); } catch { }

                    // Also write system event to protocol log
                    try { ProtocolFileLogger.WriteProtocol("SYS", "-- USER START SERVICE: " + value, null); } catch { }



                }
            }
        }


        private void SetTimer(bool start = false)
        {
            lock (objtimerlock)
            {
                if (TimerItem == null)
                {
                    shutdown = false;
                    TimerItem = new System.Timers.Timer(100);
                    TimerItem.Elapsed += dispatcherTimer_Tick;
                    TimerItem.AutoReset = false;
                    TimerItem.Enabled = true;
                }
                if (start)
                    TimerItem.Start();
            }
        }
        private object objtimerlock = new object();
        private void DisposeTimer()
        {
            lock (objtimerlock)
            {
                if (TimerItem != null)
                {
                    TimerItem.Stop();
                    TimerItem.Enabled = false;
                    TimerItem.Elapsed -= dispatcherTimer_Tick;
                    TimerItem.Dispose();
                    TimerItem = null;
                }
            }
        }
        public static string makeCRC(string cmd)
        {
            char[] Res = new char[2];
            char[] CRC = new char[8];
            int i, j, k;
            char DoInvert;
            string BitString = "";

            for (i = 0; i < cmd.Length; ++i)
            {
                k = 128;
                int c = cmd.ToCharArray()[i];
                for (j = 0; j < 8; ++j)
                {
                    if ((c & k) == 0)
                        BitString += "0";
                    else
                        BitString += "1";
                    k /= 2;
                }
            }

            for (i = 0; i < 8; ++i)
                CRC[i] = (char)0;

            for (i = 0; i < BitString.Length; ++i)
            {
                if (BitString[i] == '1')
                    DoInvert = (char)(CRC[7] ^ 1);
                else
                    DoInvert = CRC[7];

                CRC[7] = CRC[6];
                CRC[6] = CRC[5];
                CRC[5] = (char)(CRC[4] ^ DoInvert);
                CRC[4] = CRC[3];
                CRC[3] = CRC[2];
                CRC[2] = (char)(CRC[1] ^ DoInvert);
                CRC[1] = CRC[0];
                CRC[0] = DoInvert;

            }

            Res[0] = (char)(CRC[4] + CRC[5] * 2 + CRC[6] * 4 + CRC[7] * 8 + '0');
            Res[1] = (char)(CRC[0] + CRC[1] * 2 + CRC[2] * 4 + CRC[3] * 8 + '0');
            if (Res[0] > '9')
                Res[0] += (char)('A' - '9' - 1);
            if (Res[1] > '9')
                Res[1] += (char)('A' - '9' - 1);

            return new string(Res);
        }



        public List<Tuple<string, byte[]>> GetInitReadFrames()
        {
            var list = new List<Tuple<string, byte[]>>();
            try
            {
                // 1) ZO (SetZeroTorque)
                string ztCmd = _id + "ZO" + "1" + "0";
                string ztFrame = "[" + ztCmd + makeCRC(ztCmd) + "]";
                list.Add(new Tuple<string, byte[]>(ztFrame, Encoding.UTF8.GetBytes(ztFrame)));

                // 2) ZO0 (SetZeroAngle)
                string zaCmd = _id + "ZO0" + "1";
                string zaFrame = "[" + zaCmd + makeCRC(zaCmd) + "]";
                list.Add(new Tuple<string, byte[]>(zaFrame, Encoding.UTF8.GetBytes(zaFrame)));

                // 3) CS (Click Wrench config)
                string csCmd = _id + "CS" +
                               ((int)LimitClickFall(AquisitionClickWrenchConfig.FallPercentage)).ToString("X2") +
                               ((int)LimitClickRise(AquisitionClickWrenchConfig.RisePercentage)).ToString("X2") +
                               ((int)LimitClickWidth_ms(AquisitionClickWrenchConfig.MinTimeBetweenPulses_ms)).ToString("X2");
                string csFrame = "[" + csCmd + makeCRC(csCmd) + "]";
                list.Add(new Tuple<string, byte[]>(csFrame, Encoding.UTF8.GetBytes(csFrame)));

                // 4) SA (Acquisition config)
                string sSA = _id + "SA" +
                    ((int)LimitThreshold(Nm2AD(AquisitionConfig.Threshold))).ToString("X8") +
                    ((int)LimitThresholdEnd(Nm2AD(AquisitionConfig.ThresholdEnd))).ToString("X8") +
                    ((int)LimitTimeoutEnd_ms(AquisitionConfig.TimeoutEnd_ms)).ToString("X4") +
                    ((int)LimitTimeStep_ms(AquisitionConfig.TimeStep_ms)).ToString("X4") +
                    ((int)LimitFilterFrequency(AquisitionConfig.FilterFrequency)).ToString("X4") +
                    ((int)AquisitionConfig.Dir).ToString("X2") +
                    ((int)AquisitionConfig.ToolType).ToString("X2");
                string saFrame = "[" + sSA + makeCRC(sSA) + "]";
                list.Add(new Tuple<string, byte[]>(saFrame, Encoding.UTF8.GetBytes(saFrame)));

                // 5) SB (optional)
                string sSB = _id + "SB" +
                    ((int)LimitTorqueTarget(Nm2AD(AquisitionConfig.TorqueTarget))).ToString("X8") +
                    ((int)LimitTorqueMax(Nm2AD(AquisitionConfig.TorqueMax))).ToString("X8") +
                    ((int)LimitTorqueMin(Nm2AD(AquisitionConfig.TorqueMin))).ToString("X8") +
                    ((int)LimitAngleTarget(ConvertAngleToBus(AquisitionConfig.AngleTarget))).ToString("X8") +
                    ((int)LimitAngleMax(ConvertAngleToBus(AquisitionConfig.AngleMax))).ToString("X8") +
                    ((int)LimitAngleMin(ConvertAngleToBus(AquisitionConfig.AngleMin))).ToString("X8") +
                    "00000000";
                string sbFrame = "[" + sSB + makeCRC(sSB) + "]";
                list.Add(new Tuple<string, byte[]>(sbFrame, Encoding.UTF8.GetBytes(sbFrame)));

                // 6) SC (optional)
                string sSC = _id + "SC" +
                    ((int)LimitDelayToDetectFirstPeak_ms(AquisitionConfig.DelayToDetectFirstPeak_ms)).ToString("X4") +
                    ((int)LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms(AquisitionConfig.TimeToIgnoreNewPeak_AfterFinalThreshold_ms)).ToString("X4") +
                    "000000000000000000000000000000000000000000000000";
                string scFrame = "[" + sSC + makeCRC(sSC) + "]";
                list.Add(new Tuple<string, byte[]>(scFrame, Encoding.UTF8.GetBytes(scFrame)));
            }
            catch (Exception ex)
            {
                // Apenas loga falhas na montagem; não altera estado
                try { TransducerLogger.LogException(ex, "GetInitReadFrames failed"); } catch { }
            }
            return list;
        }









        public void RequestInformation()
        {
            try
            {
                flags.MustSendRequestInformation = true;
                SetTimer(true);

                TryCout++;
                TransducerLogger.Log("RequestInformation called - MustSendRequestInformation set true");
            }
            catch (Exception err)
            {

                TransducerLogger.LogException(err, "RequestInformation exception");
                throw err;
            }
        }
        private object objconfig = new object();



        public void WriteSetup(DataInformation Info)
        {
            lock (objconfig)
            {
                if (Info.FullScale > 0)
                    this.FatAD = (5M / Info.TorqueLimit * Info.FullScale) * 2;

                if (Info.TorqueLimit > 0)
                    this.TolCel = Info.TorqueLimit * 0.01M;

                StringBuilder Line = new StringBuilder();
                try
                {
                    flags.Configuration = Line.ToString() + '\r';
                    flags.MustConfigure = true;
                    flags.NewConfiguragion = true;

                    TransducerLogger.Log("WriteSetup: flags.MustConfigure set true");
                }
                catch (Exception err)
                {
                    TransducerLogger.LogException(err, "WriteSetup exception");
                    throw err;
                }
            }

        }
        private int GetSpanBetweenReads()
        {
            switch (_aquisitionstatus.state)
            {
                case (eAquisitionState.Threshold):
                case (eAquisitionState.Timeout):
                    return DEF_TIMESPAN_BETWEENREADS_TRACING;

                case (eAquisitionState.Idle):
                case (eAquisitionState.Finished):
                default:
                    return DEF_TIMESPAN_BETWEENREADS;
            }
        }
        private int GetbatteryLevel()
        {
            int level = 0;
            try
            {
                const int BAT_FAULT_CHARGE = 0x00;
                const int BAT_CHARGING = 0x01;
                const int BAT_COMPLETE_CHARGE = 0x02;
                const int BAT_NOT_DETECTED = 0x03;
                const int BAT_NORMAL_LEVEL = 0x04;
                const int BAT_LOW_LEVEL = 0x05;
                const int BAT_CRITICAL_LEVEL = 0x06;

                switch (debug.PowerState)
                {
                    case BAT_COMPLETE_CHARGE:
                        level = 100;
                        break;
                    case BAT_NORMAL_LEVEL:
                        level = 60;
                        break;
                    case BAT_LOW_LEVEL:
                        level = 30;
                        break;
                    case BAT_FAULT_CHARGE:
                    case BAT_CHARGING:
                    case BAT_NOT_DETECTED:
                    case BAT_CRITICAL_LEVEL:
                        level = 15;
                        break;
                }
            }
            catch { }
            return level;
        }
        private bool GetbatteryCharging()
        {
            return (debug.PowerSource == 0);
        }
        private int GetInterface()
        {
            if (signalmeasure.lmeasure.Count > 10)
            {
                if (debug.Type == 0 && debug.Interface == 1)
                    return 0;
                return debug.Interface;
            }
            else
                return -1;
        }
        private bool GetValidBatteryInfo()
        {
            return true;
        }
        public bool GetMeasures(out int ptim, out int pok, out int perr, out int pgarb, out int iansavg, out bool validbateryinfo, out int batterylevel, out bool charging, out int Interface, out int laststatetimeout, out int laststateerr)
        {
            bool bok = false;

            ptim = 0;
            pok = 0;
            perr = 0;
            pgarb = 0;
            iansavg = 0;

            validbateryinfo = GetValidBatteryInfo();
            charging = GetbatteryCharging();
            batterylevel = GetbatteryLevel();
            Interface = GetInterface();
            laststatetimeout = signalmeasure.laststatetimeout;
            laststateerr = signalmeasure.laststateerr;
            try
            {

                lock (signalmeasure.lmeasure)
                {
                    while (signalmeasure.lmeasure.Count > DEF_MAX_SIGNALMEASURES)
                        signalmeasure.lmeasure.RemoveAt(0);
                    while (signalmeasure.lticktx.Count > 10)
                        signalmeasure.lticktx.RemoveAt(0);

                    if (signalmeasure.lmeasure.Count > 0)
                    {
                        int itim = signalmeasure.lmeasure.Where(x => x == DEF_MEASURE_TIMEOUT).Count();
                        int iok = signalmeasure.lmeasure.Where(x => x == DEF_MEASURE_OK).Count();
                        int ierr = signalmeasure.lmeasure.Where(x => x == DEF_MEASURE_ERR).Count();
                        int igarb = signalmeasure.lmeasure.Where(x => x == DEF_MEASURE_GARBAGE).Count();

                        int isum = itim + iok + ierr;
                        float ffac = 100.0F / (float)isum;
                        ptim = (int)Math.Round(itim * ffac, 0);
                        pok = (int)Math.Round(iok * ffac, 0);
                        perr = (int)Math.Round(ierr * ffac, 0);
                        pgarb = (int)Math.Round(igarb * ffac, 0);

                        iansavg = (int)(signalmeasure.lticktx.Count > 0 ? signalmeasure.lticktx.Average() : double.NaN);
                        bok = true;
                    }
                }

            }
            catch
            {

            }
            return bok;
        }
        private string slogprev = null;
        private void dispatcherTimer_Tick(object sender, System.Timers.ElapsedEventArgs e)
        {
            try
            {
                // Log rápido do timer para acompanhar estado periódico
                TransducerLogger.LogFmt("TimerTick entry: state={0} tick_tx={1} tick_datareceived={2}", _state, ticks.tick_tx, ticks.tick_datareceived);

                lock (signalmeasure.lmeasure)
                {
                    try
                    {
                        while (signalmeasure.lmeasure.Count > DEF_MAX_SIGNALMEASURES)
                            signalmeasure.lmeasure.RemoveAt(0);
                        while (signalmeasure.lticktx.Count > 10)
                            signalmeasure.lticktx.RemoveAt(0);
                    }
                    catch { }
                }

                TimerItem.Enabled = false;
                lock (locker_State)
                {


                    if (bPrintCommToFile)
                    {
                        try
                        {
                            string slog = "f:"
                                        + (flags.MustSendGetID ? "MSID," : "")
                                        + (flags.MustConfigure ? "MConf," : "")
                                        + (flags.MustSendZeroTorque ? "MSZT," : "")
                                        + (flags.MustSendZeroAngle ? "MSZA," : "")
                                        + (flags.MustSendTorqueOffset ? "MSTO," : "")
                                        + (flags.MustCalibrate ? "MCal," : "")
                                        + (flags.MustSendTorqueOffset ? "MSTO," : "")
                                        + (flags.MustSendRequestInformation ? "MSRI," : "")
                                        + (flags.MustSendGetChartBlock ? "MSGCB," : "")
                                        + (flags.MustSendAquisitionConfig ? "MSAC," : "")
                                        + (flags.MustSendAquisitionClickWrenchConfig ? "MSACWC," : "")
                                        + (flags.MustSendAquisitionAdditionalConfig ? "MSAAC," : "")
                                        + (flags.MustSendAquisitionAdditional2Config ? "MSAA2C," : "")
                                        + (flags.MustSendGetStatus ? "MSGS," : "")
                                        + (flags.MustSendGetCounters ? "MSGC," : "")
                                        + (flags.MustSendReadData ? "MSRD," : "")
                                        ;
                            if (slog != slogprev)
                            {
                                Write2Log(slog);
                                slogprev = slog;
                                TransducerLogger.LogFmt("Flags snapshot: {0}", slog);
                            }
                        }
                        catch { }
                    }

                    lock (objconfig)
                    {
                        if (_Eth_IP != string.Empty && _Eth_IP != null)
                        {
                            commandtest = "";
                            serialPort_DataReceived(null);
                        }




                        if (_IsConnected)
                        {
                            if (flags.MustSendGetID || (_id == "000000000000"))
                            {
                                if (_state != eState.eWaitingID)
                                    SetState(eState.eMustSendGetID);
                            }
                            else if (flags.MustConfigure)
                            {
                                if (flags.Configuration != "")
                                {
                                    if (_state != eState.eWaitingConfigure)
                                        SetState(eState.eMustConfigure);
                                }
                                else
                                {
                                    flags.MustConfigure = false;
                                }
                            }
                            else if (flags.MustSendZeroTorque)
                            {
                                if (_state != eState.eWaitingZeroTorque)
                                    SetState(eState.eMustSendZeroTorque);
                            }
                            else if (flags.MustSendZeroAngle)
                            {
                                if (_state != eState.eWaitingZeroAngle)
                                    SetState(eState.eMustSendZeroAngle);
                            }
                            else if (flags.MustSendTorqueOffset)
                            {
                                if (flags.AvoidSendTorqueOffset)
                                    flags.MustSendTorqueOffset = false;
                                else if (_state != eState.eWaitingTorqueOffset)
                                    SetState(eState.eMustSendTorqueOffset);
                            }
                            else if (flags.MustCalibrate)
                            {
                                if (_state != eState.eWaitingCalibrate)
                                    SetState(eState.eMustSendCalibrate);
                            }
                            else if (flags.MustSendRequestInformation)
                            {
                                if (_state != eState.eWaitAnswerRequestInformation)
                                    SetState(eState.eMustSendRequestInformation);
                            }
                            else if (flags.MustSendGetChartBlock)
                            {
                                if (_state != eState.eWaitingChartBlock)
                                    SetState(eState.eMustSendGetChartBlock);
                            }
                            else if (flags.MustSendAquisitionConfig)
                            {
                                if (_state != eState.eWaitingAquisitionConfig)
                                    SetState(eState.eMustSendAquisitionConfig);
                            }
                            else if (flags.MustSendAquisitionClickWrenchConfig)
                            {
                                if (flags.AvoidSendAquisitionClickWrenchConfig)
                                    flags.MustSendAquisitionClickWrenchConfig = false;
                                else if (_state != eState.eWaitingAquisitionClickWrenchConfig)
                                    SetState(eState.eMustSendAquisitionClickWrenchConfig);
                            }
                            else if (flags.MustSendAquisitionAdditionalConfig)
                            {
                                if (flags.AvoidSendAquisitionAdditionalConfig)
                                    flags.MustSendAquisitionAdditionalConfig = false;
                                else if (_state != eState.eWaitingAquisitionAdditionalConfig)
                                    SetState(eState.eMustSendAquisitionAdditionalConfig);
                            }
                            else if (flags.MustSendAquisitionAdditional2Config)
                            {
                                if (flags.AvoidSendAquisitionAdditional2Config)
                                    flags.MustSendAquisitionAdditional2Config = false;
                                else if (_state != eState.eWaitingAquisitionAdditional2Config)
                                    SetState(eState.eMustSendAquisitionAdditional2Config);
                            }
                            else if (flags.MustSendGetStatus)
                            {
                                if (_state != eState.eWaitingGetStatus)
                                    SetState(eState.eMustSendGetStatus);
                            }
#if !DEF_IGNORE_COUNTERS
                            else if (flags.MustSendGetCounters)
                            {
                                if (_state != eState.eWaitingCounters)
                                    SetState(eState.eMustSendGetCounters);
                            }
#endif
                            else if (_state != eState.eWaitingAnswerReadCommand &&
                                (System.Environment.TickCount - flags.tickDeviceStatus > DEF_TIMESPAN_GETDEVICESTATUS)
                                )
                            {
                                if (_state != eState.eWaitingDeviceStatus)
                                    SetState(eState.eMustGetDeviceStatus);
                            }
                            else if (flags.MustSendReadData)
                            {
                                if (_state != eState.eWaitingAnswerReadCommand && _state != eState.eWaitBetweenReads)
                                    SetState(eState.eMustSendReadCommand);
                            }
                            else
                            {
                                SetState(eState.eIdle);
                            }
                        }
                        else
                        {
                            SetState(eState.eIdle);
                        }

                        if (!flags.MustSendGetChartBlock)
                        {
                            try
                            {
                                if (TimerItem != null)
                                    TimerItem.Interval = DEF_SLOW_TIMER_INTERVAL;
                            }
                            catch { }
                        }
                        bool btimeouted = false;


                        bool bRestartTightening = false;

                        if (_state == eState.eWaitingID && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_ID, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendGetID);

                            //if (iConsecTimeout_OR_InvalidAnswer % TRIES_BAUD == 0)
                            //ChangeBaudRate();
                        }
                        if (_state == eState.eMustSendGetID)
                        {
                            SetState(eState.eWaitingID);
                            SendCommand(_PortIndex.ToString("x2") + "0000000000ID");
                        }

                        if (_state == eState.eWaitingConfigure && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_CONFIGURATION, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustConfigure);
                        }
                        if (_state == eState.eMustConfigure)
                        {
                            SetState(eState.eWaitingConfigure);
                            SendCommand(flags.Configuration);
                            flags.NewConfiguragion = false;
                        }

                        if (_state == eState.eWaitingDeviceStatus && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_DEVICESTATUS, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustGetDeviceStatus);
                        }
                        if (_state == eState.eMustGetDeviceStatus)
                        {
                            SetState(eState.eWaitingDeviceStatus);
                            SendCommand(_id + "DS");
                        }


                        if (_state == eState.eWaitingGetStatus && Timeouted(TickTXCommand, DEF_TIMESPAN_GETSTATUS, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendGetStatus);
                        }
                        if (_state == eState.eMustSendGetStatus)
                        {
                            SetState(eState.eWaitingGetStatus);
                            SendCommand(_id + "LS");
                        }


                        if (_state == eState.eWaitingChartBlock && Timeouted(TickTXCommand, DEF_TIMESPAN_WAITCHARTBLOCK, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            SetState(eState.eMustSendGetChartBlock);
                            iConsecTimeout_OR_InvalidAnswer++;
                            if (iConsecErrsUnknown == 4)
                            {
                                iConsecErrsUnknown = 0;
                                bRestartTightening = true;
                            }
                        }

                        if (_state == eState.eMustSendGetChartBlock)
                        {
                            if (_blocks.block != null)
                            {
                                SetState(eState.eWaitingChartBlock);
                                if (_blocks.block.Length > _blocks.indextoask)
                                {
                                    // ADDED LOG: information about GD being requested (start, size, step, awaited)
                                    try
                                    {
                                        int start = _blocks.block[_blocks.indextoask, DEF_COLUMN_STARTINDEX];
                                        int blockSize = _blocks.block[_blocks.indextoask, DEF_COLUMN_SIZE];
                                        int step = _blocks.step;
                                        int awaited = 18 + blockSize * 5;
                                        TransducerLogger.LogFmt("Dispatch GD SEND: blockIndex={0} start={1} size={2} step={3} awaited={4}", _blocks.indextoask, start, blockSize, step, awaited);
                                        try { ProtocolFileLogger.WriteProtocol("TX", $"GD_REQUEST idx={_blocks.indextoask} start={start} size={blockSize} step={step}", Encoding.UTF8.GetBytes(_id + "GD" + start.ToString("X4") + blockSize.ToString("X2") + step.ToString("X2"))); } catch { }
                                    }
                                    catch (Exception exLog)
                                    {
                                        TransducerLogger.LogException(exLog, "Dispatch GD SEND logging failed");
                                    }


                                    SendCommand(
                                        _id +
                                        "GD" + ////aqui GD
                                        _blocks.block[_blocks.indextoask, DEF_COLUMN_STARTINDEX].ToString("X4") +
                                        _blocks.block[_blocks.indextoask, DEF_COLUMN_SIZE].ToString("X2") +
                                        _blocks.step.ToString("X2")
                                        ,
                                        18 + _blocks.block[_blocks.indextoask, DEF_COLUMN_SIZE] * 5
                                        );

                                }
                                else
                                {
                                    SetState(eState.eIdle);
                                    Debug.Print("ALGORITHM PROBLEM");
                                    TransducerLogger.Log("ALGORITHM PROBLEM: _blocks length mismatch");
                                }
                            }
                            else
                            {
                                SetState(eState.eIdle);
                            }
                        }

                        if (_state == eState.eWaitBetweenReads && Timeouted(TickTXCommand, GetSpanBetweenReads(), "st:" + _state.ToString()))
                        {
                            if (flags.CaptureNewTightening)
                                flags.MustSendGetStatus = true;
                            else
                                SetState(eState.eIdle);
                        }
                        else if (_state == eState.eWaitingAnswerReadCommand && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_READ, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendReadCommand);
                        }
                        if (_state == eState.eMustSendReadCommand)
                        {
                            SetState(eState.eWaitingAnswerReadCommand);
                            SendCommand(_id + "TQ");

                            TransducerLogger.Log("SendCommand_id + TQ");
                        }


                        if (_state == eState.eWaitAnswerRequestInformation && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_REQUESTINFORMATION, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendRequestInformation);
                        }
                        if (_state == eState.eMustSendRequestInformation)
                        {
                            SetState(eState.eWaitAnswerRequestInformation);
                            SendCommand(_id + "DI");
                        }

                        if (_state == eState.eWaitingZeroTorque && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_ZERO, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendZeroTorque);
                        }
                        if (_state == eState.eMustSendZeroTorque)
                        {
                            SetState(eState.eWaitingZeroTorque);
                            string zt = (flags.MustSendZeroTorque ? "1" : "0");
                            SendCommand(_id + "ZO" + zt + "0");

                        }

                        if (_state == eState.eWaitingZeroAngle && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_ZERO, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendZeroAngle);
                        }
                        if (_state == eState.eMustSendZeroAngle)
                        {
                            SetState(eState.eWaitingZeroAngle);
                            string za = (flags.MustSendZeroAngle ? "1" : "0");
                            SendCommand(_id + "ZO0" + za);
                        }

                        if (_state == eState.eWaitingTorqueOffset && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_ZERO, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendTorqueOffset);
                        }
                        if (_state == eState.eMustSendTorqueOffset)
                        {
                            SetState(eState.eWaitingTorqueOffset);
                            SendCommand(_id + "SO" + ((int)(LimitTorqueOffset(Nm2AD(flags.TorqueOffset)))).ToString("X8"));

                        }

                        if (_state == eState.eWaitingCalibrate && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_CALIBRATE, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendCalibrate);
                        }
                        if (_state == eState.eMustSendCalibrate)
                        {


                            SetState(eState.eWaitingCalibrate);
                            double ft = 0;
                            if (flags.Calibrate_AppliedTorque != 0 && flags.Calibrate_CurrentTorque != 0)
                                ft = (double)(flags.Calibrate_AppliedTorque / flags.Calibrate_CurrentTorque);
                            else
                                ft = 1;
                            double fa = 0;
                            if (flags.Calibrate_AppliedAngle != 0 && flags.Calibrate_CurrentAngle != 0)
                                fa = (double)(flags.Calibrate_AppliedAngle / flags.Calibrate_CurrentAngle);
                            else
                                fa = 1;

                            SendCommand(_id + "CW" +
                                ((int)(TorqueConversionFactor * ft / 0.000000000001)).ToString("X8") +
                                ((int)(AngleConversionFactor * fa / 0.001)).ToString("X8") +
                                "00000000" +
                                "00000000" +
                                "00000000"
                                );

                        }

                        if (_state == eState.eWaitingAquisitionConfig && Timeouted(TickTXCommand, DEF_TIMESPAN_AQUISITIONCONFIG, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendAquisitionConfig);
                        }
                        if (_state == eState.eMustSendAquisitionConfig)
                        {
                            SetState(eState.eWaitingAquisitionConfig);
                            string s = _id + "SA" +
                                ((int)(LimitThreshold(Nm2AD(AquisitionConfig.Threshold)))).ToString("X8") +
                                ((int)(LimitThresholdEnd(Nm2AD(AquisitionConfig.ThresholdEnd)))).ToString("X8") +
                                ((int)(LimitTimeoutEnd_ms(AquisitionConfig.TimeoutEnd_ms))).ToString("X4") +
                                ((int)(LimitTimeStep_ms(AquisitionConfig.TimeStep_ms))).ToString("X4") +
                                ((int)(LimitFilterFrequency(AquisitionConfig.FilterFrequency))).ToString("X4") +
                                ((int)AquisitionConfig.Dir).ToString("X2") +
                                ((int)AquisitionConfig.ToolType).ToString("X2");

                            SendCommand(s);
                        }


                        if (_state == eState.eWaitingAquisitionClickWrenchConfig && Timeouted(TickTXCommand, DEF_TIMESPAN_AQUISITIONCONFIG, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendAquisitionClickWrenchConfig);
                        }
                        if (_state == eState.eMustSendAquisitionClickWrenchConfig)
                        {
                            SetState(eState.eWaitingAquisitionClickWrenchConfig);
                            string s = _id + "CS" +
                                ((int)(LimitClickFall(AquisitionClickWrenchConfig.FallPercentage))).ToString("X2") +
                                ((int)(LimitClickRise(AquisitionClickWrenchConfig.RisePercentage))).ToString("X2") +
                                ((int)(LimitClickWidth_ms(AquisitionClickWrenchConfig.MinTimeBetweenPulses_ms))).ToString("X2");

                            SendCommand(s);
                        }


                        if (_state == eState.eWaitingAquisitionAdditionalConfig && Timeouted(TickTXCommand, DEF_TIMESPAN_AQUISITIONCONFIG, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendAquisitionAdditionalConfig);
                        }
                        if (_state == eState.eMustSendAquisitionAdditionalConfig)
                        {
                            SetState(eState.eWaitingAquisitionAdditionalConfig);
                            string s = _id + "SB" +
                                ((int)(LimitTorqueTarget(Nm2AD(AquisitionConfig.TorqueTarget)))).ToString("X8") +
                                ((int)(LimitTorqueMax(Nm2AD(AquisitionConfig.TorqueMax)))).ToString("X8") +
                                ((int)(LimitTorqueMin(Nm2AD(AquisitionConfig.TorqueMin)))).ToString("X8") +
                                ((int)(LimitAngleTarget(ConvertAngleToBus(AquisitionConfig.AngleTarget)))).ToString("X8") +
                                ((int)(LimitAngleMax(ConvertAngleToBus(AquisitionConfig.AngleMax)))).ToString("X8") +
                                ((int)(LimitAngleMin(ConvertAngleToBus(AquisitionConfig.AngleMin)))).ToString("X8") +
                                "00000000";

                            SendCommand(s);
                        }

                        if (_state == eState.eWaitingAquisitionAdditional2Config && Timeouted(TickTXCommand, DEF_TIMESPAN_AQUISITIONCONFIG, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendAquisitionAdditional2Config);
                        }
                        if (_state == eState.eMustSendAquisitionAdditional2Config)
                        {
                            SetState(eState.eWaitingAquisitionAdditional2Config);
                            string s = _id + "SC" +
                                ((int)(LimitDelayToDetectFirstPeak_ms(AquisitionConfig.DelayToDetectFirstPeak_ms))).ToString("X4") +
                                ((int)(LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms(AquisitionConfig.TimeToIgnoreNewPeak_AfterFinalThreshold_ms))).ToString("X4") +
                                "000000000000000000000000000000000000000000000000";

                            SendCommand(s);
                        }

                        if (_state == eState.eWaitingCounters && Timeouted(TickTXCommand, DEF_TIMESPAN_TIMEOUT_COUNTERS, "st:" + _state.ToString()))
                        {
                            btimeouted = true;
                            iConsecTimeout_OR_InvalidAnswer++;
                            SetState(eState.eMustSendGetCounters);
                        }
                        if (_state == eState.eMustSendGetCounters)
                        {
                            SetState(eState.eWaitingCounters);
                            SendCommand(_id + "RC");
#if DEF_REGINALDO_NAO_IMPLEMENTOU_CONTADORES
                            //0.26Nm
                            string s = _id + "RC" + "00000001" + "00000002" + "0000AAFE" + "00000004" + "00000005";
                            s = "[" + s + makeCRC(s) + "]";
                            SimulateRXData(s);
#endif
                        }

                        if (btimeouted)
                        {
                            if (_state != eState.eWaitingAquisitionAdditionalConfig && _state != eState.eWaitingAquisitionAdditional2Config)
                            {
                                lock (signalmeasure.lmeasure)
                                {
                                    signalmeasure.lmeasure.Add(DEF_MEASURE_TIMEOUT);
                                    signalmeasure.laststatetimeout = (int)_state;

                                    Write2Log("timeout");
                                    TransducerLogger.LogFmt("Timeout detected in state {0}", _state);
                                }
                            }
                        }


                        if (bRestartTightening)
                        {

                            Write2Log("CANCELING CHART", true);
                            TransducerLogger.Log("CANCELING CHART triggered");

                            flags.MustSendGetChartBlock = false;
                            try
                            {
                                if (TesteResult != null)
                                {
                                    TesteResultsList.Clear();
                                }
                            }
                            catch { }
                            SetState(eState.eMustSendReadCommand);
                            StartReadData();

                            KA();
                            flags.MustSendAquisitionConfig = true;
                            flags.MustSendAquisitionAdditionalConfig = true;
                            flags.MustSendAquisitionAdditional2Config = true;

                        }


                        if ((_state != eState.eIdle && iConsecTimeout_OR_InvalidAnswer >= DEF_MAX_ERRS) || iTrashing > 3)
                        {
                            Write2Log("dt max err s" + iConsecTimeout_OR_InvalidAnswer + " " + iTrashing);
                            TransducerLogger.LogFmt("Too many timeouts/errors: iConsecTimeout_OR_InvalidAnswer={0} iTrashing={1}", iConsecTimeout_OR_InvalidAnswer, iTrashing);


                            iConsecTimeout_OR_InvalidAnswer = 0;
                            iTrashing = 0;

                            SetState(eState.eIdle);

                            flags = new sFlags();

                            StopService();
                        }
                    }
                }
            }
            finally
            {
                try
                {
                    if (!shutdown)
                        TimerItem.Enabled = true;
                    else
                    {
                        DisposeTimer();
                    }
                }
                catch { }
            }
        }

        private decimal LimitTorqueOffset(decimal n)
        {
            if (n < DEF_MIN_TORQUEOFFSET) return DEF_MIN_TORQUEOFFSET;
            if (n > DEF_MAX_TORQUEOFFSET) return DEF_MAX_TORQUEOFFSET;
            return n;
        }
        private decimal LimitThreshold(decimal n)
        {
            if (n < DEF_MIN_THRESHOLD) return DEF_MIN_THRESHOLD;
            if (n > DEF_MAX_THRESHOLD) return DEF_MAX_THRESHOLD;
            return n;
        }


        private decimal LimitThresholdEnd(decimal n)
        {
            if (n < DEF_MIN_THRESHOLDEND) return DEF_MIN_THRESHOLDEND;
            if (n > DEF_MAX_THRESHOLDEND) return DEF_MAX_THRESHOLDEND;
            return n;
        }

        private int LimitTimeoutEnd_ms(int n)
        {
            if (n < DEF_MIN_TIMEOUTEND_MS) return DEF_MIN_TIMEOUTEND_MS;
            if (n > DEF_MAX_TIMEOUTEND_MS) return DEF_MAX_TIMEOUTEND_MS;
            return n;
        }
        private int LimitClickFall(int n)
        {
            if (n < DEF_MIN_CLICKFALL) return DEF_MIN_CLICKFALL;
            if (n > DEF_MAX_CLICKFALL) return DEF_MAX_CLICKFALL;
            return n;
        }
        private int LimitClickRise(int n)
        {
            if (n < DEF_MIN_CLICKRISE) return DEF_MIN_CLICKRISE;
            if (n > DEF_MAX_CLICKRISE) return DEF_MAX_CLICKRISE;
            return n;
        }
        private int LimitClickWidth_ms(int n)
        {
            if (n < DEF_MIN_CLICKWIDTH_MS) return DEF_MIN_CLICKWIDTH_MS;
            if (n > DEF_MAX_CLICKWIDTH_MS) return DEF_MAX_CLICKWIDTH_MS;
            return n;
        }

        // Corrige typo em nome da constante (DEF_MAX_TIMESPAN_MS -> DEF_MAX_TIMESTEP_MS)
        // Mantém comportamento original: limita n entre DEF_MIN_TIMESTEP_MS e DEF_MAX_TIMESTEP_MS
        private int LimitTimeStep_ms(int n)
        {
            if (n < DEF_MIN_TIMESTEP_MS) return DEF_MIN_TIMESTEP_MS;
            if (n > DEF_MAX_TIMESTEP_MS) return DEF_MAX_TIMESTEP_MS;
            return n;
        }
        private int LimitFilterFrequency(int n)
        {
            if (n < DEF_MIN_FILTERFREQUENCY) return DEF_MIN_FILTERFREQUENCY;
            if (n > DEF_MAX_FILTERFREQUENCY) return DEF_MAX_FILTERFREQUENCY;
            return n;
        }

        private decimal LimitTorqueTarget(decimal n)
        {
            if (n < DEF_MIN_TORQUETARGET) return DEF_MIN_TORQUETARGET;
            if (n > DEF_MAX_TORQUETARGET) return DEF_MAX_TORQUETARGET;
            return n;
        }
        private decimal LimitTorqueMax(decimal n)
        {
            if (n < DEF_MIN_TORQUEMAX) return DEF_MIN_TORQUEMAX;
            if (n > DEF_MAX_TORQUEMAX) return DEF_MAX_TORQUEMAX;
            return n;
        }
        private decimal LimitTorqueMin(decimal n)
        {
            if (n < DEF_MIN_TORQUEMIN) return DEF_MIN_TORQUEMIN;
            if (n > DEF_MAX_TORQUEMIN) return DEF_MAX_TORQUEMIN;
            return n;
        }
        private decimal LimitAngleTarget(decimal n)
        {
            if (n < DEF_MIN_ANGLETARGET) return DEF_MIN_ANGLETARGET;
            if (n > DEF_MAX_ANGLETARGET) return DEF_MAX_ANGLETARGET;
            return n;
        }
        private decimal LimitAngleMax(decimal n)
        {
            if (n < DEF_MIN_ANGLEMAX) return DEF_MIN_ANGLEMAX;
            if (n > DEF_MAX_ANGLEMAX) return DEF_MAX_ANGLEMAX;
            return n;
        }
        private decimal LimitAngleMin(decimal n)
        {
            if (n < DEF_MIN_ANGLEMIN) return DEF_MIN_ANGLEMIN;
            if (n > DEF_MAX_ANGLEMIN) return DEF_MAX_ANGLEMIN;
            return n;
        }

        private decimal LimitDelayToDetectFirstPeak_ms(decimal n)
        {
            if (n < DEF_MIN_DelayToDetectFirstPeak_ms) return DEF_MIN_DelayToDetectFirstPeak_ms;
            if (n > DEF_MAX_DelayToDetectFirstPeak_ms) return DEF_MAX_DelayToDetectFirstPeak_ms;
            return n;
        }
        private decimal LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms(decimal n)
        {
            if (n < DEF_MIN_LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms) return DEF_MIN_LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms;
            if (n > DEF_MAX_LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms) return DEF_MAX_LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms;
            return n;
        }

        private decimal ConvertAngleFromBus(int n)
        {
            return (decimal)(n * AngleConversionFactor);
        }
        private int ConvertAngleToBus(decimal n)
        {
            return (int)((double)n / (AngleConversionFactor == 0 ? 1 : AngleConversionFactor));
        }

        private decimal AD2Nm(int n)
        {
            return (decimal)(n * TorqueConversionFactor);
        }

        private int Nm2AD(decimal n)
        {
            return (int)((double)n / (TorqueConversionFactor == 0 ? 1 : TorqueConversionFactor));
        }

        private void Test_SimulateRequestInformation()
        {
            DataInformation DataInfo = new DataInformation();
            string[] Colunas = new string[]
            {
                "$ID",
                "MSH5006-A5",
                " 5000",
                "13029",
                "   1",
                " 325",
                "ST ",
                "0002",
                "0437",
                "1",
                "   0",
                "0"
            };

            DataInfo.SetDataInformationByColunms(Colunas);
            try
            {
                if (DataInformation != null)
                    DataInformation(DataInfo);
            }
            catch { }
            flags.MustSendRequestInformation = false;
            TransducerLogger.Log("Test_SimulateRequestInformation: simulated DI delivered");
        }

        private int GetTick()
        {
            return System.Environment.TickCount;
        }
        private bool Timeouted(int tick, int timeout, string printstring = null)
        {
            bool ret = (System.Environment.TickCount - tick) > timeout;
            if (ret && printstring != null)
            {
                Write2Log("timeout:" + printstring);
                TransducerLogger.LogFmt("Timeouted: {0} (tick={1} timeout={2})", printstring, tick, timeout);

                // Also write timeout to protocol log
                try { ProtocolFileLogger.WriteProtocol("SYS", "timeout:" + printstring, null); } catch { }


            }
            return ret;
        }

        private void ClearAllRast()
        {
            try
            {
                for (int i = 0; i < RastCommands.Length; i++)
                {
                    RastTX[i] = 0;
                    RastRX[i] = 0;
                    RastAdditional[i] = 0;
                    RastAdditionalD[i] = 0;
                    RastAdditionalS[i] = null;
                }
            }
            finally { }
        }
        private void ClearSingleRast(string cmd)
        {
            try
            {
                for (int i = 0; i < RastCommands.Length; i++)
                {
                    if (RastCommands[i] == cmd)
                    {
                        RastTX[i] = 0;
                        RastRX[i] = 0;
                        RastAdditional[i] = 0;
                        RastAdditionalD[i] = 0;
                        RastAdditionalS[i] = null;
                        break;
                    }
                }
            }
            finally { }
        }


        private string GetRastInfo()
        {
            string s = "s:" + _state + " "
                + (flags.MustSendGetID ? "1" : "0")
                + (flags.MustSendRequestInformation ? "1" : "0")
                + (flags.MustSendReadData ? "1" : "0")
                + (flags.MustSendAquisitionConfig ? "1" : "0")
                + (flags.MustSendGetStatus ? "1" : "0")
                + (flags.MustSendGetChartBlock ? "1" : "0")
                + (flags.MustSendZeroTorque ? "1" : "0")
                + (flags.MustSendZeroAngle ? "1" : "0")
                + (flags.MustConfigure ? "1" : "0")
                + (flags.MustCalibrate ? "1" : "0")
                + (flags.MustSendTorqueOffset ? "1" : "0")
                + (flags.MustSendAquisitionClickWrenchConfig ? "1" : "0")
                + (flags.MustSendAquisitionAdditionalConfig ? "1" : "0")
                + (flags.MustSendAquisitionAdditional2Config ? "1" : "0")
                + "\n";

            try
            {

                for (int i = 0; i < RastCommands.Length; i++)
                {
                    if (RastCommands[i] != null)
                    {
                        s += RastCommands[i] + ":";
                        if (RastRX[i] > 0 || RastTX[i] > 0)
                            s += RastRX[i] + "(" + RastTX[i] + ")";
                        if (RastAdditional[i] > 0)
                            s += " " + RastAdditional[i];
                        if (RastAdditionalD[i] > 0)
                            s += " " + RastAdditionalD[i].ToString();
                        if (RastAdditionalS[i] != null)
                            s += " " + RastAdditionalS[i];
                        s += "|";
                    }
                }

            }
            finally { }
            return s;
        }
        private void AddDoubleToRast(string cmd, double value)
        {
            try
            {
                for (int i = 0; i < RastCommands.Length; i++)
                {
                    if (RastCommands[i] == null)
                    {
                        RastCommands[i] = cmd;
                    }
                    if (RastCommands[i] == cmd)
                    {
                        RastAdditionalD[i] = value;
                        break;
                    }
                }
            }
            catch { }
        }
        private void AddIntToRast(string cmd, int value)
        {
            try
            {
                for (int i = 0; i < RastCommands.Length; i++)
                {
                    if (RastCommands[i] == null)
                    {
                        RastCommands[i] = cmd;
                    }
                    if (RastCommands[i] == cmd)
                    {
                        RastAdditional[i] = value;
                        break;
                    }
                }
            }
            catch { }
        }
        private void AddStringToRast(string cmd, string value)
        {
            try
            {
                for (int i = 0; i < RastCommands.Length; i++)
                {
                    if (RastCommands[i] == null)
                    {
                        RastCommands[i] = cmd;
                    }
                    if (RastCommands[i] == cmd)
                    {
                        RastAdditionalS[i] = value;
                        break;
                    }
                }
            }
            catch { }
        }
        private void AddToRast(string cmd, bool tx, bool rx, bool additional)
        {
            try
            {
                for (int i = 0; i < RastCommands.Length; i++)
                {
                    if (RastCommands[i] == null)
                    {
                        RastCommands[i] = cmd;
                    }
                    if (RastCommands[i] == cmd)
                    {
                        if (tx && RastTX[i] < int.MaxValue) RastTX[i]++;
                        if (rx && RastRX[i] < int.MaxValue) RastRX[i]++;
                        if (additional && RastAdditional[i] < int.MaxValue) RastAdditional[i]++;
                        break;
                    }
                }
            }
            catch { }
        }


        public List<Tuple<string, byte[]>> GetInitReadPayloads()
        {
            var list = new List<Tuple<string, byte[]>>();

            try
            {
                // 1) ZO (SetZeroTorque) - InitRead usa sempre "1" + "0" (force zero torque on)
                string ztCmd = _id + "ZO" + "1" + "0";
                byte[] ztBytes = Encoding.UTF8.GetBytes(ztCmd);
                list.Add(new Tuple<string, byte[]>(ztCmd, ztBytes));

                // 2) ZO0 (SetZeroAngle) - InitRead uses _id + "ZO0" + "1"
                string zaCmd = _id + "ZO0" + "1";
                byte[] zaBytes = Encoding.UTF8.GetBytes(zaCmd);
                list.Add(new Tuple<string, byte[]>(zaCmd, zaBytes));

                // 3) CS (Click wrench config) - uses clickFall, clickRise, clickMinTime_ms
                string csCmd = _id + "CS" +
                               ((int)LimitClickFall(AquisitionClickWrenchConfig.FallPercentage)).ToString("X2") +
                               ((int)LimitClickRise(AquisitionClickWrenchConfig.RisePercentage)).ToString("X2") +
                               ((int)LimitClickWidth_ms(AquisitionClickWrenchConfig.MinTimeBetweenPulses_ms)).ToString("X2");
                byte[] csBytes = Encoding.UTF8.GetBytes(csCmd);
                list.Add(new Tuple<string, byte[]>(csCmd, csBytes));

                // 4) SA (Aquisition config) - build same fields as InitRead / eMustSendAquisitionConfig
                string sSA = _id + "SA" +
                    ((int)LimitThreshold(Nm2AD(AquisitionConfig.Threshold))).ToString("X8") +
                    ((int)LimitThresholdEnd(Nm2AD(AquisitionConfig.ThresholdEnd))).ToString("X8") +
                    ((int)LimitTimeoutEnd_ms(AquisitionConfig.TimeoutEnd_ms)).ToString("X4") +
                    ((int)LimitTimeStep_ms(AquisitionConfig.TimeStep_ms)).ToString("X4") +
                    ((int)LimitFilterFrequency(AquisitionConfig.FilterFrequency)).ToString("X4") +
                    ((int)AquisitionConfig.Dir).ToString("X2") +
                    ((int)AquisitionConfig.ToolType).ToString("X2");
                byte[] saBytes = Encoding.UTF8.GetBytes(sSA);
                list.Add(new Tuple<string, byte[]>(sSA, saBytes));

                // 5) SB and SC MAY be sent by dispatcher (InitRead doesn't always call them synchronously),
                //    but if you want them included, build them here similarly to dispatcher:
                string sSB = _id + "SB" +
                    ((int)LimitTorqueTarget(Nm2AD(AquisitionConfig.TorqueTarget))).ToString("X8") +
                    ((int)LimitTorqueMax(Nm2AD(AquisitionConfig.TorqueMax))).ToString("X8") +
                    ((int)LimitTorqueMin(Nm2AD(AquisitionConfig.TorqueMin))).ToString("X8") +
                    ((int)LimitAngleTarget(ConvertAngleToBus(AquisitionConfig.AngleTarget))).ToString("X8") +
                    ((int)LimitAngleMax(ConvertAngleToBus(AquisitionConfig.AngleMax))).ToString("X8") +
                    ((int)LimitAngleMin(ConvertAngleToBus(AquisitionConfig.AngleMin))).ToString("X8") +
                    "00000000";
                byte[] sbBytes = Encoding.UTF8.GetBytes(sSB);
                list.Add(new Tuple<string, byte[]>(sSB, sbBytes));

                string sSC = _id + "SC" +
                    ((int)LimitDelayToDetectFirstPeak_ms(AquisitionConfig.DelayToDetectFirstPeak_ms)).ToString("X4") +
                    ((int)LimitTimeToIgnoreNewPeak_AfterFinalThreshold_ms(AquisitionConfig.TimeToIgnoreNewPeak_AfterFinalThreshold_ms)).ToString("X4") +
                    "000000000000000000000000000000000000000000000000";
                byte[] scBytes = Encoding.UTF8.GetBytes(sSC);
                list.Add(new Tuple<string, byte[]>(sSC, scBytes));
            }
            catch (Exception ex)
            {
                // Nunca deixe falha aqui impedir a execução; apenas logue.
                try { TransducerLogger.LogException(ex, "GetInitReadPayloads failed"); } catch { }
            }

            return list;
        }












        // Substitua o método SendCommand existente por este (mantém comportamento, adiciona logs pré-CRC)
        private void SendCommand(string cmd, int awaitedsize = 0)
        {
            for (int i = 0; i < 3; i++)
            {
                try
                {
                    if (((_Eth_IP == string.Empty || _Eth_IP == null)) || (_Eth_IP != string.Empty && _Eth_IP != null && sck.Connected))
                    {
                        // LOG: comando solicitado antes de montar CRC (texto do payload sem CRC)
                        try
                        {
                            TransducerLogger.LogFmt("SendCommand REQUEST: cmd='{0}' awaitedsize={1}", cmd, awaitedsize);
                        }
                        catch { }

                        // --- NOVO: gravação do payload (SEM CRC) no log de protocolo ---
                        try
                        {
                            // payload ASCII sem CRC e sem colchetes
                            string payloadAscii = cmd;

                            // converte payload para bytes (UTF8 como uso atual)
                            byte[] payloadBytes = Encoding.UTF8.GetBytes(payloadAscii);

                            // Grava uma entrada no arquivo de protocolo com a TAG "TX" contendo o ASCII (texto)
                            ProtocolFileLogger.WriteProtocol("TX", payloadAscii, payloadBytes);

                            // Também registra no TransducerLogger em hex (útil na depuração)
                            TransducerLogger.LogHex("TX PAYLOAD BYTES", payloadBytes, 0, payloadBytes.Length);
                        }
                        catch (Exception exLog)
                        {
                            // não interromper envio por falha de logging
                            TransducerLogger.LogException(exLog, "SendCommand: ProtocolFileLogger.WriteProtocol pre-CRC failed");
                        }
                        // --- FIM do log pré-CRC ---




                        // ADDED LOG: se o comando é um GD (pedido de bloco) log detalhado do payload sem CRC
                        try
                        {
                            if (!string.IsNullOrEmpty(cmd) && cmd.Contains("GD"))
                            {
                                try
                                {
                                    TransducerLogger.LogFmt(">> GD SEND (pre-CRC): payload='{0}' awaitedsize={1}", cmd, awaitedsize);
                                    try { ProtocolFileLogger.WriteProtocol("TX", "GD SEND (pre-CRC): " + cmd, Encoding.UTF8.GetBytes(cmd)); } catch { }
                                }
                                catch (Exception exgd) { TransducerLogger.LogException(exgd, "SendCommand GD extra log failed"); }
                            }
                        }
                        catch { }













                        // Agora monta o frame final com CRC e colchetes (comportamento antigo)
                        string cmdout = "[" + cmd + makeCRC(cmd) + "]";
                        _awaitedsize = awaitedsize;

                        // LOG: comando final (com CRC) que será enviado (texto)
                        try { TransducerLogger.LogFmt("TX -> {0}", cmdout); } catch { }

                        // LOG: escrevendo também em hex (bytes que serão enviados)
                        try
                        {
                            byte[] txBytes = Encoding.UTF8.GetBytes(cmdout);
                            TransducerLogger.LogHex("TX BYTES", txBytes, 0, txBytes.Length);

                            // grava o frame completo + bytes (como era antes)
                            ProtocolFileLogger.WriteProtocol("TX Bytes", cmdout, txBytes);
                        }
                        catch (Exception exHexLog)
                        {
                            TransducerLogger.LogException(exHexLog, "SendCommand: ProtocolFileLogger.WriteProtocol frame logging failed");
                        }

                        Write2Log("<-TX: " + System.Environment.TickCount + " :" + cmdout);

                        try { AddToRast(cmdout.Substring(13, 2), true, false, false); } catch { }

                        if (bClosing)
                        {
                            Write2Log("dtTX closing", true);
                            TransducerLogger.Log("SendCommand aborted: bClosing true");
                            return;
                        }

                        waitans = true;

                        if ((_Eth_IP == string.Empty || _Eth_IP == null))
                        {
                            // LOG: SerialPort write (also hex)
                            try
                            {
                                byte[] txBytes = Encoding.UTF8.GetBytes(cmdout);
                                TransducerLogger.LogHex("SerialPort TX BYTES", txBytes, 0, txBytes.Length);
                            }
                            catch { }

                            //SerialPort.Write(cmdout);
                        }



                        else if (_Eth_IP != string.Empty && _Eth_IP != null && sck.Connected)
                        {
                            Byte[] bs = Encoding.UTF8.GetBytes(cmdout);
                            try
                            {
                                // Write + Flush for socket to ensure immediate send
                                swrite.Write(bs, 0, bs.Length);
                                try { swrite.Flush(); } catch { }
                                // LOG bytes sent on socket (hex)
                                TransducerLogger.LogHex("Socket TX BYTES", bs, 0, bs.Length);
                            }
                            catch (Exception exSocket)
                            {
                                TransducerLogger.LogException(exSocket, "Socket write error in SendCommand");
                                throw;
                            }
                        }

                        TrySendCmd++;
                        TickTXCommand = GetTick();

                        ticks.tick_tx = System.Environment.TickCount;
                        signalmeasure.iTickTX = System.Environment.TickCount;
                    }
                    else
                    {
                        Write2Log("send err " + System.Environment.TickCount);
                        TransducerLogger.LogFmt("SendCommand ERROR: connection not open (Eth_IP='{0}', SerialOpen={1}, sckConnected={2})", _Eth_IP, (sck != null && sck.Connected));

                        Internal_StopService();


                    }
                    break;
                }
                catch (Exception ex)
                {
                    // Log completo da exceção e retry
                    TransducerLogger.LogException(ex, "SendCommand exception - retrying");

                }
            }
        } // aqui sendCommand

















        // SetState: log de transição de estado
        private void SetState(eState state)
        {
            lock (locker_State)
            {
                if (_state != state)
                {
                    var old = _state;
                    _state = state;
                    Write2Log("state:" + _state);
                    try { TransducerLogger.LogFmt("SetState: {0} -> {1}", old, state); } catch { }
                }
            }
        }
        private void SetAquisitionState(eAquisitionState st)
        {
            if (_aquisitionstatus.state != st)
            {
                _aquisitionstatus.state = st;
                if (st == eAquisitionState.Finished)
                {
                    flags.MustSendGetCounters = true;
                    flags.MustSendGetChartBlock = true;
                    _aquisitionstatus.state = eAquisitionState.Idle;
                }
            }
        }
        public static string ByteArrayToHexString(byte[] buffer, string separator = " ", int offset = 0, int size = 0)
        {
            if (buffer == null)
                return "";
            string s = string.Empty;
            int max = (size == 0 ? buffer.Length : size);
            for (int i = offset; i < max; i++)
            {
                s += ByteToHex(buffer[i]) + (i < max - 1 ? separator : "");
                if (i > 0 && i % 16 == 0)
                    s += "\n";
            }
            return s;
        }
        public static string ByteToHex(byte b)
        {
            const string hex = "0123456789ABCDEF";
            int lowNibble = b & 0x0F;
            int highNibble = (b & 0xF0) >> 4;
            string s = new string(new char[] { hex[highNibble], hex[lowNibble] });
            return s;
        }
        public static byte[] ConvertStringToBytes(string input)
        {
            MemoryStream stream = new MemoryStream();

            using (StreamWriter writer = new StreamWriter(stream))
            {
                writer.Write(input);
                writer.Flush();
            }
            return stream.ToArray();
        }

        private int LookForByte(byte[] b, char c, int offset = 0)
        {
            for (int i = offset; i < b.Length; i++)
            {
                if (b[i] == c)
                    return i;
            }
            return -1;
        }

        string commandtest;
        private void SimulateRXData(string command)
        {
            commandtest = command;
            serialPort_DataReceived(null);
            TransducerLogger.Log("SimulateRXData called - queued simulated RX");
        }

        public void SetPerformance(ePCSpeed pcspeed, eCharPoints charpoints)
        {
            TransducerLogger.LogFmt("SetPerformance called: pcspeed={0} charpoints={1}", pcspeed, charpoints);
            switch (pcspeed)
            {
                case ePCSpeed.Slow:
                    DEF_SLOW_TIMER_INTERVAL = 100;
                    DEF_TIMESPAN_BETWEENREADS = 500;
                    DEF_TIMESPAN_BETWEENREADS_TRACING = 500;
                    break;

                case ePCSpeed.Medium:
                    DEF_SLOW_TIMER_INTERVAL = 60;
                    DEF_TIMESPAN_BETWEENREADS = 250;
                    DEF_TIMESPAN_BETWEENREADS_TRACING = 250;
                    break;

                case ePCSpeed.Fast:
                default:
                    DEF_SLOW_TIMER_INTERVAL = 40;
                    DEF_TIMESPAN_BETWEENREADS = 100;
                    DEF_TIMESPAN_BETWEENREADS_TRACING = 100;
                    break;
            }

            switch (charpoints)
            {
                case eCharPoints.VeryFew:
                    DEF_MAX_GRAPHSIZE = 100;
                    break;

                case eCharPoints.Few:
                    DEF_MAX_GRAPHSIZE = 600;
                    break;

                case eCharPoints.Many:
                    DEF_MAX_GRAPHSIZE = 2000;
                    break;

                case eCharPoints.Medium:
                default:
                    DEF_MAX_GRAPHSIZE = 1200;
                    break;
            }
        }

        public bool bPrintCommToFile = false;
        public bool bSim_Angle = false;



        public void SetTests(string[] s = null)
        {
            if (s != null)
            {
                if (s.Length >= 1)
                {
                    try
                    {
                        bPrintCommToFile = Convert.ToBoolean(Convert.ToInt32(s[0]));

                    }
                    catch { bPrintCommToFile = false; }
                }
                else
                {
                    bPrintCommToFile = false;
                }
                if (s.Length >= 2)
                {
                    try
                    {
                        bSim_Angle = Convert.ToBoolean(Convert.ToInt32(s[1]));
                    }
                    catch { bSim_Angle = false; }
                }
                else
                {
                    bSim_Angle = false;
                }

            }
        }

        int iTrashing = 0;
        byte[] data = new Byte[2048];
        int tickLastTransducerError = System.Environment.TickCount;






        private void serialPort_DataReceived(object sender)
        {

            if (!waitans) return;
            if (bClosing)
            {
                Write2Log("dtRX closing", true);
                TransducerLogger.Log("serialPort_DataReceived: exit early - bClosing true");
                try { ProtocolFileLogger.WriteProtocol("RX", "dtRX closing", null); } catch { }

                return;
            }

            try
            {
                lock (locker_State)
                {
                    {
                        Write2Log("RX ackd", true);
                        ticks.tick_datareceived = System.Environment.TickCount;
                        TransducerLogger.Log("serialPort_DataReceived: entry");
                        TransducerLogger.LogFmt("RX entry state={0} waitans={1} _IsConnected={2}", _state, waitans, _IsConnected);

                        DataInformation DataInfo = new DataInformation();
                        try
                        {
                            #region PARSE_RX
                            bool bValidCmd = false;
                            int tickCheckGarbage = System.Environment.TickCount;
                            int bspini = 0, bspend = 0;
                            int itry = 0;
                            byte[] bs = new byte[(_awaitedsize > 0 ? _awaitedsize : 200)];
                            int offset = 0;

                            //SerialPort sp = null;
                            //if (_Eth_IP == string.Empty || _Eth_IP == null)
                            //{
                            //if (sender != null)
                            //sp = (SerialPort)sender;
                            //}
                            // else
                            //{
                            //}



                            bool bstart = true;
                            string message = "";
                            int tries = 0;

                            while (true)
                            {
                                bool bTrashing = false;
                                while (true)
                                {
                                    if (bClosing)
                                    {
                                        Write2Log("dtRX closing", true);
                                        return;
                                    }
                                    if (_Eth_IP == string.Empty || _Eth_IP == null)
                                    {
                                        if (sender != null)
                                        {
                                            vazio();

                                        }
                                        else
                                        {
                                            bs = Encoding.UTF8.GetBytes(commandtest);
                                            offset += bs.Length;
                                            TransducerLogger.LogFmt("Simulated RX used, offset now {0}", offset);
                                            TransducerLogger.LogHex("simulated RAW", bs, 0, offset);
                                            try { ProtocolFileLogger.WriteProtocol("RX", "SIMULATED RAW", bs); } catch { }


                                        }
                                    }
                                    else
                                    {

                                        try
                                        {
                                            if (_IsConnected)
                                            {
                                                if (sockStream.DataAvailable)
                                                {
                                                    tries++;
                                                    if (bstart)
                                                        tickCheckGarbage = System.Environment.TickCount;
                                                    bstart = false;
                                                    Debug.Print("------ WILL READ SCK --------- " + System.Environment.TickCount);
                                                    Int32 bytes = sockStream.Read(data, 0, data.Length);
                                                    if (bytes > 0)
                                                    {
                                                        TransducerLogger.LogFmt("socket.Read bytes={0}", bytes);
                                                        TransducerLogger.LogHex("socket RAW", data, 0, bytes);

                                                        message += System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                                                        Debug.Print("------ READ ---------:" + message);
                                                        bs = Encoding.UTF8.GetBytes(message);
                                                        offset += bytes;



                                                        // log raw part read from socket to protocol (incremental)
                                                        try
                                                        {
                                                            byte[] rawpart = new byte[bytes];
                                                            Array.Copy(data, 0, rawpart, 0, bytes);
                                                            ProtocolFileLogger.WriteProtocol("RX", "socket read partial", rawpart);
                                                        }
                                                        catch { }







                                                    }
                                                }
                                                else
                                                {
                                                }
                                            }

                                        }
                                        catch (Exception ex)
                                        {
                                            Debug.Print("Err:" + ex.Message + " details:" + (ex.InnerException != null ? ex.InnerException.Message : "") + " " + System.Environment.TickCount);
                                            TransducerLogger.LogException(ex, "socket.Read exception in serialPort_DataReceived");
                                            try { ProtocolFileLogger.WriteProtocol("SYS", "socket.Read exception: " + ex.Message, null); } catch { }

                                        }
                                    }
                                    if (offset == 0)
                                    {
                                        break;
                                    }
                                    if (
                                        (_state != eState.eWaitingID || _PortIndex != 0)
                                        || !(_Eth_IP == string.Empty || _Eth_IP == null)
                                        )
                                    {
                                        if (
                                                (_awaitedsize > 0 && offset >= _awaitedsize)
                                                ||
                                                (_awaitedsize == 0 && LookForByte(bs, ']') > 0)
                                            )
                                        {
                                            Debug.Print("- ] found -");
                                            TransducerLogger.Log("']' found in buffer");
                                            break;
                                        }
                                    }
                                    if (Timeouted(tickCheckGarbage, DEF_TIMESPAN_ABORTGARBAGE, "garbage"))
                                    {
                                        Debug.Print("Abort garbage. bytes:" + offset + " as:" + _awaitedsize + " mes:" + message + " s:" + _state + " pi:" + _PortIndex);
                                        TransducerLogger.LogFmt("Abort garbage: offset={0} awaited={1} state={2}", offset, _awaitedsize, _state);

                                        try { ProtocolFileLogger.WriteProtocol("SYS", "Abort garbage: offset=" + offset + " awaited=" + _awaitedsize, null); } catch { }

                                        break;
                                    }
                                    else
                                    {
                                        Debug.Print("incomplete. wait. bytes:" + offset + " as:" + _awaitedsize + " mes:" + message + " s:" + _state + " pi:" + _PortIndex);


                                        try
                                        {
                                            Debug.Print(BitConverter.ToString(bs, 0, offset));
                                            bool bEr = false;
                                            bspini = LookForByte(bs, '[');
                                            if (bspini < 0)
                                                bEr = true;

                                            if (bEr)
                                            {
                                                Debug.Print("only trash. giving up");
                                                bTrashing = true;
                                                if (iTrashing < Int32.MaxValue)
                                                    iTrashing++;
                                                TransducerLogger.Log("only trash in buffer - giving up");
                                                try { ProtocolFileLogger.WriteProtocol("SYS", "only trash in buffer - giving up", null); } catch { }

                                                break;
                                            }
                                        }
                                        catch { }
                                    }
                                    Thread.Sleep(10);

                                }


                                if (offset == 0)
                                {
                                    break;
                                }
                                if (bTrashing)
                                {
                                    break;
                                }
                                if (offset > 0)
                                {
                                    if (_awaitedsize > 0 && bspend == 0 && offset >= _awaitedsize)
                                        bspend = _awaitedsize - 2;

                                    string sall = System.Text.Encoding.UTF8.GetString(bs, 0, bs.Length);
                                    Debug.Print("->RX(s all):" + sall);
                                    TransducerLogger.LogFmt("RX raw all (len={0}): {1}", offset, sall);

                                    bspini = LookForByte(bs, '[');
                                    bspend = LookForByte(bs, ']', bspend + 1);

                                    if (bspend > bspini && bspini > -1)
                                    {
                                        iTrashing = 0;
                                        string s = System.Text.Encoding.UTF8.GetString(bs, bspini + 1, bspend - bspini - 3);
                                        Debug.Print("->RX(s one):" + s);

                                        // LOG do payload candidato e do pacote bruto
                                        TransducerLogger.LogFmt("PARSER candidate len={0} s='{1}'", s.Length, s);
                                        TransducerLogger.LogHex("PARSER packet raw", bs, bspini, bspend - bspini + 1);

                                        if (_awaitedsize == 0 ||
                                                bspend - bspini + 1 == _awaitedsize ||
                                                (bspend - bspini == 0x13 && bs[13 + bspini] == 0x45 && bs[14 + bspini] == 0x52)
                                            )
                                        {
                                            try
                                            {
                                                string crcRecv = System.Text.Encoding.UTF8.GetString(bs, bspend - bspini - 2, 2);
                                                string crcCalc = makeCRC(s);
                                                TransducerLogger.LogFmt("PARSER CRC calc={0} recv={1}", crcCalc, crcRecv);
                                                if ((crcCalc == crcRecv) || (_awaitedsize != 0 && DEF_IGNOREGRAPHCRC))
                                                {
                                                    waitans = false;
                                                    bValidCmd = true;
                                                    break;
                                                }
                                                else
                                                {
                                                    Debug.Print("wrong crc. waiting:" + makeCRC(s));
                                                    TransducerLogger.Log("PARSER: wrong CRC, will wait for more data");
                                                    try { ProtocolFileLogger.WriteProtocol("SYS", "PARSER wrong CRC, will wait", Encoding.UTF8.GetBytes(s)); } catch { }

                                                }
                                            }
                                            catch (Exception ex)
                                            {
                                                TransducerLogger.LogException(ex, "PARSER CRC check exception");
                                                try { ProtocolFileLogger.WriteProtocol("SYS", "PARSER CRC check exception: " + ex.Message, null); } catch { }

                                            }
                                        }
                                        if (!bValidCmd)
                                        {
                                            Debug.Print("waiting:" + _awaitedsize + " rx:" + (bspend - bspini + 1));
                                            TransducerLogger.LogFmt("PARSER waiting awaited={0} rx={1}", _awaitedsize, (bspend - bspini + 1));
                                        }
                                        if (_awaitedsize == 0 || bspend - bspini + 1 == _awaitedsize)
                                            break;
                                        else
                                            itry++;
                                    }
                                }
                                Thread.Sleep(5);
                                if (Timeouted(tickCheckGarbage, DEF_TIMESPAN_ABORTGARBAGE, "parse"))
                                {
                                    Debug.Print("TIMEOUT PARSING");
                                    TransducerLogger.Log("TIMEOUT PARSING");
                                    try { ProtocolFileLogger.WriteProtocol("SYS", "TIMEOUT PARSING", null); } catch { }
                                    break;
                                }
                            }


                            #endregion

                            if (!bValidCmd)
                            {
                                if (offset > 0)
                                {
                                    Debug.Print("RX INVALID COMMAND " + offset + " bytes " + System.Environment.TickCount);
                                    lock (signalmeasure.lmeasure)
                                    {
                                        signalmeasure.lmeasure.Add(DEF_MEASURE_GARBAGE);
                                        signalmeasure.lticktx.Add(System.Environment.TickCount - signalmeasure.iTickTX);
                                    }
                                    TransducerLogger.LogFmt("RX INVALID COMMAND: bytes={0}", offset);

                                    // log invalid raw
                                    try
                                    {
                                        byte[] rawInv = new byte[offset];
                                        Array.Copy(bs, 0, rawInv, 0, offset);
                                        ProtocolFileLogger.WriteProtocol("RX", "INVALID_COMMAND bytes=" + offset, rawInv);
                                    }
                                    catch { }



                                }
                            }
                            else
                            {
                                KA();

                                iConsecTimeout_OR_InvalidAnswer = 0;

#if !DEF_IGNORE_TRANSDUCER_ERRORS

                                if (bspend - bspini == 0x13 && bs[13 + bspini] == 0x45 && bs[14 + bspini] == 0x52)
                                {
                                    if (
                                        _state == eState.eWaitingAquisitionConfig &&
                                        Convert.ToInt32(System.Text.Encoding.UTF8.GetString(bs, 15 + bspini, 2)) == 4 &&
                                        iConsecErrs >= 2
                                        )
                                    {
                                        string s = "000008C4D0B4SA01";
                                        s = "[" + s + makeCRC(s) + "]";
                                        bs = Encoding.UTF8.GetBytes(s);

                                        Write2Log("dummy SA");
                                        TransducerLogger.Log("Injected dummy SA response");
                                        try { ProtocolFileLogger.WriteProtocol("RX", "Injected dummy SA response", bs); } catch { }
                                    }
                                    if (
                                        _state == eState.eWaitingAquisitionAdditionalConfig &&
                                        Convert.ToInt32(System.Text.Encoding.UTF8.GetString(bs, 15 + bspini, 2)) == 3 &&
                                        iConsecErrs >= 2
                                        )
                                    {
                                        string s = "000008C4D0B4SB01";
                                        s = "[" + s + makeCRC(s) + "]";
                                        bs = Encoding.UTF8.GetBytes(s);

                                        Write2Log("dummy SB");
                                        TransducerLogger.Log("Injected dummy SB response");
                                        try { ProtocolFileLogger.WriteProtocol("RX", "Injected dummy SB response", bs); } catch { }

                                    }
                                    if (
                                        _state == eState.eWaitingAquisitionAdditional2Config &&
                                        Convert.ToInt32(System.Text.Encoding.UTF8.GetString(bs, 15 + bspini, 2)) == 3 &&
                                        iConsecErrs >= 2
                                        )
                                    {
                                        string s = "000008C4D0B4SC01";
                                        s = "[" + s + makeCRC(s) + "]";
                                        bs = Encoding.UTF8.GetBytes(s);

                                        Write2Log("dummy SC");
                                        TransducerLogger.Log("Injected dummy SC response");
                                        try { ProtocolFileLogger.WriteProtocol("RX", "Injected dummy SC response", bs); } catch { }

                                    }

                                }
                                if (
                                    bspend - bspini == 0x13 && bs[13 + bspini] == 0x45 &&
                                    bs[14 + bspini] == 0x52
                                    )
                                {
                                    lock (signalmeasure.lmeasure)
                                    {
                                        if (_state != eState.eWaitingAquisitionAdditionalConfig && _state != eState.eWaitingAquisitionAdditional2Config)
                                        {
                                            signalmeasure.laststateerr = (int)_state;
                                            signalmeasure.lmeasure.Add(DEF_MEASURE_ERR);

                                            Write2Log("err");
                                            TransducerLogger.Log("Transducer returned ERR packet");
                                        }
                                        signalmeasure.lticktx.Add(System.Environment.TickCount - signalmeasure.iTickTX);
                                    }
                                    int errtran = Convert.ToInt32(System.Text.Encoding.UTF8.GetString(bs, 15 + bspini, 2));

                                    iConsecErrs++;
                                    Debug.Print("######## TRASDUCER RETURNS ERR consec:" + iConsecErrs + " type:" + errtran);

                                    string s = "could not encode command";
                                    try
                                    {
                                        s = System.Text.Encoding.UTF8.GetString(bs, bspini, bspend - bspini + 1);
                                    }
                                    catch { }
                                    Write2Log("<-" + "rx [ERR] " + System.Environment.TickCount + " :" + s + " " + (errtran == 3 ? "INVALID COMMAND" : ""));
                                    TransducerLogger.LogFmt("RX [ERR] {0} errtype={1}", s, errtran);


                                    // também gravar raw do pacote ERR no log de protocolo
                                    try
                                    {
                                        int lenErr = bspend - bspini + 1;
                                        if (lenErr > 0)
                                        {
                                            byte[] rawErr = new byte[lenErr];
                                            Array.Copy(bs, bspini, rawErr, 0, lenErr);
                                            ProtocolFileLogger.WriteProtocol("RX", s, rawErr);
                                        }
                                        else
                                        {
                                            ProtocolFileLogger.WriteProtocol("RX", s, null);
                                        }
                                    }
                                    catch { }








                                    if (errtran == 3 && iConsecErrs > 1)
                                    {
                                        if (_state == eState.eWaitingTorqueOffset)
                                        {
                                            iConsecErrs = 0;
                                            flags.MustSendTorqueOffset = false;
                                            flags.AvoidSendTorqueOffset = true;
                                            try
                                            {
                                                //if (RaiseEvent != null)
                                                //RaiseEvent(TransducerEvent.OldTransducerFirmwareDetected);
                                            }
                                            catch { }
                                        }
                                        if (_state == eState.eWaitingAquisitionClickWrenchConfig)
                                        {
                                            iConsecErrs = 0;
                                            flags.MustSendAquisitionClickWrenchConfig = false;
                                            flags.AvoidSendAquisitionClickWrenchConfig = true;
                                            try
                                            {
                                                //if (RaiseEvent != null)
                                                //RaiseEvent(TransducerEvent.OldTransducerFirmwareDetected);
                                            }
                                            catch { }
                                        }
                                        if (_state == eState.eWaitingAquisitionAdditionalConfig)
                                        {
                                            iConsecErrs = 0;
                                            flags.MustSendAquisitionAdditionalConfig = false;
                                            flags.AvoidSendAquisitionAdditionalConfig = true;
                                            try
                                            {
                                                //if (RaiseEvent != null)
                                                //RaiseEvent(TransducerEvent.OldTransducerFirmwareDetected);
                                            }
                                            catch { }
                                        }
                                        if (_state == eState.eWaitingAquisitionAdditional2Config)
                                        {
                                            iConsecErrs = 0;
                                            flags.MustSendAquisitionAdditional2Config = false;
                                            flags.AvoidSendAquisitionAdditional2Config = true;
                                            try
                                            {
                                                //if (RaiseEvent != null)
                                                //RaiseEvent(TransducerEvent.OldTransducerFirmwareDetected);
                                            }
                                            catch { }
                                        }
                                    }
                                    if (iConsecErrs >= DEF_MAX_ERRS)
                                    {
                                        iConsecErrs = 0;
                                        SetState(eState.eIdle);

                                        flags = new sFlags();

                                        Write2Log("max rx errs " + System.Environment.TickCount);
                                        TransducerLogger.Log("max rx errs reached - stopping service");

                                        Internal_StopService();
                                        try
                                        {
                                            //if (RaiseError != null)
                                            //{
                                            //if (enableraiseerrors)
                                            //RaiseError(errtran);
                                            //}
                                        }
                                        catch { }
                                    }
                                }
                                else
#endif
                                {
                                    lock (signalmeasure.lmeasure)
                                    {
                                        signalmeasure.lmeasure.Add(DEF_MEASURE_OK);
                                        signalmeasure.lticktx.Add(System.Environment.TickCount - signalmeasure.iTickTX);
                                    }
                                    if (iConsecErrs > 22)
                                        Debug.Print("esquema de iConsecErrs funciona :" + iConsecErrs);

                                    iConsecErrs = 0;

                                    int lppini = 0;
                                    if (_state != eState.eWaitingChartBlock || bPrintCommToFile)
                                    {
                                        LastPackage = System.Text.Encoding.UTF8.GetString(bs, bspini, bspend - bspini + 1);

                                        Write2Log("<-" + "rx " + System.Environment.TickCount + " :" + LastPackage);
                                        TransducerLogger.LogFmt("RX package stored LastPackage len={0}", LastPackage?.Length ?? 0);

                                        // gravação completa (texto + raw bytes) no arquivo de protocolo por sessão
                                        try
                                        {
                                            int len = (bspend - bspini + 1);
                                            if (len > 0)
                                            {
                                                byte[] raw = new byte[len];
                                                Array.Copy(bs, bspini, raw, 0, len);
                                                ProtocolFileLogger.WriteProtocol("RX", LastPackage, raw);
                                            }
                                            else
                                            {
                                                ProtocolFileLogger.WriteProtocol("RX", LastPackage, null);
                                            }
                                        }
                                        catch { }




                                    }

                                    lock (locker_State)
                                    {
                                        bool ValidCommand = false;
                                        string com = System.Text.Encoding.UTF8.GetString(bs, bspini + 13, 2);


                                        #region ID
                                        if (_state == eState.eWaitingID)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "ID")
                                            {
                                                ValidCommand = true;
                                                _id = LastPackage.Substring(1 + lppini, 12);
                                                flags.MustSendGetID = false;
                                                TransducerLogger.LogFmt("Parsed ID -> _id={0}", _id);
                                            }
                                        }
                                        #endregion

                                        #region CONFIGURATION
                                        else if (_state == eState.eWaitingConfigure)
                                        {
                                            TickRXCommand = GetTick();

                                            {
                                                ValidCommand = true;
                                                if (!flags.NewConfiguragion)
                                                {
                                                    flags.MustConfigure = false;
                                                    flags.Configuration = "";
                                                    TransducerLogger.Log("CONFIGURE ack received - flags.MustConfigure cleared");
                                                }
                                            }
                                        }
                                        #endregion

                                        #region DEVICE_STATUS
                                        else if (_state == eState.eWaitingDeviceStatus)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "DS")
                                            {
                                                ValidCommand = true;
                                                debug.State = int.Parse(LastPackage.Substring(15 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                debug.Error = int.Parse(LastPackage.Substring(17 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                debug.Temp_mC = int.Parse(LastPackage.Substring(19 + lppini, 4), System.Globalization.NumberStyles.HexNumber);
                                                debug.Interface = int.Parse(LastPackage.Substring(23 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                debug.PowerSource = int.Parse(LastPackage.Substring(25 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                debug.PowerState = int.Parse(LastPackage.Substring(27 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                debug.AnalogPowerState = int.Parse(LastPackage.Substring(29 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                debug.EncoderPowerState = int.Parse(LastPackage.Substring(31 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                debug.PowerVoltage_mV = int.Parse(LastPackage.Substring(33 + lppini, 4), System.Globalization.NumberStyles.HexNumber);
                                                debug.AutoPowerOFFSpan_s = int.Parse(LastPackage.Substring(37 + lppini, 4), System.Globalization.NumberStyles.HexNumber);
                                                debug.ResetReason = int.Parse(LastPackage.Substring(41 + lppini, 4), System.Globalization.NumberStyles.HexNumber);
                                                debug.AliveTime_s = int.Parse(LastPackage.Substring(45 + lppini, 8), System.Globalization.NumberStyles.HexNumber);
                                                debug.TorqueConversionFactor = TorqueConversionFactor;
                                                debug.AngleConversionFactor = AngleConversionFactor;

                                                if (debug.Error != 0 && Timeouted(tickLastTransducerError, 5000))
                                                {
                                                    tickLastTransducerError = System.Environment.TickCount;
                                                    try
                                                    {
                                                        //if (enableraiseerrors && RaiseError != null)
                                                        //RaiseError(104);
                                                    }
                                                    catch { }
                                                }

                                                debug.RastInfo = GetRastInfo();
                                                try
                                                {
                                                    //if (DebugInformation != null)
                                                    //DebugInformation(debug);
                                                }
                                                catch { }

                                                flags.tickDeviceStatus = System.Environment.TickCount;
                                                TransducerLogger.LogFmt("DeviceStatus parsed: State={0} Error={1} Temp_mC={2}", debug.State, debug.Error, debug.Temp_mC);
                                            }
                                        }
                                        #endregion


                                        #region AQUISITION_STATUS
                                        else if (_state == eState.eWaitingGetStatus)
                                        {
                                            TickRXCommand = GetTick();
                                            if (com == "LS")
                                            {
                                                ValidCommand = true;
                                                flags.MustSendGetStatus = false;
                                                SetState(eState.eMustSendReadCommand);

                                                try
                                                {
                                                    eAquisitionState ackst = (eAquisitionState)int.Parse(LastPackage.Substring(15 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                    SetAquisitionState(ackst);
                                                    if (ackst == eAquisitionState.Finished)
                                                    {
                                                        ticks.tick_tighteningfinished = System.Environment.TickCount;
                                                        iChart++;

                                                        flags.CaptureNewTightening = false;
                                                        _aquisitionstatus.dir = (eDirection)int.Parse(LastPackage.Substring(17 + lppini, 2), System.Globalization.NumberStyles.HexNumber);
                                                        _aquisitionstatus.size = int.Parse(LastPackage.Substring(23 + lppini, 4), System.Globalization.NumberStyles.HexNumber);
                                                        _aquisitionstatus.peakindex = int.Parse(LastPackage.Substring(27 + lppini, 4), System.Globalization.NumberStyles.HexNumber);
                                                        _aquisitionstatus.indextht = int.Parse(LastPackage.Substring(31 + lppini, 4), System.Globalization.NumberStyles.HexNumber);



                                                        int number = Convert.ToInt32(LastPackage.Substring(35 + lppini, 8), 16);
                                                        _aquisitionstatus.TorqueResult = AD2Nm(number);
                                                        number = Convert.ToInt32(LastPackage.Substring(43 + lppini, 8), 16);
                                                        _aquisitionstatus.AngleResult = ConvertAngleFromBus(number);

                                                        TransducerLogger.LogFmt("Acquisition finished: TorqueResult={0} AngleResult={1} samples={2}", _aquisitionstatus.TorqueResult, _aquisitionstatus.AngleResult, _aquisitionstatus.size);

                                                        int maxblocksize = DEF_MAX_BLOCKSIZE;
                                                        int maxsamples = DEF_MAX_GRAPHSIZE;

                                                        int lastsample;
                                                        if (_aquisitionstatus.indextht > _aquisitionstatus.peakindex && _aquisitionstatus.indextht < _aquisitionstatus.size)
                                                            lastsample = _aquisitionstatus.indextht;
                                                        else
                                                            lastsample = _aquisitionstatus.size;

                                                        Debug.Print("maxsamples (to ask):" + maxsamples);
                                                        Debug.Print("peakindex: " + _aquisitionstatus.peakindex);
                                                        Debug.Print("size: " + lastsample + " => 0 to " + (lastsample - 1));



                                                        int nblocks = 0;


                                                        int step = ((lastsample - 1) / maxsamples) + 1;
                                                        Debug.Print("step: " + step);

                                                        int nsamples = lastsample / step;
                                                        Debug.Print("samples to ask: " + nsamples);

                                                        int firstsample = lastsample - (nsamples * step);
                                                        Debug.Print("firstsample: " + firstsample);
                                                        int rest = _aquisitionstatus.peakindex % step;

                                                        if (rest > 0)
                                                        {
                                                            firstsample += rest - (firstsample % step);
                                                            while (firstsample + ((nsamples - 1) * step) > lastsample - 1)
                                                                nsamples--;
                                                            Debug.Print("NEW samples to ask: " + nsamples);
                                                            Debug.Print("NEW firstsample: " + firstsample);
                                                        }

                                                        _aquisitionstatus.ResultIndexInOutputSamplesArray = (_aquisitionstatus.peakindex / step) - firstsample;
                                                        nblocks = nsamples / maxblocksize + (nsamples % maxblocksize > 0 ? 1 : 0);

                                                        _blocks.samples = nsamples;
                                                        _blocks.count = nblocks;
                                                        _blocks.block = new int[_blocks.count, 3];
                                                        _blocks.indextoask = 0;
                                                        _blocks.step = step;
                                                        for (int i = 0; i < nblocks; i++)
                                                        {
                                                            _blocks.block[i, DEF_COLUMN_STARTINDEX] = firstsample + i * maxblocksize * step;

                                                            int len = (lastsample - _blocks.block[i, DEF_COLUMN_STARTINDEX]) / step + 1;
                                                            if (len > maxblocksize) len = maxblocksize;
                                                            while (_blocks.block[i, DEF_COLUMN_STARTINDEX] + ((len - 1) * step) > lastsample - 1)
                                                                len--;
                                                            _blocks.block[i, DEF_COLUMN_SIZE] = len;


                                                        }
                                                        if (_blocks.step > 0XFF) _blocks.step = 0xFF;
                                                        if (_blocks.step > lastsample) _blocks.step = lastsample;
                                                        ticks.tick_endcalcs = System.Environment.TickCount;
                                                        try
                                                        {
                                                            if (TimerItem != null)
                                                                TimerItem.Interval = DEF_FAST_TIMER_INTERVAL;
                                                        }
                                                        catch { }

                                                    }
                                                }
                                                catch (Exception ex) { TransducerLogger.LogException(ex, "Parsing LS block exception"); }


                                            }
                                        }
                                        #endregion

                                        #region READ
                                        else if (_state == eState.eWaitingAnswerReadCommand)
                                        {
                                            TickRXCommand = GetTick();
                                            if (com == "TQ")
                                            {
                                                ValidCommand = true;

                                                AddStringToRast("S1", LastPackage.Substring(15 + lppini, 8));
                                                AddDoubleToRast("D1", TorqueConversionFactor);


                                                DataResult Result = new DataResult();



                                                int number = Convert.ToInt32(LastPackage.Substring(23 + lppini, 8), 16);
                                                Result.Angle = ConvertAngleFromBus(number);

                                                number = Convert.ToInt32(LastPackage.Substring(15 + lppini, 8), 16);
                                                Result.Torque = AD2Nm(number);

                                                // LOG: parsed TQ raw and converted values
                                                TransducerLogger.LogFmt("PARSED TQ raw: torqueHex={0} torqueAd={1} torqueNm={2} | angleHex={3} angleAd={4} angleDeg={5}",
                                                    LastPackage.Substring(15 + lppini, 8), number, Result.Torque,
                                                    LastPackage.Substring(23 + lppini, 8), Convert.ToInt32(LastPackage.Substring(23 + lppini, 8), 16), Result.Angle);

                                                if (bSim_Angle)
                                                {
                                                    Result.Angle = Result.Torque * 10;
                                                }

                                                AddIntToRast("I1", number);

                                                try
                                                {
                                                    if (DataResult != null)
                                                    {
                                                        Debug.Print("Informing " + Result.Torque + " " + System.Environment.TickCount);
                                                        TransducerLogger.LogFmt("Publishing DataResult: Torque={0} Angle={1}", Result.Torque, Result.Angle);
                                                        DataResult(Result);
                                                    }
                                                }
                                                catch (Exception ex)
                                                {
                                                    TransducerLogger.LogException(ex, "DataResult event handler threw");
                                                }

                                                SetState(eState.eWaitBetweenReads);
                                            }
                                        }
                                        #endregion

                                        #region AQUISITION_CONFIG
                                        else if (_state == eState.eWaitingAquisitionConfig)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "SA")
                                            {
                                                ValidCommand = true;
                                                flags.MustSendAquisitionConfig = false;
                                                TransducerLogger.Log("SA ack received - MustSendAquisitionConfig cleared");
                                            }
                                        }
                                        else if (_state == eState.eWaitingAquisitionClickWrenchConfig)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "CS")
                                            {
                                                ValidCommand = true;
                                                flags.MustSendAquisitionClickWrenchConfig = false;
                                                TransducerLogger.Log("CS ack received - MustSendAquisitionClickWrenchConfig cleared");
                                            }
                                        }
                                        else if (_state == eState.eWaitingAquisitionAdditionalConfig)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "SB")
                                            {
                                                ValidCommand = true;
                                                flags.MustSendAquisitionAdditionalConfig = false;
                                                TransducerLogger.Log("SB ack received - MustSendAquisitionAdditionalConfig cleared");
                                            }
                                        }
                                        else if (_state == eState.eWaitingAquisitionAdditional2Config)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "SC")
                                            {
                                                ValidCommand = true;
                                                flags.MustSendAquisitionAdditional2Config = false;
                                                TransducerLogger.Log("SC ack received - MustSendAquisitionAdditional2Config cleared");
                                            }
                                        }
                                        #endregion

                                        #region INFORMATION
                                        else if (_state == eState.eWaitAnswerRequestInformation)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "DI")
                                            {
                                                ValidCommand = true;

                                                DataInformation di = new DataInformation();
                                                string sn = LastPackage.Substring(15 + lppini, 8);
                                                if (LastPackage[15] == 0x00)
                                                    sn = "";
                                                string model = LastPackage.Substring(23 + lppini, 32);
                                                if (LastPackage[23] == 0x00)
                                                    model = "";
                                                string hw = LastPackage.Substring(55 + lppini, 4);
                                                string fw = LastPackage.Substring(59 + lppini, 4);
                                                string type = LastPackage.Substring(63 + lppini, 2);
                                                string cap = LastPackage.Substring(65 + lppini, 6);
                                                string outtype;

                                                debug.Type = int.Parse(type, System.Globalization.NumberStyles.HexNumber);

                                                switch (type)
                                                {
                                                    case (DEVTYPE_TORQUEANGLE_ACQ):
                                                    default:
                                                        outtype = "TR ";
                                                        break;

                                                    case (DEVTYPE_TORQUE_ACQ):
                                                        outtype = "ST ";
                                                        break;
                                                }
                                                try
                                                {
                                                    TorqueConversionFactor = Convert.ToInt32(LastPackage.Substring(75 + lppini, 8), 16) * 0.000000000001;
                                                    AngleConversionFactor = Convert.ToInt32(LastPackage.Substring(83 + lppini, 8), 16) * 0.001;
                                                }
                                                catch
                                                {
                                                    TorqueConversionFactor = 1;
                                                    AngleConversionFactor = 1;
                                                }
                                                if (type == DEVTYPE_TORQUE_ACQ && AngleConversionFactor == 0)
                                                    AngleConversionFactor = 1;

                                                try
                                                {
                                                    //if (enableraiseerrors && RaiseError != null)
                                                    //{
                                                    //if (TorqueConversionFactor == 0)
                                                    // RaiseError(102);

                                                    //if (AngleConversionFactor == 0)
                                                    //RaiseError(103);
                                                    //}
                                                }
                                                catch { }

                                                if (TorqueConversionFactor == 0) TorqueConversionFactor = 1;
                                                if (AngleConversionFactor == 0) AngleConversionFactor = 1;

                                                flags.BufferSize = int.Parse(LastPackage.Substring(71 + lppini, 4), System.Globalization.NumberStyles.HexNumber);


                                                Debug.Print("INFO - sn:" + sn + " model:" + model + " hw:" + hw + " fw:" + fw + " type:" + type + " cap:" + cap + " TorqueConversionFactor:" + TorqueConversionFactor + " AngleConversionFactor:" + AngleConversionFactor);

                                                TransducerLogger.LogFmt("DI parsed - sn:{0} model:{1} hw:{2} fw:{3} type:{4} cap:{5}", sn, model, hw, fw, type, cap);

                                                string[] Colunas = new string[]
                                                                {
                                                                    "$ID",
                                                                    sn,
                                                                    cap,
                                                                    "0",
                                                                    "0",
                                                                    "0",
                                                                    outtype,
                                                                    "0",
                                                                    "0",
                                                                    "1",
                                                                    "0",
                                                                    "0",
                                                                    TorqueConversionFactor.ToString(),
                                                                    AngleConversionFactor.ToString(),
                                                                    model,
                                                                    hw,
                                                                    fw,
                                                                    _id
                                                                };

                                                di.SetDataInformationByColunms(Colunas);
                                                if (di != null)
                                                {
                                                    try
                                                    {
                                                        if (DataInformation != null)
                                                        {
                                                            Write2Log("datainfo hardid:" + di.HardID + " " + System.Environment.TickCount);
                                                            DataInformation(di);
                                                            TransducerLogger.Log("DataInformation event fired");
                                                        }
                                                    }
                                                    catch { Debug.Assert(false); }
                                                }
                                                flags.MustSendRequestInformation = false;
                                            }
                                        }
                                        #endregion

                                        #region ZERO
                                        else if (_state == eState.eWaitingZeroTorque)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "ZO")
                                            {
                                                ValidCommand = true;

                                                flags.MustSendZeroTorque = false;
                                                TransducerLogger.Log("ZO ack received - MustSendZeroTorque cleared");
                                            }
                                        }
                                        else if (_state == eState.eWaitingZeroAngle)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "ZO")
                                            {
                                                ValidCommand = true;

                                                flags.MustSendZeroAngle = false;
                                                TransducerLogger.Log("ZO ack received - MustSendZeroAngle cleared");
                                            }
                                        }
                                        else if (_state == eState.eWaitingTorqueOffset)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "SO")
                                            {
                                                ValidCommand = true;

                                                flags.MustSendTorqueOffset = false;
                                                TransducerLogger.Log("SO ack received - MustSendTorqueOffset cleared");
                                            }
                                        }

                                        #endregion

                                        #region CALIBRATE
                                        else if (_state == eState.eWaitingCalibrate)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "CW")
                                            {
                                                ValidCommand = true;

                                                flags.MustCalibrate = false;
                                                TransducerLogger.Log("CW ack received - calibration flags cleared");
                                            }
                                        }
                                        #endregion

                                        #region COUNTERS
                                        else if (_state == eState.eWaitingCounters)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "RC")
                                            {
                                                ValidCommand = true;

                                                CountersInformation di = new CountersInformation();


                                                uint iCycles = Convert.ToUInt32(LastPackage.Substring(15 + lppini, 8), 16);
                                                if (iCycles == UInt32.MaxValue)
                                                    iCycles = 0;
                                                string Cycles = iCycles.ToString();
                                                uint iOvershuts = Convert.ToUInt32(LastPackage.Substring(23 + lppini, 8), 16);
                                                if (iOvershuts == UInt32.MaxValue)
                                                    iOvershuts = 0;
                                                string Overshuts = iOvershuts.ToString();

                                                int number = Convert.ToInt32(LastPackage.Substring(31 + lppini, 8), 16);
                                                string HigherOvershut = AD2Nm(number).ToString("F3");

                                                uint iAdditionalCounter1 = Convert.ToUInt32(LastPackage.Substring(39 + lppini, 8), 16);
                                                if (iAdditionalCounter1 == UInt32.MaxValue)
                                                    iAdditionalCounter1 = 0;
                                                string AdditionalCounter1 = iAdditionalCounter1.ToString();

                                                uint iAdditionalCounter2 = Convert.ToUInt32(LastPackage.Substring(47 + lppini, 8), 16);
                                                if (iAdditionalCounter2 == UInt32.MaxValue)
                                                    iAdditionalCounter2 = 0;
                                                string AdditionalCounter2 = iAdditionalCounter2.ToString();


                                                string[] Colunas = new string[]
                                                                {
                                                                    Cycles.ToString(),
                                                                    Overshuts.ToString(),
                                                                    HigherOvershut.ToString(),
                                                                    AdditionalCounter1.ToString(),
                                                                    AdditionalCounter2.ToString()
                                                                };

                                                di.SetInformationByColunms(Colunas);
                                                if (di != null)
                                                {
                                                    try
                                                    {
                                                        //if (CountersInformation != null)
                                                        //CountersInformation(di);
                                                        TransducerLogger.Log("Counters parsed and set (not fired)");
                                                    }
                                                    catch { Debug.Assert(false); }
                                                }
                                                flags.MustSendGetCounters = false;
                                            }
                                        }
                                        #endregion

                                        #region CHART_BLOCK
                                        if (_state == eState.eWaitingChartBlock)
                                        {
                                            TickRXCommand = GetTick();

                                            if (com == "GD")
                                            {
                                                if (countSimulateGoodChartBlocksBeforeFail > 0)
                                                {
                                                    countSimulateGoodChartBlocksBeforeFail--;
                                                    if (countSimulateGoodChartBlocksBeforeFail == 0)
                                                        simulateChartBlockFail = true;
                                                }
                                                if (simulateChartBlockFail)
                                                    simulateChartBlockFail = false;
                                                else
                                                {
                                                    ValidCommand = true;

                                                    Debug.Print("******** NEW PACKET " + System.Environment.TickCount);
                                                    TransducerLogger.Log("CHART_BLOCK GD received - processing");

                                                    // ADDED LOG: more detailed GD RX log (including block index and packet sizes)
                                                    try
                                                    {
                                                        int currentBlockIndex = _blocks.indextoask;
                                                        int packetLen = (bspend - bspini + 1);
                                                        TransducerLogger.LogFmt("GD RECEIVED: blockIndex={0} packetLen={1} bspini={2} bspend={3}", currentBlockIndex, packetLen, bspini, bspend);
                                                        try
                                                        {
                                                            int lenBytes = bspend - bspini + 1;
                                                            byte[] raw = new byte[lenBytes];
                                                            Array.Copy(bs, bspini, raw, 0, lenBytes);
                                                            ProtocolFileLogger.WriteProtocol("RX", $"GD RECEIVED idx={currentBlockIndex} len={lenBytes}", raw);
                                                        }
                                                        catch { }
                                                    }
                                                    catch (Exception exgdlog)
                                                    {
                                                        TransducerLogger.LogException(exgdlog, "GD RECEIVED extra log failed");
                                                    }









                                                    ticks.tick_initproc = System.Environment.TickCount;
                                                    Debug.Print("meas.\tiniproc-RX:\t" + (ticks.tick_initproc - ticks.tick_datareceived));

                                                    if (_blocks.indextoask == 0)
                                                    {
                                                        TesteResultsList = new List<DataResult>();
                                                        ixHigherTQ = 0;
                                                        higherTQ = 0;
                                                    }

                                                    int parsedSamplesThisBlock = 0;



                                                    for (int i = 15; i < bspend - bspini + 1 - 3; i += 5)
                                                    {
                                                        byte[] b = new byte[3];
                                                        b[0] = bs[i];
                                                        b[1] = bs[i + 1];
                                                        b[2] = bs[i + 2];

                                                        bool bcomplete = false;
                                                        if ((b[0] & 128) == 128)
                                                            bcomplete = true;

                                                        int iaux = (b[0] << 16) + (b[1] << 8) + b[2];
                                                        if (bcomplete)
                                                        {
                                                            iaux |= 255 << 24;
                                                        }

                                                        if (_aquisitionstatus.TorqueResult < 0)
                                                            iaux *= -1;

                                                        byte[] b2 = new byte[2];
                                                        b2[0] = bs[i + 3];
                                                        b2[1] = bs[i + 4];

                                                        bcomplete = false;
                                                        if ((b2[0] & 128) == 128)
                                                            bcomplete = true;

                                                        int iaux2 = (b2[0] << 8) + b2[1];

                                                        if (bcomplete)
                                                        {
                                                            iaux2 = -(65536 - iaux2);
                                                        }


                                                        DataResult Result = new DataResult();
                                                        Result.Torque = Math.Truncate(1000 * AD2Nm(iaux)) / 1000;
                                                        Result.Angle = ConvertAngleFromBus(iaux2);

                                                        if (bSim_Angle)
                                                        {
                                                            Result.Angle = Result.Torque * 10;
                                                        }

                                                        Result.Type = "TV";

                                                        if (TesteResultsList != null)
                                                        {
                                                            Result.SampleTime = TesteResultsList.Count * AquisitionConfig.TimeStep_ms * _blocks.step;
                                                            TesteResultsList.Add(Result);
                                                        }
                                                        if (Result.Torque > higherTQ)
                                                        {
                                                            higherTQ = Result.Torque;
                                                            ixHigherTQ = TesteResultsList.Count - 1;
                                                        }
                                                        parsedSamplesThisBlock++;

                                                    }


                                                    // ADDED LOG: after parsing block, how many samples parsed
                                                    try
                                                    {
                                                        TransducerLogger.LogFmt("GD parsed blockIndex={0} parsedSamples={1} accumulatedSamples={2}", _blocks.indextoask, parsedSamplesThisBlock, TesteResultsList?.Count ?? 0);
                                                        try { ProtocolFileLogger.WriteProtocol("SYS", $"GD parsed idx={_blocks.indextoask} parsedSamples={parsedSamplesThisBlock}", null); } catch { }
                                                    }
                                                    catch { }








                                                    ticks.tick_endproc = System.Environment.TickCount;
                                                    _blocks.indextoask++;
                                                    if (_blocks.indextoask >= _blocks.count)
                                                    {

                                                        flags.MustSendGetChartBlock = false;
                                                        SetState(eState.eMustSendReadCommand);


                                                        DataResult Result = new DataResult();
                                                        Result.Angle = _aquisitionstatus.AngleResult;
                                                        Result.Torque = _aquisitionstatus.TorqueResult;
                                                        Result.SampleTime = _aquisitionstatus.ResultIndexInOutputSamplesArray * AquisitionConfig.TimeStep_ms * _blocks.step;


                                                        Result.Type = "FR";
                                                        Result.ThresholdDir = (int)_aquisitionstatus.dir;
                                                        Result.ResultDir = (Result.Torque > 0 ? 0 : 1);

                                                        Result.Torque = Math.Abs(Result.Torque);

                                                        if (bSim_Angle)
                                                        {
                                                            Result.Angle = Result.Torque * 10;
                                                        }

                                                        if (TesteResultsList != null)
                                                            TesteResultsList.Add(Result);

                                                        if (TesteResult != null)
                                                        {
                                                            try
                                                            {
                                                                TransducerLogger.LogFmt("Publishing TesteResult: samples_count={0} higherTQ={1} ixHigher={2}", TesteResultsList?.Count ?? 0, higherTQ, ixHigherTQ);
                                                                // ADDED LOG: before firing TesteResult write a protocol entry
                                                                try { ProtocolFileLogger.WriteProtocol("RX", $"GD COMPLETE totalSamples={TesteResultsList?.Count ?? 0}", null); } catch { }



                                                                TesteResult(TesteResultsList);
                                                            }
                                                            finally { }
                                                            TesteResultsList.Clear();
                                                        }

                                                        ticks.tick_endall = System.Environment.TickCount;

                                                        try
                                                        {
                                                            if (TimerItem != null)
                                                                TimerItem.Interval = DEF_SLOW_TIMER_INTERVAL;
                                                        }
                                                        catch { }
                                                        countGraphsComplete++;
                                                        AddIntToRast("G1", countGraphsComplete);

                                                    }
                                                    else
                                                    {
                                                        SetState(eState.eMustSendGetChartBlock);
                                                    }
                                                }

                                            }

                                        }

                                        #endregion

                                        if (ValidCommand)
                                        {
                                            AddToRast(com, false, true, false);
                                            iConsecErrsUnknown = 0;
                                        }

                                    }

                                }

                            }

                        }

                        catch (Exception err)
                        {
                            var erro = err.Message;
                            Debug.Print("############### ERRO PhoenixTransducer ##################");
                            Debug.Print(err.Message);
                            iConsecErrsUnknown++;

                            TransducerLogger.LogException(err, "serialPort_DataReceived catch (unknown)");
                            Write2Log("data catch (unknown) " + err.Message + " " + (err.InnerException != null ? err.InnerException.Message : "") + " " + System.Environment.TickCount, true);
                            try { ProtocolFileLogger.WriteProtocol("SYS", "data catch (unknown) " + err.Message, null); } catch { }


                        }
                    }
                }
            }
            catch (Exception ex)
            {
                TransducerLogger.LogException(ex, "serialPort_DataReceived outer catch");
                Write2Log("RX err " + ex.Message, true);
                try { ProtocolFileLogger.WriteProtocol("SYS", "RX err " + ex.Message, null); } catch { }

            }
            finally
            {
                Write2Log("RX out", true);
                TransducerLogger.Log("serialPort_DataReceived: exit");
                try { ProtocolFileLogger.WriteProtocol("SYS", "serialPort_DataReceived exit", null); } catch { }
            }





        }//aqui




















        public string PortName
        {
            set { _PortName = value; }
        }
        public int PortIndex
        {
            set { _PortIndex = value; }

        }








        public string Eth_IP
        {
            set { _Eth_IP = value; }
        }
        public int Eth_Port
        {
            set { _Eth_Port = value; }
        }


        public bool IsConnected
        {
            get { return _IsConnected; }
        }




        private object objcon = new object();
        private bool enableraiseerrors = false;



        public void StartService()
        {
            Debug.Print("----- PHOENIX START SERVICE -------");
            TransducerLogger.Log("StartService called");
            bUserStartService = true;
            try { ProtocolFileLogger.WriteProtocol("SYS", "StartService called", null); } catch { }

            lock (signalmeasure.lmeasure)
            {
                try
                {
                    signalmeasure.lmeasure.Clear();
                    signalmeasure.lticktx.Clear();
                }
                catch { }
            }

            enableraiseerrors = true;
            Internal_StartService();
        }

        int masterka = System.Environment.TickCount;
        bool bClosing = false;
        public void KA()
        {
            masterka = System.Environment.TickCount;
        }










        public void vazio()
        {

        }






        public void Internal_StartService()
        {
            SetTimer();

            if (_Eth_IP == string.Empty || _Eth_IP == null)
                vazio();
            else
                Internal_StartService_Eth();


        }
        public void Internal_StartService_Eth()
        {
            lock (objcon)
            {
                bool bcon = false;
                bool braise = false;
                Exception expaux = null;
                try
                {
                    Debug.Print("%%%%%%%%%%%%% StartService %%%%%%%%%%%%%%%%%% " + System.Environment.TickCount);
                    try
                    {
                        if (sck != null)
                        {
                            if (sck.Connected)
                            {
                                sck.Close();

                            }
                        }
                    }
                    catch { }

                    sck = new TcpClient();

                    sck.ReceiveBufferSize = 2048;
                    sck.ReceiveTimeout = 2000;

                    sck.SendBufferSize = 250;
                    sck.SendTimeout = 2000;

                    Debug.Print("%%%%%%%%%%%%% StartService Connect %%%%%%%%%%%%%%%%%% " + System.Environment.TickCount);
                    sck.Connect(_Eth_IP, _Eth_Port);
                    Debug.Print("%%%%%%%%%%%%% StartService connected %%%%%%%%%%%%%%%%%% " + System.Environment.TickCount);

                    TransducerLogger.LogFmt("ETH CONNECTED: {0}:{1}", _Eth_IP, _Eth_Port);
                    try { ProtocolFileLogger.WriteProtocol("SYS", $"ETH CONNECTED: {_Eth_IP}:{_Eth_Port}", null); } catch { }



                    Debug.Print("%%%%%%%%%%%%% StartService GetStream %%%%%%%%%%%%%%%%%% " + System.Environment.TickCount);

                    sockStream = sck.GetStream();
                    swrite = new BinaryWriter(sockStream, System.Text.Encoding.UTF8);
                    Debug.Print("%%%%%%%%%%%%% StartService GetStream OK %%%%%%%%%%%%%%%%%% " + System.Environment.TickCount);

                    try
                    {
                        if (sockStream.DataAvailable)
                        {
                            Debug.Print("------ WILL READ TRASH TO DISCARD --------- " + System.Environment.TickCount);
                            Int32 bytes = sockStream.Read(data, 0, data.Length);
                            if (bytes > 0)
                            {
                                string message = System.Text.Encoding.ASCII.GetString(data, 0, bytes);
                                Debug.Print("------ DISCARDED: " + bytes + " " + message);
                                TransducerLogger.LogFmt("StartService_Eth: discarded initial bytes={0} msg='{1}'", bytes, message);


                                try
                                {
                                    byte[] rawDiscard = new byte[bytes];
                                    Array.Copy(data, 0, rawDiscard, 0, bytes);
                                    ProtocolFileLogger.WriteProtocol("RX", "StartService_Eth discarded initial bytes", rawDiscard);
                                }
                                catch { }






                            }
                        }
                    }
                    //catch (Exception ex) { TransducerLogger.LogException(ex, "StartService_Eth discard read"); }
                    catch (Exception ex) { TransducerLogger.LogException(ex, "StartService_Eth discard read"); try { ProtocolFileLogger.WriteProtocol("SYS", "StartService_Eth discard read exception: " + ex.Message, null); } catch { } }


                    bcon = true;

                }
                catch (Exception err) { braise = true; expaux = err; TransducerLogger.LogException(err, "Internal_StartService_Eth exception"); }
                finally
                {
                    _IsConnected = bcon;
                }
                if (braise)
                {
                    Write2Log("starteth ip:" + _Eth_IP + " port:" + _Eth_Port + " err " + (expaux != null ? expaux.Message + (expaux.InnerException != null ? expaux.InnerException.Message : "") : "") + System.Environment.TickCount);

                    Internal_StopService();
                    try
                    {
                        //if (RaiseError != null)
                        //{
                        //if (enableraiseerrors)
                        //RaiseError(101);
                        //}
                    }
                    catch { }
                }
            }
        }










        private void ClearNeeds()
        {
            flags = new sFlags();
            Write2Log("clear needs");
            TransducerLogger.Log("ClearNeeds called - flags cleared");
            try { ProtocolFileLogger.WriteProtocol("SYS", "clear needs", null); } catch { }



        }

        public void StopService()
        {
            Debug.Print("----- PHOENIX STOP SERVICE -------");
            bUserStartService = false;

            Write2Log("stop service " + System.Environment.TickCount);
            TransducerLogger.Log("StopService called");
            try { ProtocolFileLogger.WriteProtocol("SYS", "StopService called", null); } catch { }


            enableraiseerrors = false;

            Internal_StopService();
        }
        public void Internal_StopService()
        {
            lock (objcon)
            {
                Debug.Print("%%%%%%%%%%%%% StopService %%%%%%%%%%%%%%%%%%");
                Write2Log("internal stop service " + System.Environment.TickCount);
                TransducerLogger.Log("Internal_StopService called");
                try { ProtocolFileLogger.WriteProtocol("SYS", "Internal_StopService called", null); } catch { }



                try
                {
                    Thread.Sleep(200);
                    ClearNeeds();
                    this.StopReadData();
                    Debug.Print("%%%%%%%%%%%%% CLOSE PORT %%%%%%%%%%%%%%%%%%");
                    if (_Eth_IP == string.Empty || _Eth_IP == null)
                    {
                        //Thread t = new Thread(() => ClosePort(SerialPort));
                        //t.Start();
                        //t.Join(2000);
                        vazio();

                    }
                    else
                    {
                        sck.Close();

                    }
                    Debug.Print("%%%%%%%%%%%%% PORT CLOSED %%%%%%%%%%%%%%%%%%");

                    _IsConnected = false;
                }
                catch (Exception err)
                {
                    TransducerLogger.LogException(err, "Internal_StopService exception");
                    throw err;
                }
                Write2Log("stopped " + System.Environment.TickCount);
                TransducerLogger.Log("Internal_StopService completed");

            }
        }
        bool shutdown = false;
        public void Dispose()
        {
            try
            {
                //if (SerialPort.IsOpen)
                //{
                //this.StopReadData();
                //this.StopService();
                //}

                sck.Close();
                shutdown = true;
                DisposeTimer();
                TransducerLogger.Log("Dispose called - cleaned up resources");
                try { ProtocolFileLogger.WriteProtocol("SYS", "Dispose called", null); } catch { }


            }
            //catch (Exception ex) { TransducerLogger.LogException(ex, "Dispose exception"); }
            catch (Exception ex) { TransducerLogger.LogException(ex, "Dispose exception"); try { ProtocolFileLogger.WriteProtocol("SYS", "Dispose exception: " + ex.Message, null); } catch { } }

        }
        public void StartCalibration()
        {
            try
            {

                flags.MustSendReadData = true;
                TransducerLogger.Log("StartCalibration called - MustSendReadData set true");
                try { ProtocolFileLogger.WriteProtocol("SYS", "StartCalibration called", null); } catch { }


            }
            catch (Exception err)
            {

                TransducerLogger.LogException(err, "StartCalibration exception");
                throw err;
            }

        }
        public void StartReadData()
        {
            try
            {
                flags.MustSendReadData = true;
                flags.CaptureNewTightening = true;
                TransducerLogger.Log("StartReadData called - MustSendReadData & CaptureNewTightening true");
                try { ProtocolFileLogger.WriteProtocol("SYS", "StartReadData called", null); } catch { }

            }
            catch (Exception err)
            {

                TransducerLogger.LogException(err, "StartReadData exception");
                throw err;
            }

        }
        public void StopReadData()
        {
            try
            {

                flags.MustSendReadData = false;
                flags.MustSendAquisitionConfig = false;
                flags.MustSendGetChartBlock = false;
                flags.CaptureNewTightening = false;

                flags.MustSendAquisitionClickWrenchConfig = false;
                flags.MustSendAquisitionAdditionalConfig = false;
                flags.MustSendAquisitionAdditional2Config = false;

                TransducerLogger.Log("StopReadData called - acquisition flags cleared");
                try { ProtocolFileLogger.WriteProtocol("SYS", "StopReadData called", null); } catch { }

            }
            catch (Exception err)
            {
                TransducerLogger.LogException(err, "StopReadData exception");
                throw err;
            }

        }
        public void SetZeroTorque()
        {
            try
            {
                flags.MustSendZeroTorque = true;
                System.Threading.Thread.Sleep(10);
                TransducerLogger.Log("SetZeroTorque called - MustSendZeroTorque true");
                try { ProtocolFileLogger.WriteProtocol("SYS", "SetZeroTorque called", null); } catch { }

                StartReadData();
            }
            catch (Exception err)
            {
                TransducerLogger.LogException(err, "SetZeroTorque exception");
                throw err;
            }
        }
        public void SetZeroAngle()
        {
            try
            {
                flags.MustSendZeroAngle = true;
                System.Threading.Thread.Sleep(10);
                TransducerLogger.Log("SetZeroAngle called - MustSendZeroAngle true");
                try { ProtocolFileLogger.WriteProtocol("SYS", "SetZeroAngle called", null); } catch { }


            }
            catch (Exception err)
            {
                TransducerLogger.LogException(err, "SetZeroAngle exception");
                throw err;
            }
        }
        public void SetTorqueOffset(decimal torqueoffset)
        {
            try
            {
                flags.MustSendTorqueOffset = true;
                flags.TorqueOffset = torqueoffset;
                System.Threading.Thread.Sleep(10);
                TransducerLogger.LogFmt("SetTorqueOffset called - torqueoffset={0}", torqueoffset);
                try { ProtocolFileLogger.WriteProtocol("SYS", "SetTorqueOffset called: " + torqueoffset, null); } catch { }


            }
            catch (Exception err)
            {
                TransducerLogger.LogException(err, "SetTorqueOffset exception");
                throw err;
            }
        }
        public void SetTestParameter(DataInformation Info, TesteType Type, ToolType toolType, decimal NominalTorque, decimal Threshold)
        {
            SetTestParameter(Info, Type, toolType, NominalTorque, Threshold, Threshold / 2, 10, 1, 500, eDirection.CW);
            TransducerLogger.Log("SetTestParameter overload called (short)");
        }
        public void SetTestParameter(DataInformation Info, TesteType Type, ToolType toolType, decimal NominalTorque, decimal Threshold, decimal ThresholdEnd, int TimeoutEnd_ms, int TimeStep_ms, int FilterFrequency, eDirection direction)
        {
            SetTestParameter(Info, Type, toolType, NominalTorque, Threshold, ThresholdEnd, TimeoutEnd_ms, TimeStep_ms, FilterFrequency, direction, 0, 0, 0, 0, 0, 0, 0, 0);
            TransducerLogger.Log("SetTestParameter overload called (mid)");
        }
        public void SetTestParameter(DataInformation Info, TesteType Type, ToolType toolType, decimal NominalTorque, decimal Threshold, decimal ThresholdEnd, int TimeoutEnd_ms, int TimeStep_ms, int FilterFrequency, eDirection direction, decimal TorqueTarget, decimal TorqueMin, decimal TorqueMax, decimal AngleTarget, decimal AngleMin, decimal AngleMax, int DelayToDetectFirstPeak_ms, int TimeToIgnoreNewPeak_AfterFinalThreshold_ms)
        {
            KA();
            flags.MustSendAquisitionConfig = true;
            flags.MustSendAquisitionAdditionalConfig = true;
            flags.MustSendAquisitionAdditional2Config = true;

            AquisitionConfig.ToolType = toolType;
            AquisitionConfig.Threshold = Threshold;
            AquisitionConfig.ThresholdEnd = ThresholdEnd;
            AquisitionConfig.TimeoutEnd_ms = TimeoutEnd_ms;
            AquisitionConfig.TimeStep_ms = TimeStep_ms;
            AquisitionConfig.FilterFrequency = FilterFrequency;
            AquisitionConfig.Dir = direction;

            AquisitionConfig.TorqueTarget = TorqueTarget;
            AquisitionConfig.TorqueMin = TorqueMin;
            AquisitionConfig.TorqueMax = TorqueMax;
            AquisitionConfig.AngleTarget = AngleTarget;
            AquisitionConfig.AngleMin = AngleMin;
            AquisitionConfig.AngleMax = AngleMax;
            AquisitionConfig.DelayToDetectFirstPeak_ms = DelayToDetectFirstPeak_ms;
            AquisitionConfig.TimeToIgnoreNewPeak_AfterFinalThreshold_ms = TimeToIgnoreNewPeak_AfterFinalThreshold_ms;

            TransducerLogger.LogFmt("SetTestParameter called: Threshold={0} ThresholdEnd={1} TimeStep_ms={2} FilterFreq={3}", Threshold, ThresholdEnd, TimeStep_ms, FilterFrequency);
            try
            {
                ProtocolFileLogger.WriteProtocol("SYS", $"SetTestParameter: Threshold={Threshold} ThresholdEnd={ThresholdEnd} TimeStep_ms={TimeStep_ms} FilterFreq={FilterFrequency} direction={direction} " +
            $"TorqueTarget={TorqueTarget} TorqueMin={TorqueMin} TorqueMax={TorqueMax} AngleTarget={AngleTarget} " +
            $"AngleMin={AngleMin} AngleMax={AngleMax} DelayToDetectFirstPeak_ms={DelayToDetectFirstPeak_ms} " +
            $"TimeToIgnoreNewPeak_AfterFinalThreshold_ms={TimeToIgnoreNewPeak_AfterFinalThreshold_ms}", null);
            }
            catch { }


        }

        public void SetTestParameter_ClickWrench(ushort FallPercentage, ushort RisePercentage, ushort MinTimeBetweenPulses_ms)
        {
            flags.MustSendAquisitionClickWrenchConfig = true;

            AquisitionClickWrenchConfig.FallPercentage = FallPercentage;
            AquisitionClickWrenchConfig.RisePercentage = RisePercentage;
            AquisitionClickWrenchConfig.MinTimeBetweenPulses_ms = MinTimeBetweenPulses_ms;

            TransducerLogger.LogFmt("SetTestParameter_ClickWrench called: Fall={0} Rise={1} MinTime_ms={2}", FallPercentage, RisePercentage, MinTimeBetweenPulses_ms);
            try { ProtocolFileLogger.WriteProtocol("SYS", $"SetTestParameter_ClickWrench: Fall={FallPercentage} Rise={RisePercentage} MinTime_ms={MinTimeBetweenPulses_ms}", null); } catch { }


        }


        public void Calibrate(decimal AppliedTorque, decimal CurrentTorque, decimal AppliedAngle, decimal CurrentAngle)
        {
            try
            {
                flags.MustCalibrate = true;
                flags.Calibrate_AppliedTorque = AppliedTorque;
                flags.Calibrate_CurrentTorque = CurrentTorque;
                flags.Calibrate_AppliedAngle = AppliedAngle;
                flags.Calibrate_CurrentAngle = CurrentAngle;
                TransducerLogger.LogFmt("Calibrate called: AppliedTorque={0} CurrentTorque={1} AppliedAngle={2} CurrentAngle={3}", AppliedTorque, CurrentTorque, AppliedAngle, CurrentAngle);
                try { ProtocolFileLogger.WriteProtocol("SYS", $"Calibrate called: AppliedTorque={AppliedTorque}", null); } catch { }


            }
            catch (Exception err)
            {
                TransducerLogger.LogException(err, "Calibrate exception");
                throw err;
            }
        }

        public void StartCommunication()
        {
            KA();
            try
            {
                lock (signalmeasure.lmeasure)
                {
                    try
                    {
                        signalmeasure.lmeasure.Clear();
                        signalmeasure.lticktx.Clear();
                    }
                    catch { }
                }

                flags.MustSendGetID = true;
                flags.MustSendGetCounters = true;

                TransducerLogger.Log("StartCommunication called - MustSendGetID & MustSendGetCounters true");
                try { ProtocolFileLogger.WriteProtocol("SYS", "StartCommunication called", null); } catch { }


            }
            catch (Exception err)
            {
                TransducerLogger.LogException(err, "StartCommunication exception");
                throw err;
            }
        }
    }
}