using Microsoft.AspNetCore.Mvc;
using SmartHome.Shared.Interfaces;

namespace SmartHome.API.Controllers
{
    public class RepoController<T> :  ControllerBase
    {
        protected readonly IRepo<T> Repo;

        public RepoController(IRepo<T> repo)
        {
            Repo = repo;
        }

        [HttpGet]
        public async Task<IEnumerable<T>> Get()
        {
            return await Repo.GetAllAsync();
        }

        [HttpPost("Add")]
        public async Task Add([FromBody] T battery)
        {
            await Repo.AddAsync(battery);
        }

        [HttpPost("AddRange")]
        public async Task AddRange([FromBody] IEnumerable<T> batteries)
        {
            await Repo.AddRangeAsync(batteries);
        }
    }
}
