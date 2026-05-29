#include "lidar.h"
#include <HardwareSerial.h>

namespace ge {

namespace {

// Use UART1 on the XIAO ESP32-S3. UART0 is reserved for USB-CDC console.
HardwareSerial g_serial(1);

LidarFrame g_latest = {0, 0, 0, 0, false};

int      g_rxPin   = -1;
int      g_txPin   = -1;
uint32_t g_baud    = 115200;
bool     g_enabled = false;

// Parser state.
enum ParseState : uint8_t {
    WAIT_HEADER1,
    WAIT_HEADER2,
    READING_PAYLOAD,
};

ParseState g_state = WAIT_HEADER1;
uint8_t    g_buf[9];
uint8_t    g_bufIdx = 0;

bool checksumOk(const uint8_t* frame) {
    uint16_t sum = 0;
    for (int i = 0; i < 8; ++i) sum += frame[i];
    return (uint8_t)(sum & 0xFF) == frame[8];
}

void resetParser() {
    g_state  = WAIT_HEADER1;
    g_bufIdx = 0;
    g_latest = {0, 0, 0, 0, false};
}

} // namespace

void Lidar::begin(int rxPin, int txPin, uint32_t baud) {
    g_rxPin = rxPin;
    g_txPin = txPin;
    g_baud  = baud;
    // UART enabled by LidarPower when awake.
    setEnabled(false);
}

void Lidar::setEnabled(bool enabled) {
    if (enabled == g_enabled) return;

    g_enabled = enabled;
    if (enabled) {
        // Note: HardwareSerial.begin signature is (baud, config, rxPin, txPin).
        // rxPin = ESP32 RX (connected to TFmini TX),
        // txPin = ESP32 TX (connected to TFmini RX).
        g_serial.begin(g_baud, SERIAL_8N1, g_rxPin, g_txPin);
        resetParser();
    } else {
        g_serial.end();
        resetParser();
    }
}

bool Lidar::poll(LidarFrame& outFrame) {
    if (!g_enabled) return false;

    bool newFrame = false;

    while (g_serial.available() > 0) {
        uint8_t b = (uint8_t)g_serial.read();

        switch (g_state) {
            case WAIT_HEADER1:
                if (b == 0x59) {
                    g_buf[0] = b;
                    g_state  = WAIT_HEADER2;
                }
                break;

            case WAIT_HEADER2:
                if (b == 0x59) {
                    g_buf[1] = b;
                    g_bufIdx = 2;
                    g_state  = READING_PAYLOAD;
                } else {
                    // Unexpected byte; resync from the start.
                    g_state = WAIT_HEADER1;
                }
                break;

            case READING_PAYLOAD:
                g_buf[g_bufIdx++] = b;
                if (g_bufIdx >= 9) {
                    if (checksumOk(g_buf)) {
                        LidarFrame f;
                        f.distance_cm  = (uint16_t)g_buf[2] | ((uint16_t)g_buf[3] << 8);
                        f.strength     = (uint16_t)g_buf[4] | ((uint16_t)g_buf[5] << 8);
                        f.temperature  = (uint16_t)g_buf[6] | ((uint16_t)g_buf[7] << 8);
                        f.timestamp_ms = millis();
                        f.valid        = true;
                        g_latest       = f;
                        outFrame       = f;
                        newFrame       = true;
                    }
                    g_state  = WAIT_HEADER1;
                    g_bufIdx = 0;
                }
                break;
        }
    }

    return newFrame;
}

LidarFrame Lidar::latest() {
    return g_latest;
}

} // namespace ge
