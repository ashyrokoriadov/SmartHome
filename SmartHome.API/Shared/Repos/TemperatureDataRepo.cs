using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Constants;
using SmartHome.API.Shared.Interfaces;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public class TemperatureDataRepo : DataRepo<TemperatureData>, ITemperatureDataRepo
    {
        public TemperatureDataRepo(AppDbContext context, IInfluxClient influxClient) : base(context, influxClient) { }

        protected override string MeasurementName => MeasurementTypes.Temperature;

        protected override string BucketName => InfluxBuckets.Temperature;

        public override async Task<IEnumerable<TemperatureData>> GetAllAsync()
        {
            return await Task.FromResult(Context.TemperatureData.OrderByDescending(t => t.Timestamp).ToList());
        }

        public override async Task<IEnumerable<TemperatureData>> GetRangeAsync(DateTime start, DateTime end, int take)
        {
            if (end < start) throw new ArgumentException("End must be after start");
            if ((end - start).TotalDays > 30) throw new ArgumentException("Range cannot exceed 30 days");

            return await Context.TemperatureData
                .Where(d => d.Timestamp >= start && d.Timestamp <= end)                
                .OrderByDescending(d => d.Timestamp)
                .Take(take)
                .ToListAsync();
        }

        protected override Dictionary<string, object> GetValuesFromEntry(TemperatureData entry)
        {
            return new Dictionary<string, object>
            {
                { "Temperature", entry.Temperature }
            };
        }
    }
}
