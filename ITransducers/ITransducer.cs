using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ITransducers
{

    public delegate void DataInformationReceiver(DataInformation Data);
    public delegate void DataResultReceiver(DataResult Data);
    public delegate void DataTesteResultReceiver(List<DataResult> TesteResult);
    public delegate void ErrorReceiver(int err); 
    public delegate void DebugInformationReceiver(DebugInformation Data); 
    public delegate void EventReceiver(TransducerEvent ev); 
    public delegate void CountersInformationReceiver(CountersInformation Data); 

    public enum TesteType
    {
        TorqueOnly = 0,
        TorqueAngle = 1,
        AngleCheck = 2
    }

    public enum ToolType
    {
        ToolType1 = 1,
        ToolType2 = 2,
        ToolType3 = 3,
        ToolType4 = 4,
        ToolType5 = 5,
        ToolType6 = 6,
        ToolType7 = 7,
        ToolType8 = 8, 
        ToolType9 = 9, 
        ToolType10 = 10 
        
    }

    public enum TransducerEvent
    {
        CalibrationOK = 0,
        OldTransducerFirmwareDetected
    }

    public enum eDirection
    {
        CW,
        CCW,
        BOTH
    }
    public enum ePCSpeed
    {
        Slow,
        Medium,
        Fast,
    }
    public enum eCharPoints
    {
        VeryFew,
        Few,
        Medium,
        Many
    }
    public interface ITransducer : IDisposable
    {
        string PortName { set; }
        int PortIndex { set; }
        bool IsConnected { get; }

        string Eth_IP { set; }
        int Eth_Port { set; }
        
        event DataInformationReceiver DataInformation;
        event DataResultReceiver DataResult;
        event DataTesteResultReceiver TesteResult;
        event ErrorReceiver RaiseError; 
        event DebugInformationReceiver DebugInformation; 
        event EventReceiver RaiseEvent; 
        event CountersInformationReceiver CountersInformation;

        void RequestInformation();
        void WriteSetup(DataInformation Info);

        void Calibrate(decimal AppliedTorque, decimal CurrentTorque, decimal AppliedAngle, decimal CurrentAngle);

        void SetTestParameter(DataInformation Info, TesteType Type, ToolType toolType, decimal NominalTorque, decimal Threshold);
        void SetTestParameter(DataInformation Info, TesteType Type, ToolType toolType, decimal NominalTorque, decimal Threshold, decimal ThresholdEnd, int TimeoutEnd_ms, int TimeStep_ms, int FilterFrequency, eDirection direction);

        void SetTestParameter(DataInformation Info, TesteType Type, ToolType toolType, decimal NominalTorque, decimal Threshold, decimal ThresholdEnd, int TimeoutEnd_ms, int TimeStep_ms, int FilterFrequency, eDirection direction, decimal TorqueTarget, decimal TorqueMin, decimal TorqueMax, decimal AngleTarget, decimal AngleMin, decimal AngleMax, int DelayToDetectFirstPeak_ms, int TimeToIgnoreNewPeak_AfterFinalThreshold_ms);  
        void SetTestParameter_ClickWrench(ushort FallPercentage, ushort RisePercentage, ushort MinTimeBetweenPulses_ms);  
        
        void StartService();
        void StopService();
        void StartCalibration();
        void StartReadData();
        void StopReadData();
        void SetZeroTorque();
        void SetZeroAngle();
        void StartCommunication();

        
        void SetTorqueOffset(decimal torqueoffset);  

        void SetTests(string[] s=null);
        void KA();
        void SetPerformance(ePCSpeed pcspeed, eCharPoints charpoints);

        bool GetMeasures(out int ptim, out int pok, out int perr, out int pgarb, out int iansavg, out bool validbateryinfo, out int batterylevel, out bool charging, out int Interface, out int laststatetimeout, out int laststateerr);

        //void GetInitReadPayloads();

        //void GetInitReadFrames();

        List<Tuple<string, byte[]>> GetInitReadFrames();

        // Opcional: também retornar só os payloads (sem CRC/colchetes) se quiser
        List<Tuple<string, byte[]>> GetInitReadPayloads();


    }
}
