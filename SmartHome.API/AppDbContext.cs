using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Models.Logs;
using SmartHome.Shared.Models;

namespace SmartHome.API
{
    public class AppDbContext : DbContext
    {
        public DbSet<BatteryState> BatteryStates { get; set; }

        public DbSet<LightSensorData> LightSensorData { get; set; }

        public DbSet<TemperatureData> TemperatureData { get; set; }

        public DbSet<LogEntry> Logs { get; set; }   

        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }
    }
}
