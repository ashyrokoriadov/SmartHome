using SmartHome.Shared.Models;

namespace SmartHome.Shared.Interfaces
{
    public interface IRepo<T>
    {
        Task<IEnumerable<T>> GetAllAsync();

        Task AddAsync(T entry);

        Task AddRangeAsync(IEnumerable<T> entries);

        Task<IEnumerable<T>> FilterAsync(Predicate<T> predicate);

        Task<IEnumerable<T>> GetRangeAsync(DateTime start, DateTime end, int take);
    }
}
