namespace SmartHome.Shared.Models
{
    public class BatteryState : MetaData
    {
        public decimal Voltage { get; set; }

        public decimal Current { get; set; }        

        public decimal Power  => Voltage * Current;       
    }
}
