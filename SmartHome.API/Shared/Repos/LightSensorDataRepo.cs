using Microsoft.EntityFrameworkCore;
using SmartHome.Shared.Repos.Interfaces;
using SmartHome.Shared.Models;

namespace SmartHome.API.Shared.Repos
{
    public class LightSensorDataRepo : DataRepo<LightSensorData>, ILightSensorDataRepo
    {
        public LightSensorDataRepo(AppDbContext context) : base(context) { }

        public override async Task<IEnumerable<LightSensorData>> GetAllAsync()
        {
            return await Task.FromResult(Context.LightSensorData.OrderByDescending(l => l.Timestamp).ToList());
        }

        public override async Task<IEnumerable<LightSensorData>> GetRangeAsync(DateTime start, DateTime end, int take)
        {
            // Ensure range does not exceed 30 days
            if (end < start) throw new ArgumentException("End must be after start");
            if ((end - start).TotalDays > 30) throw new ArgumentException("Range cannot exceed 30 days");

            return await Context.LightSensorData
                .Where(d => d.Timestamp >= start && d.Timestamp <= end)                
                .OrderByDescending(d => d.Timestamp)
                .Take(take)
                .ToListAsync();
        }
    }
}
