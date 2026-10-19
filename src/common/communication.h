#pragma once
#include "esp_err.h"
#include "esp_now.h"
#include "readings.h"
#include <array>
#include <cstdint>
#include <optional>

static constexpr size_t MAX_MESSAGE_SIZE = 250;

enum class EndDeviceMessageType : uint8_t {
    SendReadings,
    RequestReadings,
};

typedef struct {
    EndDeviceMessageType type;
    uint8_t sensorId;
    uint8_t readingsData[];
} EndDeviceMessage;

typedef struct {
    uint8_t sensorId;
    uint8_t readingsData[];
} RouterMessage;

class Router {
public:
    static Router &getInstance();
    void setLastReading(uint8_t sensorId, Readings readings);
    Readings &getLastReading(uint8_t sensorId);
    esp_err_t init();

private:
    static void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);
    Router() {
    }
    Readings mLastReadings[256];
};

class EndDevice {
public:
    static EndDevice &getInstance();
    std::optional<Readings> receiveReadings();
    esp_err_t requestReadings(uint8_t sensorId);
    esp_err_t sendReadings(Readings &readings);
    esp_err_t init(std::array<uint8_t, ESP_NOW_ETH_ALEN>, uint8_t sensorId);

private:
    std::optional<std::array<uint8_t, ESP_NOW_ETH_ALEN>> mRouterAddress;
    std::optional<Readings> mReceivedReadings;
    std::optional<uint8_t> mSensorId;
    std::optional<uint8_t> mRequestedSensorId;

    static void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len);
    EndDevice() {
    }
    ~EndDevice();
};
