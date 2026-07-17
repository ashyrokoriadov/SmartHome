using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Constants;
using SmartHome.API.Shared.Interfaces;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public class AirPolutionDataRepo : DataRepo<AirPolutionData>, IAirPolutionDataRepoRepo
    {
        public AirPolutionDataRepo(AppDbContext context, IInfluxClient influxClient) : base(context, influxClient) { }

        protected override string MeasurementName => MeasurementTypes.AirPolutionData;

        protected override string BucketName => InfluxBuckets.Temperature;

        public override async Task<IEnumerable<AirPolutionData>> GetAllAsync()
        {
            return await Task.FromResult(Context.AirPolutionData.OrderByDescending(t => t.Timestamp).ToList());
        }

        public override async Task<IEnumerable<AirPolutionData>> GetRangeAsync(DateTime start, DateTime end, int take)
        {
            if (end < start) throw new ArgumentException("End must be after start");
            if ((end - start).TotalDays > 30) throw new ArgumentException("Range cannot exceed 30 days");

            return await Context.AirPolutionData
                .Where(d => d.Timestamp >= start && d.Timestamp <= end)                
                .OrderByDescending(d => d.Timestamp)
                .Take(take)
                .ToListAsync();
        }

        protected override Dictionary<string, object> GetValuesFromEntry(AirPolutionData entry)
        {
            return new Dictionary<string, object>
            {
                { "ECO2", entry.ECO2 },
                { "TVOC", entry.TVOC },
                { "AQI", entry.AQI },
            };
        }
    }
}
