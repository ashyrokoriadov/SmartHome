using Microsoft.EntityFrameworkCore;
using SmartHome.Shared.Models;

using SmartHome.Shared.Repos.Interfaces;
namespace SmartHome.API.Shared.Repos
{
    public class BatteryStateRepo : DataRepo<ElectricalSensor>, IBatteryStateRepo
    {
        public BatteryStateRepo(AppDbContext context) : base(context) { }


        public override async Task<IEnumerable<ElectricalSensor>> GetAllAsync()
        {
            return await Task.FromResult(Context.BatteryStates.OrderByDescending(b => b.Timestamp).ToList());
        }

        public override async Task<IEnumerable<ElectricalSensor>> GetRangeAsync(DateTime start, DateTime end, int take)
        {
            if (end < start) throw new ArgumentException("End must be after start");
            if ((end - start).TotalDays > 30) throw new ArgumentException("Range cannot exceed 30 days");

            return await Context.BatteryStates
                .Where(d => d.Timestamp >= start && d.Timestamp <= end)
                .OrderByDescending(d => d.Timestamp)
                .Take(take)
                .ToListAsync();
        }
    }
}
