using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public class LightSensorDataRepo : DataRepo<LightSensorData>, ILightSensorDataRepo
    {
        public LightSensorDataRepo(AppDbContext context) : base(context) { }


        public override async Task<IEnumerable<LightSensorData>> GetAllAsync()
        {
            return await Task.FromResult(Context.LightSensorData.OrderByDescending(b => b.Timestamp).ToList());
        }
    }
}
