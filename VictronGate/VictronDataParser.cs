using System.Globalization;
using System.Text;

namespace VictronGate
{
    public class VictronDataParser
    {
        public KeyValuePair<string, string> ParseBytes(string[] inputBytes)
        {
            var bytes = ParseToBytes(inputBytes).ToArray();

            var key = string.Empty;
            var value = string.Empty;

            if (ContainsSequence(bytes, CurrentPattern))
            {
                key = "Current";
            }
            else if (ContainsSequence(bytes, PanelVoltagePattern))
            {
                key = "PanelVoltage";
            }
            else if (ContainsSequence(bytes, PanelPowerPattern))
            {
                key = "PanelPower";
            }
            else if (ContainsSequence(bytes, ChargerStatePattern))
            {
                key = "ChargerState";
            }
            else if (ContainsSequence(bytes, ErrorPattern))
            {
                key = "Error";
            }
            else if (ContainsSequence(bytes, H19Pattern))
            {
                key = "H19";
            }
            else if (ContainsSequence(bytes, ProductIdPattern))
            {
                key = "ProductId";
            }
            else if (ContainsSequence(bytes, SerialNumberPattern))
            {
                key = "SerialNumber";
            }
            else if (ContainsSequence(bytes, VoltagePattern))
            {
                key = "Voltage";
            }
            else
            {
                key = "Unknown";
            }

            if (key == "Unknown")
            {
                return new KeyValuePair<string, string>(key, string.Empty);
            }

            var valueBytes = ExtractValue(bytes);
            value = Encoding.ASCII.GetString(valueBytes);

            return new KeyValuePair<string, string>(key, value);
        }        

        private List<byte> ParseToBytes(string[] inputBytes)
        {
            var result = new List<byte>();

            foreach (var token in inputBytes)
            {                

                if (byte.TryParse(token, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out var bHex))
                    result.Add(bHex);
                else if (byte.TryParse(token, NumberStyles.Integer, CultureInfo.InvariantCulture, out var bDec)) 
                    result.Add(bDec);                          
            }

            return result;
        }

        private bool ContainsSequence(byte[] arr, byte[] pattern)
        {
            if (pattern.Length == 0) return true;
            if (pattern.Length > arr.Length) return false;

            for (int i = 0; i <= arr.Length - pattern.Length; i++)
            {
                int j = 0;
                while (j < pattern.Length && arr[i + j] == pattern[j])
                {
                    j++;
                }

                if (j == pattern.Length)
                    return true;
            }

            return false;
        }

        private byte[] ExtractValue(byte[] bytes)
        {
            int tabIndex = Array.IndexOf(bytes, BytesMappings.byteTab);

            if (tabIndex == -1)
            {
                return [];
            }

            int crIndex = Array.IndexOf(bytes, BytesMappings.byteCR);

            if (crIndex == -1)
            {
                return [];
            }

            var actualValueArray = bytes.Skip(tabIndex).Take(crIndex - tabIndex + 1).ToArray();
            return actualValueArray;
        }

        private byte[] VoltagePattern => new[] { BytesMappings.byteV, BytesMappings.byteTab };

        private byte[] CurrentPattern => new[] { BytesMappings.byteI, BytesMappings.byteTab };

        private byte[] PanelVoltagePattern => new[] { BytesMappings.byteV, BytesMappings.byteP, BytesMappings.byteV, BytesMappings.byteTab };

        private byte[] PanelPowerPattern => new[] {  BytesMappings.byteP, BytesMappings.byteP, BytesMappings.byteV, BytesMappings.byteTab };

        private byte[] ChargerStatePattern => new[] {  BytesMappings.byteC, BytesMappings.byteS, BytesMappings.byteTab };

        private byte[] ErrorPattern => new[] { BytesMappings.byteE, BytesMappings.byteR, BytesMappings.byteR, BytesMappings.byteTab };

        private byte[] H19Pattern => new[] { BytesMappings.byteH, BytesMappings.hexByte1, BytesMappings.hexByte9, BytesMappings.byteTab };

        private byte[] ProductIdPattern => new[] { BytesMappings.byteP, BytesMappings.byteI, BytesMappings.byteD, BytesMappings.byteTab };

        private byte[] SerialNumberPattern => new[] { BytesMappings.byteS, BytesMappings.byteE, BytesMappings.byteR, BytesMappings.byteHash, BytesMappings.byteTab };
    }
}
