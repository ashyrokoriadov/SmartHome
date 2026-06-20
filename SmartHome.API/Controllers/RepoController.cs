using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Interfaces;

namespace SmartHome.API.Controllers
{
    public class RepoController<T> :  ControllerBase
    {
        protected readonly IRepo<T> Repo;
        protected readonly ILogger<RepoController<T>> Logger;

        public RepoController(IRepo<T> repo, ILogger<RepoController<T>> logger)
        {
            Repo = repo;
            Logger = logger;
        }

        [HttpGet]
        public async Task<IEnumerable<T>> Get()
        {
            return await Repo.GetAllAsync();
        }

        [HttpGet("Range")]
        public async Task<IEnumerable<T>> Get(DateTime start, DateTime end, int take = 10)
        {
            return await Repo.GetRangeAsync(start, end, take);
        }

        [HttpPost("Add")]
        public async Task Add([FromBody] T entity)
        {
            Logger.LogInformation("Adding new data: {Entity}", entity);
            await Repo.AddAsync(entity);
        }

        [HttpPost("AddRange")]
        public async Task AddRange([FromBody] IEnumerable<T> entities)
        {
            Logger.LogInformation("Adding new data: {Entities}", entities);
            await Repo.AddRangeAsync(entities);
        }
    }
}
