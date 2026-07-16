namespace SmartHome.Shared.Models
{
    public class AirQualityData : MetaData
    {
        public decimal Temperature { get; set; }

        public decimal TemperatureExternal { get; set; }

        public decimal Humidity { get; set; }

        public decimal Pressure { get; set; }

        public decimal Altitude { get; set; }

        public override string ToString()
        {
            return $"{nameof(CorrelationId)}: {CorrelationId}, {nameof(Timestamp)}: {Timestamp:yyyy-MM-dd HH:mm:ss}, {nameof(Temperature)}: {Temperature}," +
                $" {nameof(TemperatureExternal)}: {TemperatureExternal}, {nameof(Humidity)}: {Humidity}, {nameof(Pressure)}: {Pressure}, {nameof(Altitude)}: {Altitude}.";
        }
    }
}
