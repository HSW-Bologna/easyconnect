#include <assert.h>
#include <stdint.h>
#include "peripherals/storage.h"
#include "esp_log.h"
#include "model/model.h"
#include "gel/data_structures/watcher.h"
#include "peripherals/tft.h"
#include "utils/utils.h"


#define NUM_CONFIGURATION_PARAMETERS 4


static void save_backlight(void *mem, void *arg);


static const char *DEVICE_MAP_KEY = "DMAP";
static const char *LANGUAGE_KEY   = "LANGUAGE";
static const char *BACKLIGHT_KEY  = "BACKLIGHT";
static const char *BUZZER_KEY     = "BUZZER";
static const char *DEGREES_KEY    = "DEGREES";

static const char *TAG = "Devices";

static watcher_t watchlist[NUM_CONFIGURATION_PARAMETERS + 1];


void configuration_load(model_t *pmodel) {
    address_map_t map = {0};

    if (storage_load_blob(&map, sizeof(map), (char *)DEVICE_MAP_KEY)) {
        ESP_LOGW(TAG, "Unable to load device map!");
    }

    size_t address = 1;
    while (address_map_find(&map, address, &address)) {
        ESP_LOGI(TAG, "Found saved device at %zu", address);
        model_new_device(pmodel, (uint8_t)address);
        address++;
    }

    storage_load_uint16(&pmodel->configuration.language, (char *)LANGUAGE_KEY);
    storage_load_uint16(&pmodel->configuration.active_backlight, (char *)BACKLIGHT_KEY);
    storage_load_uint16(&pmodel->configuration.buzzer_volume, (char *)BUZZER_KEY);
    storage_load_uint8(&pmodel->configuration.use_fahrenheit, (char *)DEGREES_KEY);

    size_t i       = 0;
    watchlist[i++] = WATCHER(&pmodel->configuration.language, storage_save_uint16, (void *)LANGUAGE_KEY);
    watchlist[i++] =
        WATCHER_DELAYED(&pmodel->configuration.active_backlight, save_backlight, (void *)BACKLIGHT_KEY, 1000UL);
    watchlist[i++] = WATCHER(&pmodel->configuration.buzzer_volume, storage_save_uint16, (void *)BUZZER_KEY);
    watchlist[i++] = WATCHER(&pmodel->configuration.use_fahrenheit, storage_save_uint8, (void *)DEGREES_KEY);
    assert(i == NUM_CONFIGURATION_PARAMETERS);
    watchlist[i++] = WATCHER_NULL;
    watcher_list_init(watchlist);
}


void configuration_save(model_t *pmodel) {
    address_map_t map = model_get_address_map(pmodel);
    storage_save_blob(&map, sizeof(map), (char *)DEVICE_MAP_KEY);
}


void configuration_manage(void) {
    watcher_process_changes(watchlist, get_millis());
}


static void save_backlight(void *mem, void *arg) {
    storage_save_uint16(mem, arg);
    uint16_t value = *(uint16_t *)mem;
    tft_backlight_set(value);
}