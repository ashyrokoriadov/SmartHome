namespace SmartHome.API.Options
{
    public class InfluxOptions
    {
        public static string Section => nameof(InfluxOptions);

        public string Url { get; set; } = "http://localhost:8086";

        public string Token { get; set; } = string.Empty;
    }
}
