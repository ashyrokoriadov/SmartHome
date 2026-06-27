using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Options;
using SmartHome.API.Options;
using SmartHome.API.Shared.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class HealthCheckController : ControllerBase
    {

        private readonly ConnectionStrings _connectionStrings;
        private readonly ILogger<HealthCheckController> _logger;
        private readonly IDateTimeProvider _dateTimeProvider;

        public HealthCheckController(IOptions<ConnectionStrings> options, ILogger<HealthCheckController> logger, IDateTimeProvider dateTimeProvider)
        {
            _connectionStrings = options.Value;
            _logger = logger;
            _dateTimeProvider = dateTimeProvider;
        }


        [HttpGet]
        public IActionResult Get()
        {
            string dbPath = _connectionStrings.SqlLightConnection.Replace("Data Source=", "");
            if (System.IO.File.Exists(dbPath))
            {
                _logger.LogInformation("Health check passed. DB file found: {DbPath}, timestamp UTC: {DateTimeUtc}", dbPath, _dateTimeProvider.Utc.ToString("yyyy-MM-dd HH:mm:ss") );
                return Ok("Healthy");
            }
            _logger.LogWarning("Health check failed. DB file not found: {DbPath}", dbPath); 
            return NotFound($"DB file not found: {dbPath}");
        }
    }
}
