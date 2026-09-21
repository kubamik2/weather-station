#include "readings.h" 
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

std::vector<uint8_t> Readings::toBytes() const {
    size_t bytes = 0;
    // Calculate needed amount of bytes for serialization
    for (auto const &[name, _] : mReadings) {
        //                    v null byte
        bytes += name.length() + 1 + sizeof(float);
    }

    std::vector<uint8_t> data;
    data.resize(bytes);
    size_t i = 0;

    // Serialize
    for (auto const &[name, value] : mReadings) {
        union { float v; uint8_t arr[4]; } u;
        u.v = value;
        for (size_t j = 0; j < sizeof(float); j++) {
            data[i++] = u.arr[j];
        }

        memcpy(&data[i], name.data(), name.length());
        i += name.length();
        data[i++] = '\0';
    }

    return data;
}

std::optional<Readings> Readings::fromBytes(const uint8_t *data, const size_t length) {
    enum class State {
        ReadValue,
        ReadName,
        ReadNameEnd, // Only valid final state
    };
    Readings readings;
    std::string name;
    union { float f; uint8_t arr[4]; } value;
    size_t i = 0;
    State state = State::ReadValue;

    // Deserialize using a state machine
    for (size_t j = 0; j < length; j++) {
        uint8_t byte = *(data++);
        switch (state) {
            case State::ReadNameEnd:
                state = State::ReadValue;
            case State::ReadValue:
                value.arr[i++] = byte;
                if (i == sizeof(float)) {
                    i = 0;
                    state = State::ReadName;
                }
                break;
            case State::ReadName:
                if (byte == '\0') {
                    state = State::ReadNameEnd;
                    readings.setReading(std::move(name), value.f);
                    name = "";
                } else {
                    name.push_back(byte);
                }
                break;
        }
    }

    if (state != State::ReadNameEnd) {
        return {};
    }
    return readings;
}

std::optional<float> Readings::getReading(const char *readingName) const {
    for (auto const &[name, value] : mReadings) {
        if (name.compare(readingName) == 0) {
            return value;
        }
    }
    return {};
}

void Readings::setReading(std::string readingName, float value) {
    for (auto &[name, currentValue] : mReadings) {
        if (name.compare(readingName) == 0) {
            currentValue = value;
            return;
        }
    }
    mReadings.push_back({readingName, value});
}
