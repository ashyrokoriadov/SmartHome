namespace SmartHome.Shared.Models
{
    public class AirPolutionData : MetaData
    {
        public int ECO2 { get; set; }

        public int TVOC { get; set; }

        public int AQI { get; set; }

        public int SensorStatus { get; set; }

        public override string ToString()
        {
            return $"{nameof(CorrelationId)}: {CorrelationId}, {nameof(Timestamp)}: {Timestamp:yyyy-MM-dd HH:mm:ss}, {nameof(ECO2)}: {ECO2}," +
                $" {nameof(TVOC)}: {TVOC}, {nameof(AQI)}: {AQI}, {nameof(SensorStatus)}: {SensorStatus}.";
        }
    }
}
