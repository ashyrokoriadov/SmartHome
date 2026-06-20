namespace SmartHome.Shared.Models
{
    public class BatteryState : MetaData
    {
        public decimal Voltage { get; set; }

        public decimal Current { get; set; }        

        public decimal Power  => Voltage * Current;

        public override string ToString()
        {
            return $"{nameof(CorrelationId)}: {CorrelationId}, {nameof(Timestamp)}: {Timestamp:yyyy-MM-dd HH:mm:ss}, {nameof(Voltage)}: {Voltage}, {nameof(Current)}: {Current}, {nameof(Power)}: {Power}";
        }
    }
}
