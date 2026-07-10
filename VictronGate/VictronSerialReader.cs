namespace VictronGate
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using VictronGate.Models;

    public class VictronSerialReader
    {
        private readonly VictronDataParser _victronDataParser = new();

        private List<string> Buffer { get; } = [];

        private string _d = "D";

        public ParsedValue ReadByte(string input)
        {
            Buffer.Add(input);

            if (input == _d)
            {
                var parsingResult = _victronDataParser.ParseBytes(Buffer.ToArray());
                Buffer.Clear();

                var adjustedValue = AdjustValue(parsingResult);
                return adjustedValue;
            }

            return ParsedValue.Empty;
        }

        private ParsedValue AdjustValue(KeyValuePair<string, string> parsingResult)
        {
            switch (parsingResult.Key)
            {
                case "Voltage":
                    if (decimal.TryParse(parsingResult.Value, out var voltage))
                        return new ParsedValue(parsingResult.Key, (voltage / 1000m).ToString(CultureInfo.InvariantCulture));
                    break;
                case "Current":
                    if (decimal.TryParse(parsingResult.Value, out var current))
                        return new ParsedValue(parsingResult.Key, (current / 100m).ToString(CultureInfo.InvariantCulture));
                    break;
                case "PanelVoltage":
                    if (decimal.TryParse(parsingResult.Value, out var panelVoltage))
                        return new ParsedValue(parsingResult.Key, (panelVoltage / 1000m).ToString(CultureInfo.InvariantCulture));
                    break;
                case "PanelPower":
                    if (decimal.TryParse(parsingResult.Value, out var panelPower))
                        return new ParsedValue(parsingResult.Key, panelPower.ToString(CultureInfo.InvariantCulture));
                    break;
                case "ChargerState":
                    if (int.TryParse(parsingResult.Value, out var csInt) && Enum.IsDefined(typeof(ChargerState), csInt))
                        return new ParsedValue(parsingResult.Key, ((ChargerState)csInt).ToString());
                    break;
                case "Error":
                    if (int.TryParse(parsingResult.Value, out var errInt))
                        return new ParsedValue(parsingResult.Key, errInt.ToString(CultureInfo.InvariantCulture));
                    break;
                case "H19":
                    if (decimal.TryParse(parsingResult.Value, out var h19))
                        return new ParsedValue(parsingResult.Key, (h19 / 100m).ToString(CultureInfo.InvariantCulture));
                    break;
                case "ProductId":
                case "SerialNumber":
                case "Unknown":
                    return new ParsedValue(parsingResult.Key, parsingResult.Value);
                case "GainedEnergy":
                    if (decimal.TryParse(parsingResult.Value, out var gainedEnergy))
                        return new ParsedValue(parsingResult.Key, (gainedEnergy / 100m).ToString(CultureInfo.InvariantCulture));
                    break;
            }

            return ParsedValue.Empty;
        }
    }
}