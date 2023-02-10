#include <assert.h>
#include <stdint.h>
#include "peripherals/storage.h"
#include "esp_log.h"
#include "model/model.h"
#include "gel/data_structures/watcher.h"
#include "peripherals/tft.h"
#include "utils/utils.h"


#define NUM_CONFIGURATION_PARAMETERS 13


static void save_backlight(void *mem, void *arg);
static void save_byte_array(void *mem, void *arg);
static void save_double_byte_array(void *mem, void *arg);


static const char *DEVICE_MAP_KEY                           = "DMAP";
static const char *LANGUAGE_KEY                             = "LANGUAGE";
static const char *BACKLIGHT_KEY                            = "BACKLIGHT";
static const char *BUZZER_KEY                               = "BUZZER";
static const char *DEGREES_KEY                              = "DEGREES";
static const char *UVC_FILTERS_KEY                          = "UVCFILT";
static const char *ESF_FILTERS_KEY                          = "ESFFILT";
static const char *SIPHONING_PERCENTAGES_KEY                = "SIPPERC";
static const char *IMMISSION_PERCENTAGES_KEY                = "IMMPERC";
static const char *CLEANING_START_PERIOD_KEY                = "CLEANSTART";
static const char *CLEANING_FINISH_PERIOD_KEY               = "CLEANFINISH";
static const char *NUM_PASSIVE_FILTERS_KEY                  = "PASSIVENUM";
static const char *PASSIVE_FILTERS_HOURS_THRESHOLD_WARN_KEY = "PASSTHRESWARN";
static const char *PASSIVE_FILTERS_HOURS_THRESHOLD_STOP_KEY = "PASSTHRESSTOP";

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
    storage_load_uint16(&pmodel->configuration.environment_cleaning_start_period, (char *)CLEANING_START_PERIOD_KEY);
    storage_load_uint16(&pmodel->configuration.environment_cleaning_finish_period, (char *)CLEANING_START_PERIOD_KEY);
    storage_load_uint16(&pmodel->configuration.num_passive_filters, (char *)NUM_PASSIVE_FILTERS_KEY);
    storage_load_uint16(&pmodel->configuration.passive_filters_hours_warning_threshold,
                        (char *)PASSIVE_FILTERS_HOURS_THRESHOLD_WARN_KEY);
    storage_load_uint16(&pmodel->configuration.passive_filters_hours_stop_threshold,
                        (char *)PASSIVE_FILTERS_HOURS_THRESHOLD_STOP_KEY);
    storage_load_uint8(&pmodel->configuration.use_fahrenheit, (char *)DEGREES_KEY);
    storage_load_blob(&pmodel->configuration.uvc_filters_for_speed, MAX_FAN_SPEED, (char *)UVC_FILTERS_KEY);
    storage_load_blob(&pmodel->configuration.esf_filters_for_speed, MAX_FAN_SPEED, (char *)ESF_FILTERS_KEY);
    storage_load_blob(&pmodel->configuration.siphoning_percentages, MAX_FAN_SPEED * 2,
                      (char *)SIPHONING_PERCENTAGES_KEY);
    storage_load_blob(&pmodel->configuration.immission_percentages, MAX_FAN_SPEED * 2,
                      (char *)IMMISSION_PERCENTAGES_KEY);

    size_t i       = 0;
    watchlist[i++] = WATCHER(&pmodel->configuration.language, storage_save_uint16, (void *)LANGUAGE_KEY);
    watchlist[i++] =
        WATCHER_DELAYED(&pmodel->configuration.active_backlight, save_backlight, (void *)BACKLIGHT_KEY, 1000UL);
    watchlist[i++] = WATCHER(&pmodel->configuration.buzzer_volume, storage_save_uint16, (void *)BUZZER_KEY);
    watchlist[i++] = WATCHER(&pmodel->configuration.use_fahrenheit, storage_save_uint8, (void *)DEGREES_KEY);
    watchlist[i++] = WATCHER_DELAYED(&pmodel->configuration.num_passive_filters, storage_save_uint16,
                                     (void *)NUM_PASSIVE_FILTERS_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED(&pmodel->configuration.passive_filters_hours_warning_threshold,
                                     storage_save_uint16, (void *)PASSIVE_FILTERS_HOURS_THRESHOLD_WARN_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED(&pmodel->configuration.passive_filters_hours_stop_threshold, storage_save_uint16,
                                     (void *)PASSIVE_FILTERS_HOURS_THRESHOLD_STOP_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED(&pmodel->configuration.environment_cleaning_start_period, storage_save_uint16,
                                     (void *)CLEANING_START_PERIOD_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED(&pmodel->configuration.environment_cleaning_finish_period, storage_save_uint16,
                                     (void *)CLEANING_FINISH_PERIOD_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED_ARRAY(&pmodel->configuration.uvc_filters_for_speed, MAX_FAN_SPEED, save_byte_array,
                                           (void *)UVC_FILTERS_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED_ARRAY(&pmodel->configuration.esf_filters_for_speed, MAX_FAN_SPEED, save_byte_array,
                                           (void *)ESF_FILTERS_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED_ARRAY(&pmodel->configuration.siphoning_percentages, MAX_FAN_SPEED,
                                           save_double_byte_array, (void *)SIPHONING_PERCENTAGES_KEY, 4000UL);
    watchlist[i++] = WATCHER_DELAYED_ARRAY(&pmodel->configuration.siphoning_percentages, MAX_FAN_SPEED,
                                           save_double_byte_array, (void *)IMMISSION_PERCENTAGES_KEY, 4000UL);
    assert(i == NUM_CONFIGURATION_PARAMETERS);
    watchlist[i++] = WATCHER_NULL;
    watcher_list_init(watchlist);
}


void configuration_save(model_t *pmodel) {
    ESP_LOGI(TAG, "Saving address map");
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


static void save_byte_array(void *mem, void *arg) {
    storage_save_blob(mem, MAX_FAN_SPEED, arg);
}


static void save_double_byte_array(void *mem, void *arg) {
    storage_save_blob(mem, MAX_FAN_SPEED * 2, arg);
}