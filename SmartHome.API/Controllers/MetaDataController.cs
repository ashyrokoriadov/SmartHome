using Microsoft.AspNetCore.Mvc;
using SmartHome.API.Shared.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class MetaDataController :  ControllerBase
    {
        private readonly IDateTimeProvider _dateTimeProvider;

        public MetaDataController(IDateTimeProvider dateTimeProvider)
        {
            _dateTimeProvider = dateTimeProvider;
        }

        [HttpGet("CorrelationId")]
        public Guid GetCorrelationId()
        {
            return Guid.NewGuid();
        }

        [HttpGet("DateTimeUtc")]
        public DateTime GetDateTimeUtc()
        {
            return _dateTimeProvider.Utc;
        }
    }
}
