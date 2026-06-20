namespace SmartHome.Shared.Models
{
    public class LightSensorData : MetaData
    {
        public decimal AnalogueValue { get; set; } 

        public int DigitalValue { get; set; }

        public override string ToString()
        {
            return $"{nameof(CorrelationId)}: {CorrelationId}, {nameof(Timestamp)}: {Timestamp:yyyy-MM-dd HH:mm:ss}, {nameof(AnalogueValue)}: {AnalogueValue}, {nameof(DigitalValue)}: {DigitalValue}";
        }
    }
}
