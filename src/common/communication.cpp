#include "communication.h"
#include "esp_err.h"
#include "esp_now.h"

#include "WiFi.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>

// Router

void Router::onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    const EndDeviceMessage *message = (const EndDeviceMessage*)incomingData;
    switch (message->type) {
        case EndDeviceMessageType::SendReadings: {
            std::optional<Readings> readings = Readings::fromBytes(message->readingsData, len - sizeof(EndDeviceMessage));
            if (readings) {
                Router::getInstance().setLastReading(message->sensorId, std::move(readings.value()));
            } else {
                Serial.println("Router: received malformed message");
            }
            break;
        }
        case EndDeviceMessageType::RequestReadings: {
            esp_now_peer_info_t *peer = new esp_now_peer_info_t;
            memset(peer, 0, sizeof(esp_now_peer_info_t));
            memcpy(peer->peer_addr, info->src_addr, ESP_NOW_ETH_ALEN);

            esp_now_add_peer(peer);

            const Readings &readings = Router::getInstance().getLastReading(message->sensorId);
            const std::vector<uint8_t> bytes = readings.toBytes();
            const size_t messageSize = sizeof(RouterMessage) + bytes.size();
            RouterMessage *routerMessage = (RouterMessage*)malloc(messageSize);
            routerMessage->sensorId = message->sensorId;
            memcpy(routerMessage->readingsData, bytes.data(), bytes.size());

            if (esp_now_send(peer->peer_addr, (uint8_t*)routerMessage, messageSize) != ESP_OK) {
                Serial.println("Router: RequestReadings failed to send");
            }

            esp_now_del_peer(peer->peer_addr);
            delete peer;
            free(routerMessage);
            break;
        }
    }
}

esp_err_t Router::init() {
    WiFi.mode(WIFI_MODE_STA);
    esp_err_t res = esp_now_init();
    if (res != ESP_OK) {
        return res;
    }
    esp_now_register_recv_cb(Router::onDataRecv);
    return ESP_OK;
}

Router& Router::getInstance() {
    static Router router;
    return router;
}

void Router::setLastReading(const uint8_t sensorId, Readings readings) {
    Router::getInstance().mLastReadings[sensorId] = std::move(readings);
}

Readings& Router::getLastReading(uint8_t sensorId) {
    return Router::getInstance().mLastReadings[sensorId];
}

// EndDevice

EndDevice& EndDevice::getInstance() {
    static EndDevice endDevice;
    return endDevice;
}

esp_err_t EndDevice::init(std::array<uint8_t, ESP_NOW_ETH_ALEN> routerAddress, uint8_t sensorId) {
    WiFi.mode(WIFI_MODE_STA);
    esp_err_t res = esp_now_init();
    if (res != ESP_OK) {
        return res;
    }
    esp_now_register_recv_cb(EndDevice::onDataRecv);

    esp_now_peer_info_t *peer = new esp_now_peer_info_t;
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    memcpy(peer->peer_addr, routerAddress.data(), ESP_NOW_ETH_ALEN);

    esp_now_add_peer(peer);

    mRouterAddress = routerAddress;
    mSensorId = sensorId;
    return ESP_OK;
}


esp_err_t EndDevice::requestReadings(uint8_t sensorId) {
    EndDeviceMessage message;
    message.sensorId = sensorId;
    message.type = EndDeviceMessageType::RequestReadings;

    mRequestedSensorId = sensorId;

    return esp_now_send(mRouterAddress.value().data(), (uint8_t*)&message, sizeof(EndDeviceMessage));
}

esp_err_t EndDevice::sendReadings(Readings &readings) {
    const std::vector<uint8_t> bytes = readings.toBytes();
    const size_t messageSize = sizeof(EndDeviceMessage) + bytes.size();
    EndDeviceMessage *message = (EndDeviceMessage*)malloc(messageSize);
    message->type = EndDeviceMessageType::SendReadings;
    message->sensorId = mSensorId.value();
    memcpy(message->readingsData, bytes.data(), bytes.size());


    auto res =  esp_now_send(mRouterAddress.value().data(), (uint8_t*)message, messageSize);
    free(message);
    return res;
}

void EndDevice::onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    const RouterMessage *message = (const RouterMessage*)incomingData;
    EndDevice& endDevice = EndDevice::getInstance();
    std::optional<Readings> readings = Readings::fromBytes(message->readingsData, len - sizeof(RouterMessage));
    if (readings && endDevice.mRequestedSensorId && endDevice.mRequestedSensorId.value() == message->sensorId) {
        endDevice.mReceivedReadings = std::move(readings);
    } else {
        Serial.println("EndDevice: received malformed message");
    }
}

EndDevice::~EndDevice() {
    esp_now_del_peer(mRouterAddress.value().data());
}

std::optional<Readings> EndDevice::receiveReadings() {
    if (mReceivedReadings) {
        auto readings = std::move(std::exchange(mReceivedReadings, std::nullopt).value());
        mRequestedSensorId.reset();
        return readings;
    }
    return {};
}
