using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared;
using SmartHome.API.Shared.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class MetaDataController :  ControllerBase
    {
        private readonly IDateTimeProvider _dateTimeProvider;
        private readonly ILogger<MetaDataController> _logger;

        public MetaDataController(IDateTimeProvider dateTimeProvider, ILogger<MetaDataController> logger)
        {
            _dateTimeProvider = dateTimeProvider;
            _logger = logger;
        }

        [HttpGet("CorrelationId")]
        public Guid GetCorrelationId()
        {
            var correlationId = Guid.NewGuid(); 
            _logger.LogInformation("Generated correlation ID: {CorrelationId}, timestamp UTC: {DateTimeUtc}", correlationId, _dateTimeProvider.Utc.ToString("yyyy-MM-dd HH:mm:ss"));
            return correlationId;
        }

        [HttpGet("DateTimeUtc")]
        public string GetDateTimeUtc()
        {
            var dateTimeUtcNowResponse = _dateTimeProvider.Utc.ToString("yyyy-MM-dd HH:mm:ss");
            _logger.LogInformation("Retrieved UTC date/time: {DateTimeUtc}, timestampUTC : {DateTimeUtc}", dateTimeUtcNowResponse, _dateTimeProvider.Utc.ToString("yyyy-MM-dd HH:mm:ss"));
            return dateTimeUtcNowResponse;
        }

        [HttpGet("diag")]
        public IActionResult Diag([FromServices] AppDbContext ctx)
        {
            var conn = ctx.Database.GetDbConnection().ConnectionString;
            var batteryCount = ctx.BatteryStates.Count();
            var tempCount = ctx.TemperatureData.Count();
            var lightCount = ctx.LightSensorData.Count();
            _logger.LogInformation("Diagnostic info - Connection: {Connection}, Battery: {BatteryCount}, Temperature: {TempCount}, Light: {LightCount}", conn, batteryCount, tempCount, lightCount);
            return Ok(new { conn, batteryCount, tempCount, lightCount });
        }
    }
}
