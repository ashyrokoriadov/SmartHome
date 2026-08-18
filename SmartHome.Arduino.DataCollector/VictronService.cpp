#include "VictronService.h"
#include "Config.h"

VictronService::VictronService(ClockService& clock)
    : clockService(clock),
      serial(Serial1),
      pvVoltageValue(0.0f),
      totalEnergyValue(0.0f),
      lastReadMs(0)
{
}

bool VictronService::begin()
{
    pinMode(VICTRON_RX_PIN, INPUT);
    serial.begin(19200);
    return true;
}

void VictronService::update()
{
    while (serial.available()) {
        const char incoming = static_cast<char>(serial.read());
        String current = String(incoming);
        if (current == "\n" || current == "\r") {
            continue;
        }

        String line = current;
        while (serial.available()) {
            char next = static_cast<char>(serial.read());
            if (next == '\n' || next == '\r') {
                break;
            }
            line += next;
        }

        VictronMeasurement measurement = VictronParser::parse(line);
        if (measurement.name == "pv_voltage") {
            pvVoltageValue = measurement.value;
        }
        else if (measurement.name == "total_energy") {
            totalEnergyValue = measurement.value;
        }
    }
}

float VictronService::pvVoltage() const
{
    return pvVoltageValue;
}

float VictronService::totalEnergy() const
{
    return totalEnergyValue;
}
