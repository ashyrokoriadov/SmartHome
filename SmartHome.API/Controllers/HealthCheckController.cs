using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Options;
using SmartHome.API.Options;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class HealthCheckController : ControllerBase
    {

        private readonly ConnectionStrings _connectionStrings;
        private readonly ILogger<HealthCheckController> _logger;

        public HealthCheckController(IOptions<ConnectionStrings> options, ILogger<HealthCheckController> logger)
        {
            _connectionStrings = options.Value;
            _logger = logger;
        }


        [HttpGet]
        public IActionResult Get()
        {
            string dbPath = _connectionStrings.SqlLightConnection.Replace("Data Source=", "");
            if (System.IO.File.Exists(dbPath))
            {
                _logger.LogInformation("Health check passed. DB file found: {DbPath}", dbPath);
                return Ok("Healthy");
            }
            _logger.LogWarning("Health check failed. DB file not found: {DbPath}", dbPath); 
            return NotFound($"DB file not found: {dbPath}");
        }
    }
}
