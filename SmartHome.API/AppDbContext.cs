using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Models.Logs;
using SmartHome.Shared.Models;

namespace SmartHome.API
{
    public class AppDbContext : DbContext
    {
        public DbSet<ChargingControllerData> BatteryStates { get; set; }

        public DbSet<LightSensorData> LightSensorData { get; set; }

        public DbSet<AirQualityData> AirQualityData { get; set; }

        public DbSet<AirPolutionData> AirPolutionData { get; set; }

        public DbSet<LogEntry> Logs { get; set; }   

        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }
    }
}
