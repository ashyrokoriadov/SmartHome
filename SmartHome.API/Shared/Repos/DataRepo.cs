using SmartHome.API.Shared.Interfaces;
using SmartHome.Shared.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public abstract class DataRepo<T> : IRepo<T>
    {
        protected readonly AppDbContext Context;
        protected readonly IInfluxClient InfluxClient;

        protected abstract string MeasurementName { get; }

        protected abstract string BucketName { get; }

        protected virtual bool InfluxEnabled => true;

        public DataRepo(AppDbContext context, IInfluxClient influxClient)
        {
            Context = context;
            InfluxClient = influxClient;
        }

        public async Task AddAsync(T entry)
        {
            if (entry == null)
            {
                throw new ArgumentNullException(nameof(entry));
            }

            Context.Add(entry);
            await Context.SaveChangesAsync();
            await WriteToInfluxAsync(entry);
        }

        public async Task AddRangeAsync(IEnumerable<T> entries)
        {
            if (entries == null || !entries.Any())
            {
                throw new ArgumentNullException(nameof(entries));
            }

            foreach (var entry in entries)
            {
                if (entry == null)
                {
                    throw new ArgumentNullException(nameof(entry));
                }

                Context.Add(entry);
            }
            await Context.SaveChangesAsync();
            foreach (var entry in entries)
            {
                await WriteToInfluxAsync(entry);
            }
        }

        public Task<IEnumerable<T>> FilterAsync(Predicate<T> predicate)
        {
            throw new NotImplementedException();
        }

        public abstract Task<IEnumerable<T>> GetAllAsync();

        public abstract Task<IEnumerable<T>> GetRangeAsync(DateTime start, DateTime end, int take);

        protected virtual async Task WriteToInfluxAsync(T entry)
        {
            if (InfluxEnabled)
            {
                var values = GetValuesFromEntry(entry);
                await InfluxClient.Write(BucketName, MeasurementName, values);
            }
        }

        protected abstract Dictionary<string, object> GetValuesFromEntry(T entry);
    }
}
