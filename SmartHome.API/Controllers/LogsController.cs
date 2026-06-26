using Microsoft.AspNetCore.Mvc;
using SmartHome.API.Shared.Models.Logs;
using SmartHome.API.Shared.Repos.Interfaces;

namespace SmartHome.API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class LogsController : RepoController<LogEntry>
    {
        public LogsController(ILogsRepo repo, ILogger<LogsController> logger) : base(repo, logger) { }      
    }
}
