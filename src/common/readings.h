#pragma once

#include <optional>
#include <vector>
#include <tuple>
#include <cstdint>
#include <string>

class Readings {
public:
    void setReading(std::string readingName, float value);
    std::optional<float> getReading(const char *readingName) const;
    std::vector<uint8_t> toBytes() const;
    static std::optional<Readings> fromBytes(const uint8_t *data, const size_t length);
private:
    std::vector<std::tuple<std::string, float>> mReadings;
};
