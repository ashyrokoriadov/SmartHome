using SmartHome.API.Shared.Constants;
using SmartHome.API.Shared.Influx.Interfaces;
using SmartHome.Shared.Models;

namespace SmartHome.API.Shared.Influx
{
    public class DataTypeSetter : IDataTypeSetter
    {
        public void SetDataType(ChargingControllerData entry)
        {
            switch (entry.Name)
            {
                case "Voltage":
                    entry.MeasurementType = MeasurementTypes.Voltage;
                    entry.DataType = "Decimal";
                    break;
                case "PanelVoltage":
                    entry.MeasurementType = MeasurementTypes.PanelVoltage;
                    entry.DataType = "Decimal";
                    break;
                case "PanelPower":
                    entry.MeasurementType = MeasurementTypes.PanelPower;
                    entry.DataType = "Decimal";
                    break;
                case "Current":
                    entry.MeasurementType = MeasurementTypes.Current;
                    entry.DataType = "Decimal";
                    break;
                case "GainedEnergy":
                    entry.MeasurementType = MeasurementTypes.GainedEnergy;
                    entry.DataType = "Decimal";
                    break;
                case "H19":
                    entry.MeasurementType = MeasurementTypes.H19;
                    entry.DataType = "Decimal";
                    break;
                case "ChargerState":
                    entry.MeasurementType = MeasurementTypes.ChargerState;
                    entry.DataType = "ChargerState";
                    break;
                case "Error":
                    entry.MeasurementType = MeasurementTypes.Error;
                    entry.DataType = "String";
                    break;
                case "ProductId":
                    entry.MeasurementType = MeasurementTypes.ProductId;
                    entry.DataType = "String";
                    break;
                case "SerialNumber":
                    entry.MeasurementType = MeasurementTypes.SerialNumber;
                    entry.DataType = "String";
                    break;
                case "Unknown":
                    entry.MeasurementType = MeasurementTypes.Electricity;
                    entry.DataType = "String";
                    break;
            }
        }
    }
}
