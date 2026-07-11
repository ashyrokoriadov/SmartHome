// Converted from C# VictronGate sample to C++ console application
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <cctype>
#include <sstream>
#include <algorithm>
#include <optional>
#include <utility>

namespace BytesMappings {
	constexpr uint8_t byteV = 0x56; // V
	constexpr uint8_t byteI = 0x49; // I
	constexpr uint8_t byteP = 0x50; // P
	constexpr uint8_t byteC = 0x43; // C
	constexpr uint8_t byteS = 0x53; // S
	constexpr uint8_t byteE = 0x45; // E
	constexpr uint8_t byteR = 0x52; // R
	constexpr uint8_t byteH = 0x48; // H
	constexpr uint8_t byteD = 0x44; // D

	constexpr uint8_t byteTab = 0x09; // TAB
	constexpr uint8_t byteCR = 0x0D;  // CR
	constexpr uint8_t byteLF = 0x0A;  // LF
	constexpr uint8_t byteHash = 0x23; // #

	constexpr uint8_t hexByte0 = 0x30;
	constexpr uint8_t hexByte1 = 0x31;
	constexpr uint8_t hexByte2 = 0x32;
	constexpr uint8_t hexByte3 = 0x33;
	constexpr uint8_t hexByte4 = 0x34;
	constexpr uint8_t hexByte5 = 0x35;
	constexpr uint8_t hexByte6 = 0x36;
	constexpr uint8_t hexByte7 = 0x37;
	constexpr uint8_t hexByte8 = 0x38;
	constexpr uint8_t hexByte9 = 0x39;
}

struct ParsedValue {
	std::string Name;
	std::string Value;

	ParsedValue() = default;
	ParsedValue(const std::string& n, const std::string& v) : Name(n), Value(v) {}

	bool IsEmpty() const { return Name.empty() && Value.empty(); }

	std::string ToString() const { return Name + ": " + Value; }
	bool operator==(const ParsedValue& o) const { return Name == o.Name && Value == o.Value; }

	static ParsedValue Empty() { return ParsedValue("", ""); }
};

class VictronDataParser {
public:
	std::pair<std::string, std::string> ParseBytes(const std::vector<std::string>& inputTokens) {
		auto bytes = ParseToBytes(inputTokens);

		std::string key;
		std::string value;

		if (ContainsSequence(bytes, CurrentPattern())) key = "Current";
		else if (ContainsSequence(bytes, PanelVoltagePattern())) key = "PanelVoltage";
		else if (ContainsSequence(bytes, PanelPowerPattern())) key = "PanelPower";
		else if (ContainsSequence(bytes, ChargerStatePattern())) key = "ChargerState";
		else if (ContainsSequence(bytes, ErrorPattern())) key = "Error";
		else if (ContainsSequence(bytes, H19Pattern())) key = "H19";
		else if (ContainsSequence(bytes, ProductIdPattern())) key = "ProductId";
		else if (ContainsSequence(bytes, SerialNumberPattern())) key = "SerialNumber";
		else if (ContainsSequence(bytes, VoltagePattern())) key = "Voltage";
		else key = "Unknown";

		if (key == "Unknown") return {key, ""};

		auto valueBytes = ExtractValue(bytes);
		value = BytesToAscii(valueBytes);
		// trim control characters
		value.erase(std::remove_if(value.begin(), value.end(), [](char c){ return (unsigned char)c < 0x20; }), value.end());

		return {key, value};
	}

private:
	std::vector<uint8_t> ParseToBytes(const std::vector<std::string>& tokens) {
		std::vector<uint8_t> result;
		for (const auto& token : tokens) {
			if (token.empty()) continue;
			try {
				// try hex first
				unsigned long v = std::stoul(token, nullptr, 16);
				if (v <= 0xFF) { result.push_back(static_cast<uint8_t>(v)); continue; }
			} catch (...) { }
			try {
				unsigned long v = std::stoul(token, nullptr, 10);
				if (v <= 0xFF) { result.push_back(static_cast<uint8_t>(v)); continue; }
			} catch (...) { }
		}
		return result;
	}

	bool ContainsSequence(const std::vector<uint8_t>& arr, const std::vector<uint8_t>& pattern) {
		if (pattern.empty()) return true;
		if (pattern.size() > arr.size()) return false;
		for (size_t i = 0; i + pattern.size() <= arr.size(); ++i) {
			size_t j = 0;
			while (j < pattern.size() && arr[i + j] == pattern[j]) ++j;
			if (j == pattern.size()) return true;
		}
		return false;
	}

	std::vector<uint8_t> ExtractValue(const std::vector<uint8_t>& bytes) {
		auto itTab = std::find(bytes.begin(), bytes.end(), BytesMappings::byteTab);
		if (itTab == bytes.end()) return {};
		auto itCR = std::find(bytes.begin(), bytes.end(), BytesMappings::byteCR);
		if (itCR == bytes.end()) return {};
		// include from tab to CR (mimic original behavior), we'll strip controls later
		std::vector<uint8_t> out;
		for (auto it = itTab; it != itCR + 1; ++it) out.push_back(*it);
		return out;
	}

	std::string BytesToAscii(const std::vector<uint8_t>& bytes) {
		std::string s;
		s.reserve(bytes.size());
		for (auto b : bytes) s.push_back(static_cast<char>(b));
		return s;
	}

	std::vector<uint8_t> VoltagePattern() const { return {BytesMappings::byteV, BytesMappings::byteTab}; }
	std::vector<uint8_t> CurrentPattern() const { return {BytesMappings::byteI, BytesMappings::byteTab}; }
	std::vector<uint8_t> PanelVoltagePattern() const { return {BytesMappings::byteV, BytesMappings::byteP, BytesMappings::byteV, BytesMappings::byteTab}; }
	std::vector<uint8_t> PanelPowerPattern() const { return {BytesMappings::byteP, BytesMappings::byteP, BytesMappings::byteV, BytesMappings::byteTab}; }
	std::vector<uint8_t> ChargerStatePattern() const { return {BytesMappings::byteC, BytesMappings::byteS, BytesMappings::byteTab}; }
	std::vector<uint8_t> ErrorPattern() const { return {BytesMappings::byteE, BytesMappings::byteR, BytesMappings::byteR, BytesMappings::byteTab}; }
	std::vector<uint8_t> H19Pattern() const { return {BytesMappings::byteH, BytesMappings::hexByte1, BytesMappings::hexByte9, BytesMappings::byteTab}; }
	std::vector<uint8_t> ProductIdPattern() const { return {BytesMappings::byteP, BytesMappings::byteI, BytesMappings::byteD, BytesMappings::byteTab}; }
	std::vector<uint8_t> SerialNumberPattern() const { return {BytesMappings::byteS, BytesMappings::byteE, BytesMappings::byteR, BytesMappings::byteHash, BytesMappings::byteTab}; }
};

class VictronSerialReader {
public:
	ParsedValue ReadByte(const std::string& input) {
		buffer.push_back(input);

		if (input == "D") {
			auto parsingResult = parser.ParseBytes(buffer);
			buffer.clear();

			return AdjustValue(parsingResult);
		}

		return ParsedValue::Empty();
	}

	// original C# AdjustValue port
private:
	std::vector<std::string> buffer;
	VictronDataParser parser;

	ParsedValue AdjustValue(const std::pair<std::string, std::string>& parsingResult) {
		const auto &key = parsingResult.first;
		const auto &value = parsingResult.second;

		try {
			if (key == "Voltage") {
				if (!value.empty()) {
					double voltage = std::stod(value);
					std::ostringstream ss; ss << (voltage / 1000.0);
					return ParsedValue(key, ss.str());
				}
			}
			if (key == "Current") {
				if (!value.empty()) {
					double current = std::stod(value);
					std::ostringstream ss; ss << (current / 100.0);
					return ParsedValue(key, ss.str());
				}
			}
			if (key == "PanelVoltage") {
				if (!value.empty()) {
					double panelVoltage = std::stod(value);
					std::ostringstream ss; ss << (panelVoltage / 1000.0);
					return ParsedValue(key, ss.str());
				}
			}
			if (key == "PanelPower") {
				if (!value.empty()) return ParsedValue(key, value);
			}
			if (key == "ChargerState") {
				if (!value.empty()) {
					int csInt = std::stoi(value);
					switch (csInt) {
						case 0: return ParsedValue(key, "Bulk");
						case 1: return ParsedValue(key, "Absorption");
						case 2: return ParsedValue(key, "Float");
						default: return ParsedValue::Empty();
					}
				}
			}
			if (key == "Error") {
				if (!value.empty()) return ParsedValue(key, value);
			}
			if (key == "H19") {
				if (!value.empty()) {
					double h19 = std::stod(value);
					std::ostringstream ss; ss << (h19 / 100.0);
					return ParsedValue(key, ss.str());
				}
			}
			if (key == "ProductId" || key == "SerialNumber" || key == "Unknown") {
				return ParsedValue(key, value);
			}
			if (key == "GainedEnergy") {
				if (!value.empty()) {
					double gainedEnergy = std::stod(value);
					std::ostringstream ss; ss << (gainedEnergy / 100.0);
					return ParsedValue(key, ss.str());
				}
			}
		} catch (...) {
			// fall through
		}

		return ParsedValue::Empty();
	}
};

int main() {
	VictronSerialReader reader;

	std::string wholeInput = R"(D A 50 49 44 9 30 78 41 30 35 38 D A 46 57 9 31 35 39 D A 53 45 52 23 9 48 51 32 35 33 37 5A 45 41 47 55 D A 56 9 31 33 32 37 30 D A 49 9 30 D A 56 50 56 9 31 33 32 32 30 D A 50 50 56 9 30 D A 43 53 9 30 D A 4D 50 50 54 9 30 D A 4F 52 9 30 78 30 30 30 30 30 30 30 31 D A 45 52 52 9 30 D A 4C 4F 41 44 9 4F 4E D A 48 31 39 9 31 39 38 D A 48 32 30 9 34 D A 48 32 31 9 33 30 D A 48 32 32 9 38 D A 48 32 33 9 33 34 D A 48 53 44 53 9 33 32 D A 43 68 65 63 6B 73 75 6D 9 4F D A 50 49 44 9 30 78 41 30 35 38 D A 46 57 9 31 35 39 D A 53 45 52 23 9 48 51 32 35 33 37 5A 45 41 47 55 D A 56 9 31 33 32 38 30 D A 49 9 30 D A 56 50 56 9 31 33 32 36 30 D A 50 50 56 9 30 D A 43 53 9 30 D A 4D 50 50 54 9 30 D A 4F 52 9 30 78 30 30 30 30 30 30 30 31 D A 45 52 52 9 30 D A 4C 4F 41 44 9 4F 4E D A 48 31 39 9 31 39 38 D A 48 32 30 9 34 D A 48 32 31 9 33 30 D A 48 32 32 9 38 D A 48 32 33 9 33 34 D A 48 53 44 53 9 33 32 D A 43 68 65 63 6B 73 75 6D 9 4A D A 50 49 44 9 30 78 41 30 35 38 D A 46 57 9 31 35 39 D A 53 45 52 23 9 48 51 32 35 33 37 5A 45 41 47 55 D A 56 9 31 33 32 38 30 D A 49 9 30 D A 56 50 56 9 31 33 32 36 30 D A 50 50 56 9 30 D A 43 53 9 30 D A 4D 50 50 54 9 30 D A 4F 52 9 30 78 30 30 30 30 30 30 30 31 D A 45 52 52 9 30 D A 4C 4F 41 44 9 4F 4E D A 48 31 39 9 31 39 38 D A 48 32 30 9 34 D A 48 32 31 9 33 30 D A 48 32 32 9 38 D A 48 32 33 9 33 34 D A 48 53 44 53 9 33 32)";
	std::istringstream iss(wholeInput);
	std::vector<std::string> tokens;
	std::string t;
	while (iss >> t) tokens.push_back(t);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 2);

	for (const auto& token : tokens) {
		auto pv = reader.ReadByte(token);
		if (!pv.IsEmpty() && pv.Name != "Unknown") {
			std::cout << pv.ToString() << std::endl;
		}
		int delay = dist(gen);
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}

	std::cout << "TRANSMISSION END." << std::endl;
	std::cin.get();
	return 0;
}
