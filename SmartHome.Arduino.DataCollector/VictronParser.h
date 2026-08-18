#pragma once

#include <Arduino.h>

struct VictronMeasurement {
    String name;
    float value;
};

class VictronParser {
public:
    static VictronMeasurement parse(const String& rawValue)
    {
        VictronMeasurement measurement;
        measurement.name = "";
        measurement.value = 0.0f;

        String value = rawValue;
        value.trim();

        if (value.length() == 0) {
            return measurement;
        }

        if (value.indexOf("PV") >= 0 && value.indexOf("V") >= 0) {
            measurement.name = "pv_voltage";
            measurement.value = parseNumericValue(value);
            return measurement;
        }

        if (value.indexOf("TOTAL") >= 0 && value.indexOf("ENERGY") >= 0) {
            measurement.name = "total_energy";
            measurement.value = parseNumericValue(value);
            return measurement;
        }

        return measurement;
    }

private:
    static float parseNumericValue(const String& source)
    {
        String cleaned = source;
        cleaned.replace("PV", "");
        cleaned.replace("TOTAL", "");
        cleaned.replace("ENERGY", "");
        cleaned.replace("V", "");
        cleaned.replace("=", "");
        cleaned.replace(" ", "");
        cleaned.trim();

        if (cleaned.length() == 0) {
            return 0.0f;
        }

        return cleaned.toFloat();
    }
};
