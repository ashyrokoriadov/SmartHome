namespace SmartHome.API.Options
{
    public class ConnectionStrings
    {
        public static string Section => nameof(ConnectionStrings);

        public string SqlLightConnection { get; set; } = "Data Source=D:\\Temp\\SmartHome.db";
    }
}
