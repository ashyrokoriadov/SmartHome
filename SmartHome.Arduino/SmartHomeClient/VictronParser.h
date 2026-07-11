#pragma once

#include <Arduino.h>

namespace BytesMappings {
  const uint8_t byteV = 0x56; // V
  const uint8_t byteI = 0x49; // I
  const uint8_t byteP = 0x50; // P
  const uint8_t byteC = 0x43; // C
  const uint8_t byteS = 0x53; // S
  const uint8_t byteE = 0x45; // E
  const uint8_t byteR = 0x52; // R
  const uint8_t byteH = 0x48; // H
  const uint8_t byteD = 0x44; // D

  const uint8_t byteTab = 0x09; // TAB
  const uint8_t byteCR = 0x0D;  // CR
  const uint8_t byteLF = 0x0A;  // LF
  const uint8_t byteHash = 0x23; // #

  const uint8_t hexByte0 = 0x30;
  const uint8_t hexByte1 = 0x31;
  const uint8_t hexByte2 = 0x32;
  const uint8_t hexByte3 = 0x33;
  const uint8_t hexByte4 = 0x34;
  const uint8_t hexByte5 = 0x35;
  const uint8_t hexByte6 = 0x36;
  const uint8_t hexByte7 = 0x37;
  const uint8_t hexByte8 = 0x38;
  const uint8_t hexByte9 = 0x39;
}

struct ParsedValue {
  String Name;
  String Value;

  ParsedValue() : Name(""), Value("") {}
  ParsedValue(const String& n, const String& v) : Name(n), Value(v) {}

  bool IsEmpty() const { return Name.length() == 0 && Value.length() == 0; }
  String ToString() const { return Name + ": " + Value; }
  static ParsedValue Empty() { return ParsedValue("", ""); }
};

struct ByteBuffer {
  uint8_t data[512];
  int len;
};

class VictronDataParser {
public:
  void ParseBytes(String inputTokens[], int tokenCount, String &outKey, String &outValue) {
    ByteBuffer bytes = ParseToBytes(inputTokens, tokenCount);

    if (bytes.len == 0) {
      outKey = "Unknown";
      outValue = "";
      return;
    }

    if (ContainsSequence(bytes, CurrentPattern, sizeof(CurrentPattern))) outKey = "Current";
    else if (ContainsSequence(bytes, PanelVoltagePattern, sizeof(PanelVoltagePattern))) outKey = "PanelVoltage";
    else if (ContainsSequence(bytes, PanelPowerPattern, sizeof(PanelPowerPattern))) outKey = "PanelPower";
    else if (ContainsSequence(bytes, ChargerStatePattern, sizeof(ChargerStatePattern))) outKey = "ChargerState";
    else if (ContainsSequence(bytes, ErrorPattern, sizeof(ErrorPattern))) outKey = "Error";
    else if (ContainsSequence(bytes, H19Pattern, sizeof(H19Pattern))) outKey = "H19";
    else if (ContainsSequence(bytes, ProductIdPattern, sizeof(ProductIdPattern))) outKey = "ProductId";
    else if (ContainsSequence(bytes, SerialNumberPattern, sizeof(SerialNumberPattern))) outKey = "SerialNumber";
    else if (ContainsSequence(bytes, VoltagePattern, sizeof(VoltagePattern))) outKey = "Voltage";
    else outKey = "Unknown";

    if (outKey == "Unknown") {
      outValue = "";
      return;
    }

    String value = ExtractValue(bytes);
    String cleaned;
    for (unsigned int i = 0; i < value.length(); ++i) {
      char c = value.charAt(i);
      if ((uint8_t)c >= 0x20) cleaned += c;
    }
    outValue = cleaned;
  }

private:
  ByteBuffer ParseToBytes(String tokens[], int tokenCount) {
    ByteBuffer buf;
    buf.len = 0;
    for (int i = 0; i < tokenCount; ++i) {
      String t = tokens[i];
      if (t.length() == 0) continue;
      bool isHex = true;
      for (unsigned int k = 0; k < t.length(); ++k) {
        char c = t.charAt(k);
        if (!isxdigit((int)c)) { isHex = false; break; }
      }
      if (isHex) {
        unsigned long v = strtoul(t.c_str(), NULL, 16);
        if (v <= 0xFF) { buf.data[buf.len++] = (uint8_t)v; continue; }
      }
      bool isDec = true;
      for (unsigned int k = 0; k < t.length(); ++k) {
        char c = t.charAt(k);
        if (!isdigit((int)c)) { isDec = false; break; }
      }
      if (isDec) {
        unsigned long v = strtoul(t.c_str(), NULL, 10);
        if (v <= 0xFF) { buf.data[buf.len++] = (uint8_t)v; continue; }
      }
    }
    return buf;
  }

  bool ContainsSequence(const ByteBuffer &arr, const uint8_t *pattern, size_t patternSize) {
    if (patternSize == 0) return true;
    if ((size_t)arr.len < patternSize) return false;
    for (int i = 0; i + (int)patternSize <= arr.len; ++i) {
      int j = 0;
      while (j < (int)patternSize && arr.data[i + j] == pattern[j]) j++;
      if (j == (int)patternSize) return true;
    }
    return false;
  }

  String ExtractValue(const ByteBuffer &bytes) {
    int tabIndex = -1;
    int crIndex = -1;
    for (int i = 0; i < bytes.len; ++i) {
      if (bytes.data[i] == BytesMappings::byteTab && tabIndex == -1) tabIndex = i;
      if (bytes.data[i] == BytesMappings::byteCR) { crIndex = i; break; }
    }
    if (tabIndex == -1 || crIndex == -1 || crIndex <= tabIndex) return String("");
    String s;
    for (int i = tabIndex + 1; i < crIndex; ++i) {
      s += (char)bytes.data[i];
    }
    return s;
  }

  static const uint8_t VoltagePattern[2];
  static const uint8_t CurrentPattern[2];
  static const uint8_t PanelVoltagePattern[4];
  static const uint8_t PanelPowerPattern[4];
  static const uint8_t ChargerStatePattern[3];
  static const uint8_t ErrorPattern[4];
  static const uint8_t H19Pattern[4];
  static const uint8_t ProductIdPattern[4];
  static const uint8_t SerialNumberPattern[5];
};

class VictronSerialReader {
public:
  VictronSerialReader() : bufLen(0) {}

  ParsedValue ReadByte(const String &input) {
    if (bufLen < MAX_BUFFER_TOKENS) {
      buffer[bufLen++] = input;
    }
    if (input == "D") {
      String key, val;
      parser.ParseBytes(buffer, bufLen, key, val);
      bufLen = 0;
      return AdjustValue(key, val);
    }
    return ParsedValue::Empty();
  }

private:
  static const int MAX_BUFFER_TOKENS = 256;
  String buffer[MAX_BUFFER_TOKENS];
  int bufLen;
  VictronDataParser parser;

  ParsedValue AdjustValue(const String &key, const String &val) {
    if (key == "Voltage") {
      double v = val.toDouble();
      char tmp[32];
      dtostrf(v / 1000.0, 0, 3, tmp);
      return ParsedValue(key, String(tmp));
    }
    if (key == "Current") {
      double c = val.toDouble();
      char tmp[32]; dtostrf(c / 100.0, 0, 3, tmp);
      return ParsedValue(key, String(tmp));
    }
    if (key == "PanelVoltage") {
      double pv = val.toDouble();
      char tmp[32]; dtostrf(pv / 1000.0, 0, 3, tmp);
      return ParsedValue(key, String(tmp));
    }
    if (key == "PanelPower") {
      return ParsedValue(key, val);
    }
    if (key == "ChargerState") {
      int s = val.toInt();
      return ParsedValue(key, ChargerStateName(s));
    }
    if (key == "Error") {
      return ParsedValue(key, val);
    }
    if (key == "H19") {
      double h = val.toDouble();
      char tmp[32]; dtostrf(h / 100.0, 0, 3, tmp);
      return ParsedValue(key, String(tmp));
    }
    if (key == "ProductId" || key == "SerialNumber" || key == "Unknown") {
      return ParsedValue(key, val);
    }
    if (key == "GainedEnergy") {
      double g = val.toDouble();
      char tmp[32]; dtostrf(g / 100.0, 0, 3, tmp);
      return ParsedValue(key, String(tmp));
    }
    return ParsedValue::Empty();
  }

  String ChargerStateName(int v) {
    switch (v) {
      case 0: return "Bulk";
      case 1: return "Absorption";
      case 2: return "Float";
      case 3: return "Reserve3";
      case 4: return "Reserve4";
      default: return String(v);
    }
  }
};
