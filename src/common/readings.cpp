#include "readings.h"
#include "HardwareSerial.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

std::optional<size_t> Readings::serialize(uint8_t *dest, size_t maxSize) const {
    // Calculate needed amount of bytes for serialization
    size_t bytes = sizeof(size_t) + mMap.size() * (sizeof(char) * MAX_READING_NAME_SIZE + sizeof(float));

    if (bytes > maxSize) {
        return {};
    }

    *(size_t *)dest = mMap.size();

    size_t i = sizeof(size_t);

    // Serialize
    for (auto it = mMap.cbegin(); it != mMap.cend(); it++) {
        union {
            float v;
            uint8_t arr[4];
        } u;
        u.v = it->item;
        for (size_t j = 0; j < sizeof(float); j++) {
            dest[i++] = u.arr[j];
        }

        memcpy(&dest[i], it->name.data(), MAX_READING_NAME_SIZE);
        i += MAX_READING_NAME_SIZE;
    }

    return bytes;
}

std::optional<Readings> Readings::deserialize(const uint8_t *data, const size_t length) {
    // Check if readingCount can be read
    if (length < sizeof(size_t)) {
        return {};
    }

    Readings readings;
    size_t readingCount = *(size_t *)data;

    size_t requiredBytes = sizeof(size_t) + readingCount * (sizeof(char) * MAX_READING_NAME_SIZE + sizeof(float));

    // Validate size
    if (length != requiredBytes) {
        Serial.printf("length: %d, required: %d\n", length, requiredBytes);
        Serial.printf("readingCount: %d\n", readingCount);
        return {};
    }

    char buf[MAX_READING_NAME_SIZE];
    // deserialize
    union {
        float v;
        uint8_t arr[4];
    } u;
    size_t currentByte = sizeof(size_t);
    for (int i = 0; i < readingCount; i++) {
        for (int j = 0; j < sizeof(float); j++) {
            u.arr[j] = data[currentByte++];
        }
        memcpy(buf, &data[currentByte], MAX_READING_NAME_SIZE);
        currentByte += MAX_READING_NAME_SIZE;
        readings.mMap.set(buf, u.v);
    }

    return readings;
}

std::optional<float> Readings::getReading(const char *readingName) const {
    if (auto currentValue = mMap.get(readingName)) {
        return *currentValue.value();
    } else {
        return {};
    }
}

bool Readings::setReading(const char *readingName, float value) {
    return mMap.set(readingName, value);
}

const FixedNameMap<float, Readings::MAX_READING_NAME_SIZE, Readings::MAX_READINGS>::KeyValuePair *
Readings::cbegin() const {
    return mMap.cbegin();
}

const FixedNameMap<float, Readings::MAX_READING_NAME_SIZE, Readings::MAX_READINGS>::KeyValuePair *
Readings::cend() const {
    return mMap.cend();
}
