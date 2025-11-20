using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ITransducers
{
    public class CountersInformation
    {
        public ulong Cycles { get; set; }
        public ulong Overshuts { get; set; }
        public decimal HigherOvershut { get; set; }

        public ulong AdditionalCounter1 { get; set; }

        public ulong AdditionalCounter2 { get; set; }

        public void SetInformationByColunms(string[] Colunas)
        {
            try
            {
                this.Cycles = ulong.Parse(Colunas[0].Trim());
                this.Overshuts = ulong.Parse(Colunas[1].Trim());
                this.HigherOvershut = decimal.Parse(Colunas[2].Trim());
                this.AdditionalCounter1 = ulong.Parse(Colunas[3].Trim());
                this.AdditionalCounter2 = ulong.Parse(Colunas[4].Trim());
            }
            catch { }
        }
    }
}
