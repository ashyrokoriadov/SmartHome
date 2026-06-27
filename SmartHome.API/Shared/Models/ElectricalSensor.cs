using SmartHome.API.Shared.Models;

namespace SmartHome.Shared.Models
{
    public class ElectricalSensor : MetaData
    {
        public decimal Voltage { get; set; }

        public decimal Current { get; set; }

        public decimal Power => Voltage * Current;

        public decimal PanelVoltage { get; set; }

        public decimal PanelPower { get; set; }

        public ChargerState ChargerState { get; set; }

        public int Error { get; set; }

        public decimal GainedEnergyToday { get; set; }

        public override string ToString()
        {
            return $"{nameof(CorrelationId)}: {CorrelationId}, {nameof(Timestamp)}: {Timestamp:yyyy-MM-dd HH:mm:ss}, {nameof(Voltage)}: {Voltage}, {nameof(Current)}: {Current}, {nameof(Power)}: {Power}" +
                $", {nameof(PanelVoltage)}: {PanelVoltage}, {nameof(PanelPower)}: {PanelPower}, {nameof(ChargerState)}: {ChargerState}, {nameof(GainedEnergyToday)}: {GainedEnergyToday}";
        }
    }
}
