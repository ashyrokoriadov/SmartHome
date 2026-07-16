using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class AirPolutionDataController : RepoController<AirPolutionData>
    {
        public AirPolutionDataController(IAirPolutionDataRepoRepo repo, ILogger<AirPolutionDataController> logger) : base(repo, logger) { }
    }
}
