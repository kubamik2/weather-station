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
    static esp_now_peer_info_t peer;
    alignas(RouterMessage) static uint8_t sendBuffer[MAX_MESSAGE_SIZE];
    static RouterMessage *routerMessage = new (sendBuffer) RouterMessage;

    const EndDeviceMessage *message = (const EndDeviceMessage*)incomingData;

    switch (message->type) {
        case EndDeviceMessageType::SendReadings: {
            std::optional<Readings> readings = Readings::deserialize(message->readingsData, len - sizeof(EndDeviceMessage));
            if (readings) {
                Router::getInstance().setLastReading(message->sensorId, std::move(readings.value()));
            } else {
                Serial.println("Router::onDataRecv: received malformed message");
            }
            break;
        }
        case EndDeviceMessageType::RequestReadings: {
            const Readings &readings = Router::getInstance().getLastReading(message->sensorId);
            if (auto written = readings.serialize(routerMessage->readingsData, MAX_MESSAGE_SIZE - sizeof(RouterMessage))) {
                memset(&peer, 0, sizeof(esp_now_peer_info_t));
                memcpy(peer.peer_addr, info->src_addr, ESP_NOW_ETH_ALEN);

                esp_now_add_peer(&peer);
                const size_t messageSize = sizeof(RouterMessage) + written.value();

                routerMessage->sensorId = message->sensorId;

                if (esp_now_send(peer.peer_addr, (uint8_t*)routerMessage, messageSize) != ESP_OK) {
                    Serial.println("Router::onDataRecv: RequestReadings failed to send");
                }

                esp_now_del_peer(peer.peer_addr);
            } else {
                Serial.println("Router::onDataRecv: received readings too bif");
            }
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
    // It is safe to make these static, because the class is a singleton
    alignas(RouterMessage) static uint8_t sendBuffer[MAX_MESSAGE_SIZE];
    static EndDeviceMessage *message = new (sendBuffer) EndDeviceMessage;

    if (auto written = readings.serialize(message->readingsData, MAX_MESSAGE_SIZE - sizeof(EndDeviceMessage))) {
        message->type = EndDeviceMessageType::SendReadings;
        message->sensorId = mSensorId.value();
        auto res =  esp_now_send(mRouterAddress.value().data(), (uint8_t*)message, sizeof(EndDeviceMessage) + written.value());
        return res;
    } else {
        Serial.println("EndDevice::sendReadings: readings too big");
        return ESP_ERR_INVALID_SIZE;
        
    }

    // const std::vector<uint8_t> bytes = readings.serialize();
    // const size_t messageSize = sizeof(EndDeviceMessage) + bytes.size();
    // EndDeviceMessage *message = (EndDeviceMessage*)malloc(messageSize);
    // message->type = EndDeviceMessageType::SendReadings;
    // message->sensorId = mSensorId.value();
    // memcpy(message->readingsData, bytes.data(), bytes.size());
    //
    //
    // auto res =  esp_now_send(mRouterAddress.value().data(), (uint8_t*)message, messageSize);
    // free(message);
    // return res;
}

void EndDevice::onDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    const RouterMessage *message = (const RouterMessage*)incomingData;
    EndDevice& endDevice = EndDevice::getInstance();
    std::optional<Readings> readings = Readings::deserialize(message->readingsData, len - sizeof(RouterMessage));
    if (readings && endDevice.mRequestedSensorId && endDevice.mRequestedSensorId.value() == message->sensorId) {
        endDevice.mReceivedReadings = std::move(readings);
    } else {
        Serial.println("EndDevice::onDataRecv: received malformed message");
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
