#include <stdint.h>
#include "peripherals/storage.h"
#include "esp_log.h"


#define DEVICE_MAP_SIZE     (256 / 64)
#define DEVICE_MAP_INDEX(i) (i / 64)
#define DEVICE_MAP_SHIFT(i) (i % 64)
#define DEVICE_MAP_KEY      "DMAP"


static const char *TAG                         = "Devices";
static uint64_t    device_map[DEVICE_MAP_SIZE] = {0};


void devices_load(void) {
    if (storage_load_blob_option(device_map, sizeof(device_map), DEVICE_MAP_KEY)) {
        ESP_LOGW(TAG, "Unable to load device map!");
    }
}


void devices_save(void) {
    storage_save_blob_option(device_map, sizeof(device_map), DEVICE_MAP_KEY);
}