#include "ota.h"
#include "ble_server.h"
#include "protocol.h"
#include <esp_ota_ops.h>
#include <esp_partition.h>

namespace ge {

namespace {

bool                       g_active        = false;
const esp_partition_t*     g_targetPart    = nullptr;
esp_ota_handle_t           g_otaHandle     = 0;
uint32_t                   g_totalSize     = 0;
uint32_t                   g_received      = 0;
uint32_t                   g_expectedCrc   = 0;
uint32_t                   g_runningCrc    = 0xFFFFFFFF;
uint32_t                   g_lastProgress  = 0;

// CRC32 over the same polynomial as zlib/PNG (the de facto standard).
uint32_t crc32Update(uint32_t crc, const uint8_t* data, size_t len) {
    crc = ~crc;
    while (len--) {
        crc ^= *data++;
        for (int i = 0; i < 8; ++i) {
            crc = (crc >> 1) ^ (0xEDB88320u & -(int32_t)(crc & 1));
        }
    }
    return ~crc;
}

void notifyError(uint8_t code) {
    uint8_t payload[2] = {GE_OTA_STATUS_ERROR, code};
    BleServer::notifyOtaStatus(payload, sizeof(payload));
}

void notifyReady() {
    uint8_t payload[1] = {GE_OTA_STATUS_READY};
    BleServer::notifyOtaStatus(payload, sizeof(payload));
}

void notifyProgress() {
    uint8_t payload[5];
    payload[0] = GE_OTA_STATUS_PROGRESS;
    memcpy(&payload[1], &g_received, sizeof(uint32_t));
    BleServer::notifyOtaStatus(payload, sizeof(payload));
}

void notifySuccess() {
    uint8_t payload[1] = {GE_OTA_STATUS_SUCCESS};
    BleServer::notifyOtaStatus(payload, sizeof(payload));
}

void reset() {
    if (g_active && g_otaHandle) {
        esp_ota_abort(g_otaHandle);
    }
    g_active       = false;
    g_targetPart   = nullptr;
    g_otaHandle    = 0;
    g_totalSize    = 0;
    g_received     = 0;
    g_expectedCrc  = 0;
    g_runningCrc   = 0xFFFFFFFF;
    g_lastProgress = 0;
}

} // namespace

void Ota::begin() {
    reset();
}

void Ota::handleStart(const uint8_t* data, size_t len) {
    if (len < 8) {
        notifyError(GE_OTA_ERR_BAD_SIZE);
        return;
    }
    if (g_active) {
        reset(); // Restart cleanly.
    }

    memcpy(&g_totalSize,   &data[0], sizeof(uint32_t));
    memcpy(&g_expectedCrc, &data[4], sizeof(uint32_t));

    g_targetPart = esp_ota_get_next_update_partition(nullptr);
    if (!g_targetPart) {
        notifyError(GE_OTA_ERR_NO_PARTITION);
        return;
    }

    esp_err_t err = esp_ota_begin(g_targetPart, g_totalSize, &g_otaHandle);
    if (err != ESP_OK) {
        notifyError(GE_OTA_ERR_WRITE_FAILED);
        return;
    }

    g_active     = true;
    g_received   = 0;
    g_runningCrc = 0;
    notifyReady();
}

void Ota::handleData(const uint8_t* data, size_t len) {
    if (!g_active) {
        notifyError(GE_OTA_ERR_BAD_STATE);
        return;
    }
    if (len == 0) return;
    if (g_received + len > g_totalSize) {
        notifyError(GE_OTA_ERR_BAD_SIZE);
        handleAbort();
        return;
    }

    esp_err_t err = esp_ota_write(g_otaHandle, data, len);
    if (err != ESP_OK) {
        notifyError(GE_OTA_ERR_WRITE_FAILED);
        handleAbort();
        return;
    }
    g_runningCrc = crc32Update(g_runningCrc, data, len);
    g_received += len;

    // Notify progress every ~4KB and at completion.
    if (g_received - g_lastProgress >= 4096 || g_received == g_totalSize) {
        g_lastProgress = g_received;
        notifyProgress();
    }
}

void Ota::handleVerify() {
    if (!g_active) {
        notifyError(GE_OTA_ERR_BAD_STATE);
        return;
    }
    if (g_received != g_totalSize) {
        notifyError(GE_OTA_ERR_BAD_SIZE);
        handleAbort();
        return;
    }
    if (g_runningCrc != g_expectedCrc) {
        notifyError(GE_OTA_ERR_CRC_MISMATCH);
        handleAbort();
        return;
    }

    esp_err_t err = esp_ota_end(g_otaHandle);
    if (err != ESP_OK) {
        notifyError(GE_OTA_ERR_WRITE_FAILED);
        reset();
        return;
    }
    err = esp_ota_set_boot_partition(g_targetPart);
    if (err != ESP_OK) {
        notifyError(GE_OTA_ERR_WRITE_FAILED);
        reset();
        return;
    }

    notifySuccess();
    delay(500); // give BLE stack a chance to flush the notification
    esp_restart();
}

void Ota::handleAbort() {
    reset();
}

bool Ota::inProgress() {
    return g_active;
}

} // namespace ge
