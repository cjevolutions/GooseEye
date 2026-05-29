#include "ble_server.h"
#include "config.h"
#include "ota.h"
#include "power.h"
#include "protocol.h"

#include <NimBLEDevice.h>

namespace ge {

namespace {

NimBLEServer*         g_server         = nullptr;
NimBLEService*        g_service        = nullptr;
NimBLECharacteristic* g_distanceChar   = nullptr;
NimBLECharacteristic* g_deviceInfoChar = nullptr;
NimBLECharacteristic* g_configChar     = nullptr;
NimBLECharacteristic* g_otaCtrlChar    = nullptr;
NimBLECharacteristic* g_otaDataChar    = nullptr;
NimBLECharacteristic* g_resetChar      = nullptr;
NimBLECharacteristic* g_sessionChar    = nullptr;

bool g_connected      = false;
bool g_distSubscribed = false;

// ----- callbacks ----------------------------------------------------------

class ServerCallbacks : public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* srv, ble_gap_conn_desc* desc) override {
        g_connected = true;
        Power::setBleConnected(true);
        Serial.printf("[BLE] connected  handle=%u\n", desc->conn_handle);
        // Negotiate larger MTU for faster OTA throughput.
        srv->setDataLen(desc->conn_handle, 251);
    }

    void onDisconnect(NimBLEServer* srv) override {
        g_connected      = false;
        g_distSubscribed = false;
        Serial.println("[BLE] disconnected — restarting advertising");
        Ota::handleAbort(); // safety: abort any in-flight OTA
        Power::setBleConnected(false);
        // Resume advertising immediately so the central can reconnect.
        NimBLEDevice::startAdvertising();
    }
};

class DistanceCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onSubscribe(NimBLECharacteristic* /*chr*/, ble_gap_conn_desc* /*desc*/,
                     uint16_t subValue) override {
        g_distSubscribed = (subValue != 0);
    }
};

class SessionCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onWrite(NimBLECharacteristic* chr) override {
        std::string val = chr->getValue();
        if (val.empty()) return;
        const bool keepAwake = static_cast<uint8_t>(val[0]) == GE_SESSION_KEEP_AWAKE;
        Power::setAppKeepAwake(keepAwake);
    }
};

class ConfigCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onWrite(NimBLECharacteristic* chr) override {
        std::string val = chr->getValue();
        if (val.empty()) return;
        String json = String(val.c_str());
        bool changed = Config::applyConfigJson(json);
        // Always refresh the readable value so the central can read back what
        // was actually applied and detect any rejected fields.
        g_deviceInfoChar->setValue(std::string(Config::deviceInfoJson().c_str()));
        g_configChar->setValue(std::string(Config::deviceConfigJson().c_str()));
        if (changed) {
            // Name may have changed; restart advertising with the updated name.
            NimBLEDevice::stopAdvertising();
            NimBLEDevice::setDeviceName(Config::get().name.c_str());
            BleServer::restartAdvertising();
        }
    }
    void onRead(NimBLECharacteristic* chr) override {
        chr->setValue(std::string(Config::deviceConfigJson().c_str()));
    }
};

class DeviceInfoCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onRead(NimBLECharacteristic* chr) override {
        chr->setValue(std::string(Config::deviceInfoJson().c_str()));
    }
};

class OtaCtrlCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onWrite(NimBLECharacteristic* chr) override {
        std::string val = chr->getValue();
        if (val.empty()) return;
        const uint8_t* data = reinterpret_cast<const uint8_t*>(val.data());
        size_t len = val.size();
        switch (data[0]) {
            case GE_OTA_CMD_START:
                Ota::handleStart(data + 1, len - 1);
                break;
            case GE_OTA_CMD_VERIFY:
                Ota::handleVerify();
                break;
            case GE_OTA_CMD_ABORT:
                Ota::handleAbort();
                break;
            default:
                break;
        }
    }
};

class OtaDataCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onWrite(NimBLECharacteristic* chr) override {
        std::string val = chr->getValue();
        if (val.empty()) return;
        Ota::handleData(reinterpret_cast<const uint8_t*>(val.data()), val.size());
    }
};

class FactoryResetCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onWrite(NimBLECharacteristic* chr) override {
        std::string val = chr->getValue();
        if (val.size() != 4) return;
        if ((uint8_t)val[0] == GE_FACTORY_RESET_MAGIC_0 &&
            (uint8_t)val[1] == GE_FACTORY_RESET_MAGIC_1 &&
            (uint8_t)val[2] == GE_FACTORY_RESET_MAGIC_2 &&
            (uint8_t)val[3] == GE_FACTORY_RESET_MAGIC_3) {
            Config::resetToDefaults();
            delay(200);
            esp_restart();
        }
    }
};

} // namespace

void BleServer::begin() {
    NimBLEDevice::init(Config::get().name.c_str());
    NimBLEDevice::setMTU(247);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // +9 dBm for solid in-cab reception

    g_server = NimBLEDevice::createServer();
    g_server->setCallbacks(new ServerCallbacks());

    g_service = g_server->createService(GE_SERVICE_UUID);

    g_distanceChar = g_service->createCharacteristic(
        GE_CHAR_DISTANCE_UUID,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ
    );
    g_distanceChar->setCallbacks(new DistanceCallbacks());

    g_deviceInfoChar = g_service->createCharacteristic(
        GE_CHAR_DEVICE_INFO,
        NIMBLE_PROPERTY::READ
    );
    g_deviceInfoChar->setCallbacks(new DeviceInfoCallbacks());
    g_deviceInfoChar->setValue(std::string(Config::deviceInfoJson().c_str()));

    g_configChar = g_service->createCharacteristic(
        GE_CHAR_DEVICE_CONFIG,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );
    g_configChar->setCallbacks(new ConfigCallbacks());
    g_configChar->setValue(std::string(Config::deviceConfigJson().c_str()));

    g_otaCtrlChar = g_service->createCharacteristic(
        GE_CHAR_OTA_CONTROL,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::NOTIFY
    );
    g_otaCtrlChar->setCallbacks(new OtaCtrlCallbacks());

    g_otaDataChar = g_service->createCharacteristic(
        GE_CHAR_OTA_DATA,
        NIMBLE_PROPERTY::WRITE_NR
    );
    g_otaDataChar->setCallbacks(new OtaDataCallbacks());

    g_resetChar = g_service->createCharacteristic(
        GE_CHAR_FACTORY_RESET,
        NIMBLE_PROPERTY::WRITE
    );
    g_resetChar->setCallbacks(new FactoryResetCallbacks());

    g_sessionChar = g_service->createCharacteristic(
        GE_CHAR_SESSION_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ
    );
    g_sessionChar->setCallbacks(new SessionCallbacks());

    g_service->start();

    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    adv->addServiceUUID(GE_SERVICE_UUID);
    adv->setScanResponse(true);
    adv->setMinPreferred(0x06);
    adv->setMaxPreferred(0x12);
    adv->start();
}

void BleServer::loop() {
    // Nothing to pump here -- NimBLE runs on its own task. Hook for future use.
}

bool BleServer::isStreaming() {
    return g_connected && g_distSubscribed;
}

bool BleServer::isConnected() {
    return g_connected;
}

void BleServer::notifyDistance(const uint8_t* data, size_t len) {
    if (!g_distanceChar || !g_distSubscribed) return;
    g_distanceChar->setValue(const_cast<uint8_t*>(data), len);
    g_distanceChar->notify();
}

void BleServer::notifyOtaStatus(const uint8_t* data, size_t len) {
    if (!g_otaCtrlChar) return;
    g_otaCtrlChar->setValue(const_cast<uint8_t*>(data), len);
    g_otaCtrlChar->notify();
}

void BleServer::restartAdvertising() {
    NimBLEDevice::startAdvertising();
}

} // namespace ge
