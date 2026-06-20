namespace SmartHome.Shared.Models
{
    public class TemperatureData : MetaData
    {
        public decimal Temperature { get; set; }

        public override string ToString()
        {
            return $"{nameof(CorrelationId)}: {CorrelationId}, {nameof(Timestamp)}: {Timestamp:yyyy-MM-dd HH:mm:ss}, {nameof(Temperature)}: {Temperature}";
        }
    }
}
