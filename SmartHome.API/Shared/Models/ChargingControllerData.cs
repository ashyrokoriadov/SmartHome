using SmartHome.API.Shared.Models;

namespace SmartHome.Shared.Models
{
    public class ChargingControllerData : MetaData
    {
        public string Name { get; set; } = string.Empty;

        public string Value { get; set; } = string.Empty;

        public string DataType { get; set; } = string.Empty;

        public override string ToString()
        {
            return $"{nameof(CorrelationId)}: {CorrelationId}, {nameof(Timestamp)}: {Timestamp:yyyy-MM-dd HH:mm:ss}," +
                $", {nameof(Name)}: {Name}, {nameof(Value)}: {Value}, {nameof(DataType)}: {DataType}.";
        }
    }
}
