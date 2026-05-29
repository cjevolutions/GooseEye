#include "config.h"
#include "protocol.h"
#include <Preferences.h>
#include <esp_system.h>

namespace ge {

namespace {

constexpr const char* NVS_NS         = "gooseeye";
constexpr const char* KEY_NAME       = "name";
constexpr const char* KEY_FRAME_RATE = "frame_rate";

DeviceConfig g_config;
Preferences  g_prefs;

// Lightweight JSON helpers. We deliberately avoid pulling in ArduinoJson to
// keep the firmware footprint small; payloads are tiny and well-defined.
String jsonEscape(const String& in) {
    String out;
    out.reserve(in.length() + 2);
    for (size_t i = 0; i < in.length(); ++i) {
        char c = in[i];
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if ((uint8_t)c < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

// Naive JSON string-value extractor for keys like "name". Returns empty string
// if not found. Sufficient for our trusted, well-formed payloads.
bool extractString(const String& json, const char* key, String& out) {
    String needle = String("\"") + key + "\"";
    int k = json.indexOf(needle);
    if (k < 0) return false;
    int colon = json.indexOf(':', k);
    if (colon < 0) return false;
    int q1 = json.indexOf('"', colon);
    if (q1 < 0) return false;
    int q2 = json.indexOf('"', q1 + 1);
    if (q2 < 0) return false;
    out = json.substring(q1 + 1, q2);
    return true;
}

bool extractInt(const String& json, const char* key, int32_t& out) {
    String needle = String("\"") + key + "\"";
    int k = json.indexOf(needle);
    if (k < 0) return false;
    int colon = json.indexOf(':', k);
    if (colon < 0) return false;
    // Skip whitespace
    int p = colon + 1;
    while (p < (int)json.length() && (json[p] == ' ' || json[p] == '\t')) p++;
    int start = p;
    while (p < (int)json.length() && (isDigit(json[p]) || json[p] == '-')) p++;
    if (p == start) return false;
    out = json.substring(start, p).toInt();
    return true;
}

} // namespace

void Config::begin() {
    g_config.name        = GE_DEFAULT_DEVICE_NAME;
    g_config.frameRateHz = 100;
    load();
}

void Config::load() {
    g_prefs.begin(NVS_NS, true /*read-only*/);
    g_config.name        = g_prefs.getString(KEY_NAME, GE_DEFAULT_DEVICE_NAME);
    g_config.frameRateHz = g_prefs.getUShort(KEY_FRAME_RATE, 100);
    g_prefs.end();
}

void Config::save() {
    g_prefs.begin(NVS_NS, false /*read-write*/);
    g_prefs.putString(KEY_NAME, g_config.name);
    g_prefs.putUShort(KEY_FRAME_RATE, g_config.frameRateHz);
    g_prefs.end();
}

void Config::resetToDefaults() {
    g_prefs.begin(NVS_NS, false);
    g_prefs.clear();
    g_prefs.end();
    g_config.name        = GE_DEFAULT_DEVICE_NAME;
    g_config.frameRateHz = 100;
}

const DeviceConfig& Config::get() {
    return g_config;
}

void Config::setName(const String& name) {
    String trimmed = name;
    trimmed.trim();
    if (trimmed.length() == 0) return;
    if (trimmed.length() > 24) trimmed = trimmed.substring(0, 24);
    g_config.name = trimmed;
    save();
}

void Config::setFrameRate(uint16_t hz) {
    if (hz < 1)    hz = 1;
    if (hz > 1000) hz = 1000;
    g_config.frameRateHz = hz;
    save();
}

String Config::deviceInfoJson() {
    uint64_t macInt = ESP.getEfuseMac();
    char macStr[20];
    snprintf(macStr, sizeof(macStr), "%012llX", macInt);

    char buf[256];
    snprintf(
        buf, sizeof(buf),
        "{\"name\":\"%s\",\"firmware\":\"%d.%d.%d\",\"chip\":\"esp32s3\","
        "\"mac\":\"%s\",\"uptime_s\":%lu}",
        jsonEscape(g_config.name).c_str(),
        FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_PATCH,
        macStr,
        (unsigned long)(millis() / 1000UL)
    );
    return String(buf);
}

String Config::deviceConfigJson() {
    // Note: frame_rate is intentionally omitted from the JSON response.
    // It is stored in NVS but not yet wired to the TFmini or the BLE notify
    // interval. Re-add it here once the feature is implemented.
    char buf[128];
    snprintf(
        buf, sizeof(buf),
        "{\"name\":\"%s\"}",
        jsonEscape(g_config.name).c_str()
    );
    return String(buf);
}

bool Config::applyConfigJson(const String& json) {
    bool changed = false;
    String newName;
    if (extractString(json, "name", newName) && newName.length() > 0) {
        setName(newName);
        changed = true;
    }
    // frame_rate is accepted for forward-compat but currently has no effect.
    // TODO: wire to TFmini UART command or BLE notify interval when implemented.
    return changed;
}

} // namespace ge
