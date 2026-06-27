using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class BatteryController : RepoController<ElectricalSensor>
    {
        public BatteryController(IBatteryStateRepo repo, ILogger<BatteryController> logger) : base(repo, logger) { }      
    }
}
