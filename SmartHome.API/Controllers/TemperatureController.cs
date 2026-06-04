using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class TemperatureController : RepoController<TemperatureData>
    {
        public TemperatureController(ITemperatureDataRepo repo) : base(repo) { }       
    }
}
