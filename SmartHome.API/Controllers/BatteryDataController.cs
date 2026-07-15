using Microsoft.AspNetCore.Mvc;
using SmartHome.API.Shared.Influx.Interfaces;
using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class BatteryDataController : RepoController<ChargingControllerData>
    {
        private readonly IDataTypeSetter _dataTypeSetter;
        public BatteryDataController(IBatteryStateRepo repo, ILogger<BatteryDataController> logger, IDataTypeSetter dataTypeSetter) : base(repo, logger) 
        {
            _dataTypeSetter = dataTypeSetter;
        }

        [HttpPost("Add")]
        public override async Task Add([FromBody] ChargingControllerData entity)
        {
            _dataTypeSetter.SetDataType(entity);

            await base.Add(entity);
        }

        [HttpPost("AddRange")]
        public override async Task AddRange([FromBody] IEnumerable<ChargingControllerData> entities)
        {
            foreach (var entity in entities)
            {
                _dataTypeSetter.SetDataType(entity);
            }

            await base.AddRange(entities);
        }
    }
}
