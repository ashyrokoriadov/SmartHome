using Microsoft.EntityFrameworkCore;
using SmartHome.Shared.Models;

namespace SmartHome.API
{
    public class AppDbContext : DbContext
    {
        public DbSet<BatteryState> BatteryStates { get; set; }

        public DbSet<LightSensorData> LightSensorData { get; set; }

        public DbSet<TemperatureData> TemperatureData { get; set; }

        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }
    }
}
