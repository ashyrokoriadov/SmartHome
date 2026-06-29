using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Constants;
using SmartHome.API.Shared.Interfaces;
using SmartHome.Shared.Models;

using SmartHome.Shared.Repos.Interfaces;
namespace SmartHome.API.Shared.Repos
{
    public class BatteryStateRepo : DataRepo<ElectricalSensor>, IBatteryStateRepo
    {
        public BatteryStateRepo(AppDbContext context, IInfluxClient influxClient) : base(context, influxClient) { }

        protected override string MeasurementName => MeasurementTypes.Electricity;

        protected override string BucketName => InfluxBuckets.Electricity;


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

        protected override Dictionary<string, object> GetValuesFromEntry(ElectricalSensor entry)
        {
            return new Dictionary<string, object>
            {
                { "Voltage", entry.Voltage },
                { "Current", entry.Current },
                { "Power", entry.Power },
                { "PanelVoltage", entry.PanelVoltage },
                { "PanelPower", entry.PanelPower },
                { "GainedEnergyToday", entry.GainedEnergyToday }
            };           
        }
    }
}
