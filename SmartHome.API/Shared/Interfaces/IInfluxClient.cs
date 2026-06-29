namespace SmartHome.API.Shared.Interfaces
{
    public interface IInfluxClient
    {
        Task Write(string bucket, string measurement, Dictionary<string, object> fields);
    }
}
