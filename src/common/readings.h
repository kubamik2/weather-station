#pragma once

#include <optional>
#include <cstdint>
#include "fixedNameMap.h"

class Readings {
public:
    static constexpr size_t MAX_READINGS = 8;
    static constexpr size_t MAX_READING_NAME_SIZE = 16;

    Readings() {}
    static std::optional<Readings> deserialize(const uint8_t *data, const size_t length);
    std::optional<size_t> serialize(uint8_t *dest, size_t maxSize) const;

    bool setReading(const char* readingName, float value);
    std::optional<float> getReading(const char *readingName) const;
    const FixedNameMap<float, MAX_READING_NAME_SIZE, MAX_READINGS>::KeyValuePair* cbegin() const;
    const FixedNameMap<float, MAX_READING_NAME_SIZE, MAX_READINGS>::KeyValuePair* cend() const;
private:
    FixedNameMap<float, MAX_READING_NAME_SIZE, MAX_READINGS> mMap;
};
