using SmartHome.Shared.Models;
using SmartHome.Shared.Repos.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public class BatteryStateRepo : DataRepo<BatteryState>, IBatteryStateRepo
    {
        public BatteryStateRepo(AppDbContext context) : base(context) { }


        public override async Task<IEnumerable<BatteryState>> GetAllAsync()
        {
            return await Task.FromResult(Context.BatteryStates.OrderByDescending(b => b.Timestamp).ToList());
        }
    }
}
