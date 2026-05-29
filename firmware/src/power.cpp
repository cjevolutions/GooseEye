#include "power.h"

#include "lidar_power.h"
#include "ota.h"
#include "protocol.h"
#include "status_led.h"

namespace ge {

namespace {

bool     g_bleConnected    = false;
bool     g_appKeepAwake    = false;
uint32_t g_lastSessionMs   = 0;
uint32_t g_notAwakeSinceMs = 0;
bool     g_sleeping        = true;

} // namespace

void Power::begin() {
    g_bleConnected    = false;
    g_appKeepAwake    = false;
    g_lastSessionMs   = 0;
    g_notAwakeSinceMs = millis();
    g_sleeping        = true;
    LidarPower::begin();
    StatusLed::begin();
    StatusLed::setSleeping(true);
}

void Power::setBleConnected(bool connected) {
    g_bleConnected = connected;
    if (!connected) {
        g_appKeepAwake = false;
    }
}

void Power::setAppKeepAwake(bool keepAwake) {
    g_appKeepAwake  = keepAwake;
    g_lastSessionMs = millis();
}

bool Power::appKeepAwake() {
    if (!g_appKeepAwake) return false;
    return (millis() - g_lastSessionMs) <= GE_SESSION_TIMEOUT_MS;
}

bool Power::isAwake() {
    return g_bleConnected && appKeepAwake() && !Ota::inProgress();
}

bool Power::isSleeping() {
    if (Ota::inProgress() || isAwake()) {
        g_notAwakeSinceMs = 0;
        return false;
    }
    if (g_notAwakeSinceMs == 0) {
        g_notAwakeSinceMs = millis();
        return false;
    }
    return (millis() - g_notAwakeSinceMs) >= GE_SLEEP_ENTER_MS;
}

void Power::tick() {
    const bool sleeping = isSleeping();
    if (sleeping != g_sleeping) {
        g_sleeping = sleeping;
        StatusLed::setSleeping(sleeping);
        LidarPower::setPowered(!sleeping);
    }
    StatusLed::tick();
}

} // namespace ge
