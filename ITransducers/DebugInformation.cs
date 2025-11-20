using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ITransducers
{
    public class DebugInformation
    {
        public int State { get; set; }
        public int Error { get; set; }
        public int Temp_mC { get; set; }
        public int Interface { get; set; }  

        public int Type { get; set; }      

        public int PowerSource { get; set; }  
        public int PowerState { get; set; }     
        public int AnalogPowerState { get; set; }  
        public int EncoderPowerState { get; set; }  
        public int PowerVoltage_mV { get; set; } 
        public int AutoPowerOFFSpan_s { get; set; } 
        public int ResetReason { get; set; } 
        public int AliveTime_s { get; set; } 
        public string RastInfo { get; set; } 

        public double TorqueConversionFactor { get; set; }
        public double AngleConversionFactor { get; set; }
    }
}
