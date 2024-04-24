#include "sensors_calibration.h"
#include "utils/utils.h"
#include "gel/timer/timecheck.h"
#include "esp_log.h"


#define STABILIZING_TIME 50000UL


static size_t get_sensor_index(model_t *pmodel, uint8_t address);
static void   add_pressure(model_t *pmodel, pressure_sensor_t *sensor, int16_t pressure);


static const char *TAG = "Calibration";

static unsigned long reading_ts               = 0;
static unsigned long stabilization_ts         = 0;
static unsigned long stabilization_timeout_ts = 0;


void sensors_calibration_manage(model_t *pmodel) {
    switch (pmodel->sensor_calibration.state) {
        case CALIBRATION_STATE_STABILIZING:
            if (is_expired(stabilization_ts, get_millis(), STABILIZING_TIME)) {
                reading_ts = get_millis();
                ESP_LOGI(TAG, "Reading values");
                pmodel->sensor_calibration.state = CALIBRATION_STATE_READING;
            }

            if (is_expired(stabilization_timeout_ts, get_millis(), STABILIZING_TIME + 10000UL)) {
                ESP_LOGI(TAG, "First timeout");
                pmodel->sensor_calibration.state = CALIBRATION_STATE_STABILIZING_TIMEOUT;
            }
            break;

        case CALIBRATION_STATE_STABILIZING_TIMEOUT:
            if (is_expired(stabilization_ts, get_millis(), STABILIZING_TIME)) {
                reading_ts = get_millis();
                ESP_LOGI(TAG, "Reading values");
                pmodel->sensor_calibration.state = CALIBRATION_STATE_READING;
            }
            break;

        case CALIBRATION_STATE_READING:
            if (is_expired(reading_ts, get_millis(), 10000UL)) {
                ESP_LOGI(TAG, "Calibration attempt");
                model_calibrate_pressures(pmodel);

                if (model_get_excessive_offset_sensor(pmodel) < 0) {
                    ESP_LOGI(TAG, "Calibration done");
                    pmodel->sensor_calibration.state = CALIBRATION_STATE_DONE;
                } else {
                    ESP_LOGI(TAG, "Calibration retry");
                    pmodel->sensor_calibration.state = CALIBRATION_STATE_READING_RETRY;
                }

                reading_ts = get_millis();
            }
            break;

        case CALIBRATION_STATE_READING_RETRY:
            if (is_expired(reading_ts, get_millis(), 10000UL)) {
                model_calibrate_pressures(pmodel);
                ESP_LOGI(TAG, "Calibration attempt");

                if (model_get_excessive_offset_sensor(pmodel) < 0) {
                    ESP_LOGI(TAG, "Calibration done");
                    pmodel->sensor_calibration.state = CALIBRATION_STATE_DONE;
                }

                reading_ts = get_millis();
            }
            break;

        case CALIBRATION_STATE_DONE:
            free(pmodel->sensor_calibration.sensors);
            pmodel->sensor_calibration.num_sensors = 0;
            pmodel->sensor_calibration.sensors     = NULL;
            break;
    }
}


void sensors_calibration_new_reading(model_t *pmodel, uint8_t address, int16_t pressure) {
    switch (pmodel->sensor_calibration.state) {
        case CALIBRATION_STATE_DONE:
            return;
        default:
            break;
    }

    size_t             sensor_index = get_sensor_index(pmodel, address);
    pressure_sensor_t *sensor       = &pmodel->sensor_calibration.sensors[sensor_index];

    add_pressure(pmodel, sensor, pressure);

    ESP_LOGI(TAG, "Added new pressure for %i, %iPa (%zu)", address, pressure, sensor->pressure_index);

    if (!model_is_pressure_sensor_stable(pmodel, sensor_index)) {
        stabilization_ts = get_millis();
    }
}


static size_t get_sensor_index(model_t *pmodel, uint8_t address) {
    int found = -1;

    for (size_t i = 0; i < pmodel->sensor_calibration.num_sensors; i++) {
        if (pmodel->sensor_calibration.sensors[i].address == address) {
            found = i;
            break;
        }
    }

    if (found < 0) {
        found = pmodel->sensor_calibration.num_sensors;
        pmodel->sensor_calibration.num_sensors++;
        pmodel->sensor_calibration.sensors = realloc(
            pmodel->sensor_calibration.sensors, pmodel->sensor_calibration.num_sensors * sizeof(pressure_sensor_t));
        pressure_sensor_t *sensor = &pmodel->sensor_calibration.sensors[found];
        sensor->address           = address;
        sensor->pressure_index    = 0;
        sensor->loop              = 0;
        memset(&sensor->pressures, 0, sizeof(sensor->pressures));
    }

    return (size_t)found;
}


static void add_pressure(model_t *pmodel, pressure_sensor_t *sensor, int16_t pressure) {
    sensor->pressures[sensor->pressure_index] = pressure;
    sensor->pressure_index                    = sensor->pressure_index + 1;
    if (sensor->pressure_index >= NUM_CALIBRATION_READINGS) {
        sensor->loop           = 1;
        sensor->pressure_index = 0;
    }
}
