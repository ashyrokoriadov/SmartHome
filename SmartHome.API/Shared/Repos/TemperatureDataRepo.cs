using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public class TemperatureDataRepo : DataRepo<TemperatureData>, ITemperatureDataRepo
    {
        public TemperatureDataRepo(AppDbContext context) : base(context) { }


        public override async Task<IEnumerable<TemperatureData>> GetAllAsync()
        {
            return await Task.FromResult(Context.TemperatureData.OrderByDescending(b => b.Timestamp).ToList());
        }
    }
}
