using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class LightSensorController :  RepoController<LightSensorData>
    {
        public LightSensorController(ILightSensorDataRepo repo, ILogger<LightSensorController> logger) : base(repo, logger) { }        
    }
}
