using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class BatteryController : RepoController<BatteryState>
    {
        public BatteryController(IBatteryStateRepo repo) : base(repo) { }      
    }
}
