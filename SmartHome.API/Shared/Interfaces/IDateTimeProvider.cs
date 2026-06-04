namespace SmartHome.API.Shared.Interfaces
{
    public interface IDateTimeProvider
    {
        DateTime Utc { get; }

        DateTime Local { get; }
    }
}
