namespace SmartHome.API.Shared.Models.Logs
{
    public class LogEntry
    {
        public Guid Id { get; set; } = Guid.NewGuid();

        public DateTime Timestamp { get; set; } = DateTime.UtcNow;

        public LogLevel Level { get; set; } = LogLevel.Info;

        public string Message { get; set; } = string.Empty;

        public string Source { get; set; } = "Arduino";
    }
}
