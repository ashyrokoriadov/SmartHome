namespace SmartHome.Shared.Models
{
    public class MetaData
    {
        public Guid Id { get; set; } = Guid.NewGuid();

        public Guid CorrelationId { get; set; } = Guid.Empty;

        public DateTime Timestamp { get; set; } = DateTime.UtcNow;

        public string Location { get; set; } = "Balcony";
    }
}
