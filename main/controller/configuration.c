#include <stdint.h>
#include "peripherals/storage.h"
#include "esp_log.h"
#include "model/model.h"


#define DEVICE_MAP_SIZE     (256 / 64)
#define DEVICE_MAP_INDEX(i) (i / 64)
#define DEVICE_MAP_SHIFT(i) (i % 64)
#define DEVICE_MAP_KEY      "DMAP"


static const char *TAG = "Devices";


void configuration_load(model_t *pmodel) {
    address_map_t map = {0};

    if (storage_load_blob(&map, sizeof(map), DEVICE_MAP_KEY)) {
        ESP_LOGW(TAG, "Unable to load device map!");
    }

    size_t address = 1;
    while (address_map_find(&map, address, &address)) {
        ESP_LOGI(TAG, "Found saved device at %zu", address);
        model_new_device(pmodel, (uint8_t)address);
        address++;
    }
}


void configuration_save(model_t *pmodel) {
    address_map_t map = model_get_address_map(pmodel);
    storage_save_blob(&map, sizeof(map), DEVICE_MAP_KEY);
}