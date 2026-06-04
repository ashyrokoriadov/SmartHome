using SmartHome.API.Shared.Interfaces;

namespace SmartHome.API.Shared
{
    public class DateTimeProvider : IDateTimeProvider
    {
        public DateTime Utc => DateTime.UtcNow;

        public DateTime Local => DateTime.Now;
    }
}
