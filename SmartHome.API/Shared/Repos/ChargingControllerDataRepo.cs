using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Constants;
using SmartHome.API.Shared.Interfaces;
using SmartHome.Shared.Models;

using SmartHome.Shared.Repos.Interfaces;
namespace SmartHome.API.Shared.Repos
{
    public class ChargingControllerDataRepo : DataRepo<ChargingControllerData>, IBatteryStateRepo
    {
        public ChargingControllerDataRepo(AppDbContext context, IInfluxClient influxClient) : base(context, influxClient) { }

        private string _measurementName { get; set; } = MeasurementTypes.Electricity;
        protected override string MeasurementName => _measurementName;

        protected override string BucketName => InfluxBuckets.Electricity;

        public override async Task AddAsync(ChargingControllerData entry)
        {
            ArgumentNullException.ThrowIfNull(entry);

            SetDataType(entry);

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

                SetDataType(entry);

                if (entry.Name == "Unknown" || string.IsNullOrEmpty(entry.Name))
                {
                    continue;
                }

                Context.Add(entry);
            }
            await Context.SaveChangesAsync();
            foreach (var entry in entries)
            {
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
            return new Dictionary<string, object>
            {
                { "Name", entry.Name },
                { "Value", entry.Value }
            };
        }

        private void SetDataType(ChargingControllerData entry)
        {
            switch (entry.Name)
            {
                case "Voltage":
                    _measurementName = MeasurementTypes.Voltage;
                    entry.DataType = "Decimal";
                    break;
                case "PanelVoltage":
                    _measurementName = MeasurementTypes.PanelVoltage;
                    entry.DataType = "Decimal";
                    break;
                case "PanelPower":
                    _measurementName = MeasurementTypes.PanelPower;
                    entry.DataType = "Decimal";
                    break;
                case "Current":
                    _measurementName = MeasurementTypes.Current;
                    entry.DataType = "Decimal";
                    break;
                case "GainedEnergy":
                    _measurementName = MeasurementTypes.GainedEnergy;
                    entry.DataType = "Decimal";
                    break;
                case "H19":
                    _measurementName = MeasurementTypes.H19;
                    entry.DataType = "Decimal";
                    break; 
                case "ChargerState":
                    _measurementName = MeasurementTypes.ChargerState;
                    entry.DataType = "ChargerState";                 
                    break;
                case "Error":
                    _measurementName = MeasurementTypes.Error;
                    entry.DataType = "String";
                    break;
                case "ProductId":
                    _measurementName = MeasurementTypes.ProductId;
                    entry.DataType = "String";
                    break;
                case "SerialNumber":
                    _measurementName = MeasurementTypes.SerialNumber;
                    entry.DataType = "String";
                    break;
                case "Unknown":
                    _measurementName = MeasurementTypes.Electricity;
                    entry.DataType = "String";
                    break;               
            }
        }
    }
}
