#pragma once

#include <optional>
#include <vector>
#include <tuple>
#include <cstdint>
#include <string>

class Readings {
public:
    Readings();
    static std::optional<Readings> fromBytes(const uint8_t *data, const size_t length);

    Readings(const Readings&) = delete;
    Readings& operator=(const Readings&) = delete;

    Readings& operator=(Readings&&);
    Readings(Readings&&);

    void setReading(std::string readingName, float value);
    std::optional<float> getReading(const char *readingName) const;
    std::vector<uint8_t> toBytes() const;
    void clear() noexcept;
private:
    std::vector<std::tuple<std::string, float>> mReadings;
};
