using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class BatteryDataController : RepoController<ChargingControllerData>
    {
        public BatteryDataController(IBatteryStateRepo repo, ILogger<BatteryDataController> logger) : base(repo, logger) { }      
    }
}
