#include "status_led.h"

namespace ge {

namespace {

constexpr int kLedPin = LED_BUILTIN;
constexpr int kLedChannel = 0;
constexpr uint32_t kPwmFreqHz = 5000;
constexpr uint8_t kPwmBits = 8;
constexpr uint8_t kDutyPeak = 255; // 100% PWM
constexpr uint32_t kFadeUpMs = 900;
constexpr uint32_t kHoldBrightMs = 250;
constexpr uint32_t kFadeDownMs = 900;
constexpr uint32_t kHoldOffMs = 450;
constexpr uint32_t kCycleMs = kFadeUpMs + kHoldBrightMs + kFadeDownMs + kHoldOffMs;

bool g_initialized = false;
bool g_sleeping   = true;
uint32_t g_phaseMs = 0;

uint8_t breatheDuty(uint32_t elapsedMs) {
    const uint32_t t = elapsedMs % kCycleMs;
    if (t < kFadeUpMs) {
        return static_cast<uint8_t>((t * kDutyPeak) / kFadeUpMs);
    }
    if (t < kFadeUpMs + kHoldBrightMs) {
        return kDutyPeak;
    }
    if (t < kFadeUpMs + kHoldBrightMs + kFadeDownMs) {
        const uint32_t fadeT = t - kFadeUpMs - kHoldBrightMs;
        return static_cast<uint8_t>(kDutyPeak - (fadeT * kDutyPeak) / kFadeDownMs);
    }
    return 0;
}

} // namespace

void StatusLed::begin() {
    if (g_initialized) return;
    ledcSetup(kLedChannel, kPwmFreqHz, kPwmBits);
    ledcAttachPin(kLedPin, kLedChannel);
    ledcWrite(kLedChannel, 0);
    g_initialized = true;
    g_phaseMs = millis();
}

void StatusLed::setSleeping(bool sleeping) {
    g_sleeping = sleeping;
    if (!sleeping) {
        ledcWrite(kLedChannel, 0);
    } else {
        g_phaseMs = millis();
    }
}

void StatusLed::tick() {
    if (!g_initialized) return;
    if (!g_sleeping) {
        ledcWrite(kLedChannel, 0);
        return;
    }

    const uint8_t duty = breatheDuty(millis() - g_phaseMs);
    ledcWrite(kLedChannel, duty);
}

} // namespace ge
