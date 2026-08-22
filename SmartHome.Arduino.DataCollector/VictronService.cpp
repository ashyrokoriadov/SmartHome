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

VictronSerialReader victronSerialReader;

bool VictronService::begin()
{
    pinMode(VICTRON_RX_PIN, INPUT);
    serial.begin(19200);
    return true;
}

void VictronService::update()
{
    int counter = 0;

    while (Serial1.available())
    {
        byte b = Serial1.read();
        String token = (b == 'D') ? "D" : String(b, HEX);
        ParsedValue parsedValue = victronSerialReader.ReadByte(token);

        if (!parsedValue.IsEmpty()
            && parsedValue.Name != "Unknown"
            && parsedValue.Name != "ChargerState"
            && parsedValue.Name != "Error"
            && parsedValue.Name != "ProductId"
            && parsedValue.Name != "SerialNumber"            
            && parsedValue.Name != "Current"
            && parsedValue.Name != "PanelPower"
            && parsedValue.Name != "Voltage")
        {
            counter++;
            
            if (parsedValue.Name == "PanelVoltage") {
                pvVoltageValue = parsedValue.Value.toFloat();
            }
            else if (parsedValue.Name == "H19") {
                totalEnergyValue = parsedValue.Value.toFloat();
            }
        }

        if (counter == 5)
        {
            break;
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
