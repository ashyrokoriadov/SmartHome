using SmartHome.Shared.Interfaces;

namespace SmartHome.API.Shared.Repos
{
    public abstract class DataRepo<T> : IRepo<T>
    {
        protected readonly AppDbContext Context;

        public DataRepo(AppDbContext context)
        {
            Context = context;
        }

        public async Task AddAsync(T entry)
        {
            if (entry == null)
            {
                throw new ArgumentNullException(nameof(entry));
            }

            Context.Add(entry);
            await Context.SaveChangesAsync();
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
        }

        public Task<IEnumerable<T>> FilterAsync(Predicate<T> predicate)
        {
            throw new NotImplementedException();
        }

        public abstract Task<IEnumerable<T>> GetAllAsync();
    }
}
