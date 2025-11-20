using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ITransducers
{
    public class DataInformation
    {

        public string KeyName { get; set; }
        public int TorqueLimit { get; set; }
        public int FullScale { get; set; }
        public int PowerType { get; set; }
        public int AutoPowerOff { get; set; }

        public double TorqueConversionFactor { get; set; }
        public double AngleConversionFactor { get; set; }

        public string Model { get; set; }
        public string HW { get; set; }
        public string FW { get; set; }
        public string HardID { get; set; }


        public string DeviceType { get; set; }

        public int CommunicationType { get; set; }

        public void SetDataInformationByColunms(string[] Colunas)
        {
            int Value = 0;
            this.KeyName = Colunas[1].Trim(); 
            this.TorqueLimit = int.Parse(Colunas[2].Trim());
            this.FullScale = int.Parse(Colunas[3].Trim());
            this.DeviceType = Colunas[6].Trim();
            if(Colunas.Length > 9)
                this.PowerType = int.Parse(Colunas[9].Trim());
            if (Colunas.Length > 10)
                this.AutoPowerOff = int.Parse(Colunas[10].Trim());
            if (Colunas.Length > 11)
                this.CommunicationType = int.Parse(Colunas[11].Trim());
            if (Colunas.Length > 12) 
            {
                this.TorqueConversionFactor = double.Parse(Colunas[12].Trim());
                this.AngleConversionFactor = double.Parse(Colunas[13].Trim());
                this.Model = Colunas[14].Trim();
                this.HW = Colunas[15].Trim();
                this.FW = Colunas[16].Trim();
                if (Colunas.Length > 17) 
                    this.HardID = Colunas[17].Trim();
                else
                    this.HardID = this.KeyName;
            }
            else
            {
                this.TorqueConversionFactor = 1;
                this.AngleConversionFactor = 1;
                this.Model = "";
                this.HW = "";
                this.FW = "";
                this.HardID = this.KeyName;      
            }

        }


    }
}
