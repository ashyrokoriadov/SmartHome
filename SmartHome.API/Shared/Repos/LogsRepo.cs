using Microsoft.EntityFrameworkCore;
using SmartHome.API.Shared.Models.Logs;
using SmartHome.API.Shared.Repos.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public class LogsRepo : DataRepo<LogEntry>, ILogsRepo
    {
        public LogsRepo(AppDbContext context) : base(context) { }

        public override async Task<IEnumerable<LogEntry>> GetAllAsync()
        {
            return await Task.FromResult(Context.Logs.OrderByDescending(l => l.Timestamp).ToList());
        }

        public override async Task<IEnumerable<LogEntry>> GetRangeAsync(DateTime start, DateTime end, int take)
        {
            if (end < start) throw new ArgumentException("End must be after start");
            if ((end - start).TotalDays > 30) throw new ArgumentException("Range cannot exceed 30 days");

            return await Context.Logs
                .Where(l => l.Timestamp >= start && l.Timestamp <= end)
                .OrderByDescending(l => l.Timestamp)
                .Take(take)
                .ToListAsync();
        }
    }
}
