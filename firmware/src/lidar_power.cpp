#include "lidar_power.h"

#include "lidar.h"
#include "pins.h"

namespace ge {

namespace {

bool g_powered = false;

void writeGpio(bool sensorOn) {
#if GE_LIDAR_POWER_ACTIVE_HIGH
    digitalWrite(GE_LIDAR_POWER_PIN, sensorOn ? HIGH : LOW);
#else
    digitalWrite(GE_LIDAR_POWER_PIN, sensorOn ? LOW : HIGH);
#endif
}

} // namespace

void LidarPower::begin() {
    pinMode(GE_LIDAR_POWER_PIN, OUTPUT);
    setPowered(false);
}

void LidarPower::setPowered(bool on) {
    if (on == g_powered) return;

    if (on) {
        writeGpio(true);
        delay(GE_LIDAR_POWER_ON_MS);
        Lidar::setEnabled(true);
        g_powered = true;
        Serial.println(F("[GooseEye] lidar power on"));
    } else {
        Lidar::setEnabled(false);
        delay(GE_LIDAR_POWER_OFF_MS);
        writeGpio(false);
        g_powered = false;
        Serial.println(F("[GooseEye] lidar power off"));
    }
}

bool LidarPower::isPowered() {
    return g_powered;
}

} // namespace ge
