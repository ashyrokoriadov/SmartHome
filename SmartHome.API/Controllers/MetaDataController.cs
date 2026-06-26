using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class MetaDataController :  ControllerBase
    {
        private readonly IDateTimeProvider _dateTimeProvider;

        public MetaDataController(IDateTimeProvider dateTimeProvider)
        {
            _dateTimeProvider = dateTimeProvider;
        }

        [HttpGet("CorrelationId")]
        public Guid GetCorrelationId()
        {
            return Guid.NewGuid();
        }

        [HttpGet("DateTimeUtc")]
        public string GetDateTimeUtc()
        {
            return _dateTimeProvider.Utc.ToString("yyyy-MM-dd HH:mm:ss");
        }

        [HttpGet("diag")]
        public IActionResult Diag([FromServices] AppDbContext ctx)
        {
            var conn = ctx.Database.GetDbConnection().ConnectionString;
            var batteryCount = ctx.BatteryStates.Count();
            var tempCount = ctx.TemperatureData.Count();
            var lightCount = ctx.LightSensorData.Count();
            return Ok(new { conn, batteryCount, tempCount, lightCount });
        }
    }
}
