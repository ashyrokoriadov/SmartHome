using InfluxDB.Client;
using InfluxDB.Client.Api.Domain;
using InfluxDB.Client.Writes;
using Microsoft.Extensions.Options;
using SmartHome.API.Options;
using SmartHome.API.Shared.Interfaces;

namespace SmartHome.API.Shared.Influx
{
    public class InfluxClient : IInfluxClient
    {
        private readonly InfluxOptions _influxOptions;
        private readonly IDateTimeProvider _dateTimeProvider;
        private readonly string _organisation = "SmartHome";
        private IInfluxDBClient _client;

        public InfluxClient(IOptions<InfluxOptions> influxOptions, IDateTimeProvider dateTimeProvider)
        {
            _influxOptions = influxOptions.Value;
            _dateTimeProvider = dateTimeProvider;
        }

        public Task Write(string bucket, string measurement, Dictionary<string, object> fields)
        {
            var options = new InfluxDBClientOptions.Builder()
                .Url(_influxOptions.Url)
                .AuthenticateToken(_influxOptions.Token.ToCharArray())
                .Build();

            _client = new InfluxDBClient(options);

            using var writeApi = _client.GetWriteApi();

            var point = PointData.Measurement(measurement)
                .Tag("device", "arduino")
                .Timestamp(_dateTimeProvider.Utc, WritePrecision.Ns);

            foreach (var field in fields)
            {
                point = point.Field(field.Key, field.Value);
            }

            writeApi.WritePoint(point, bucket, _organisation);

            return Task.CompletedTask;
        }
    }
}
