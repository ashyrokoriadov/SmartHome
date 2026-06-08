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


        public HealthCheckController(IOptions<ConnectionStrings> options)
        {
            _connectionStrings = options.Value;
        }


        [HttpGet]
        public IActionResult Get()
        {
            string dbPath = _connectionStrings.SqlLightConnection.Replace("Data Source=", "");
            if (System.IO.File.Exists(dbPath))
            {
                return Ok("Healthy");
            }

            return NotFound($"DB file not found: {dbPath}");
        }
    }
}
