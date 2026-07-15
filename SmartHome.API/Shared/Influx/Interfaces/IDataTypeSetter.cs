using SmartHome.Shared.Models;

namespace SmartHome.API.Shared.Influx.Interfaces
{
    public interface IDataTypeSetter
    {
        void SetDataType(ChargingControllerData entry);
    }
}
