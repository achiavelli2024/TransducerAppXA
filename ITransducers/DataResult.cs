using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ITransducers
{
    public class DataResult
    {
        public int ThresholdDir { get; set; }
        public int ResultDir { get; set; }
        
        public string Type { get; set; }
        public decimal Torque { get; set; }
        public decimal Angle { get; set; }

        public int SampleTime { get; set; }

        public DateTime Date { get; set; }
        public decimal BatteryLevel { get; set; }

        public DataResult()
        {
            this.Date = DateTime.Now;
        }

        public void SetDataCalibrationByColunms(string[] Colunas, decimal FatAD)
        {
            int _Angle = 0;
            decimal _dAngle = 0; 

            decimal _Torque = 0;
            decimal _BatteryLevel = 0;

            if (Colunas.Length < 4)
                return;

            this.Type = "CA";

            try
            {
                if (decimal.TryParse(Colunas[4], out  _dAngle))
                    this.Angle = Math.Round(_dAngle, 4); 
            }
            catch {
            }
            if (_dAngle == 0)
            {
                try
                {
                    if (int.TryParse(Colunas[4], out  _Angle)) 
                        this.Angle = _Angle;
                }
                catch { }
            }
            if ((int)this.Angle == 32640)                
            {
                this.Angle = 0;
            }           
            decimal.TryParse(Colunas[2].Trim(), out _Torque);
            if (FatAD>0)
                this.Torque = Math.Round(_Torque / FatAD, 4);                


            decimal.TryParse(Colunas[5].Trim(), out _BatteryLevel);
            this.BatteryLevel = _BatteryLevel / 100;
        }

        public void SetDataTestVerificationByColunms(string[] Colunas, decimal FatAD)
        {
            int _Angle = 0;
            decimal _dAngle = 0; 
            decimal _Torque=0;
       

            if (Colunas.Length < 6)
                return;

            this.Type = "TV";
            try
            {
                if (decimal.TryParse(Colunas[5], out  _dAngle))
                    this.Angle = Math.Round(_dAngle, 4);
            }
            catch
            {
            }
            if (_dAngle == 0)
            {
                try
                {
                    if (int.TryParse(Colunas[5], out  _Angle)) 
                        this.Angle = _Angle;
                }
                catch { }
            }
            if ((int)this.Angle == 32640)                
            {
                this.Angle = 0;
            }
            decimal.TryParse(Colunas[2].Trim(), out _Torque);
            if (FatAD > 0)
                this.Torque = Math.Round(_Torque / FatAD, 4);                

            
        }

        public void SetDataFinalResultByColunms(string[] Colunas, decimal FatAD)
        {
            int _Angle = 0;
            decimal _dAngle = 0; 
            decimal _Torque = 0;

            if (Colunas.Length < 2)
                return;

            this.Type = "FR";

            try
            {
                if (decimal.TryParse(Colunas[2], out  _dAngle))
                    this.Angle = Math.Round(_dAngle, 4);

            }
            catch
            {
            }
            if (_dAngle == 0)
            {
                try
                {
                    if (int.TryParse(Colunas[2], out  _Angle)) 
                        this.Angle = _Angle;
                }
                catch { }
            }
            if ((int)this.Angle == 32640)                
            {
                this.Angle = 0;
            }
            decimal.TryParse(Colunas[1].Trim(), out _Torque);
            if (FatAD>0)
                this.Torque = Math.Round(_Torque / FatAD, 4);                


        }

    }
}
