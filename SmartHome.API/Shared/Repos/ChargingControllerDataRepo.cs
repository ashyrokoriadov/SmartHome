using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Constants;
using SmartHome.API.Shared.Interfaces;
using SmartHome.API.Shared.Models;
using SmartHome.Shared.Models;

using SmartHome.Shared.Repos.Interfaces;
using System.Globalization;
namespace SmartHome.API.Shared.Repos
{
    public class ChargingControllerDataRepo : DataRepo<ChargingControllerData>, IBatteryStateRepo
    {
        public ChargingControllerDataRepo(AppDbContext context, IInfluxClient influxClient) : base(context, influxClient) { }

        private string _measurementName { get; set; } = MeasurementTypes.Electricity;
        protected override string MeasurementName => _measurementName;

        protected override string BucketName => InfluxBuckets.Victron;

        public override async Task AddAsync(ChargingControllerData entry)
        {
            ArgumentNullException.ThrowIfNull(entry);

            _measurementName = entry.MeasurementType ?? MeasurementTypes.Electricity;

            if (entry.Name == "Unknown" || string.IsNullOrEmpty(entry.Name))
            {
                return;
            }

            Context.Add(entry);
            await Context.SaveChangesAsync();
            await WriteToInfluxAsync(entry);
        }

        public override async Task AddRangeAsync(IEnumerable<ChargingControllerData> entries)
        {
            if (entries == null || !entries.Any())
            {
                throw new ArgumentNullException(nameof(entries));
            }

            foreach (var entry in entries)
            {
                if (entry == null)
                {
                    throw new ArgumentException("One of the items in the collection is null.", nameof(entries));
                }

                if (entry.Name == "Unknown" || string.IsNullOrEmpty(entry.Name))
                {
                    continue;
                }                

                Context.Add(entry);
            }
            await Context.SaveChangesAsync();
            foreach (var entry in entries)
            {
                _measurementName = entry.MeasurementType ?? MeasurementTypes.Electricity;
                await WriteToInfluxAsync(entry);
            }
        }


        public override async Task<IEnumerable<ChargingControllerData>> GetAllAsync()
        {
            return await Task.FromResult(Context.BatteryStates.OrderByDescending(b => b.Timestamp).ToList());
        }

        public override async Task<IEnumerable<ChargingControllerData>> GetRangeAsync(DateTime start, DateTime end, int take)
        {
            if (end < start) throw new ArgumentException("End must be after start");
            if ((end - start).TotalDays > 30) throw new ArgumentException("Range cannot exceed 30 days");

            return await Context.BatteryStates
                .Where(d => d.Timestamp >= start && d.Timestamp <= end)
                .OrderByDescending(d => d.Timestamp)
                .Take(take)
                .ToListAsync();
        }

        protected override Dictionary<string, object> GetValuesFromEntry(ChargingControllerData entry)
        {
            switch (entry.DataType)
            {
                case "Decimal":
                    if (string.IsNullOrWhiteSpace(entry.Value))
                    {
                        entry.Value = "0";
                    }

                    if (decimal.TryParse(entry.Value, NumberStyles.Number, CultureInfo.InvariantCulture, out decimal decimalValue))
                    {
                        return new Dictionary<string, object>
                        {
                            { "Name", entry.Name },
                            { "Value", decimalValue }
                        };
                    }
                    else
                    {
                        return new Dictionary<string, object>
                        {
                            { "Name", entry.Name },
                            { "Value", 0M }
                        };
                    }
                case "ChargerState":
                    if (Enum.TryParse(entry.Value, out ChargerState state))
                    {
                        return new Dictionary<string, object>
                        {
                            { "Name", entry.Name },
                            { "Value", entry.Value }
                        };
                    }
                    else
                    {
                        return new Dictionary<string, object>
                        {
                            { "Name", entry.Name },
                            { "Value", ChargerState.Reserve3 }
                        };
                    }
                case "String":
                default:
                    return new Dictionary<string, object>
                {
                    { "Name", entry.Name },
                    { "Value", entry.Value }
                };
            }
        }
    }
}
