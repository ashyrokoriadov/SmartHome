using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class AirQualityDataController : RepoController<AirQualityData>
    {
        public AirQualityDataController(IAirQualityDataRepoRepo repo, ILogger<AirQualityDataController> logger) : base(repo, logger) { }       
    }
}
