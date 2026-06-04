namespace SmartHome.Shared.Models
{
    public class LightSensorData : MetaData
    {
        public decimal AnalogueValue { get; set; } 

        public int DigitalValue { get; set; }
    }
}
