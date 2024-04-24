#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


#include <stdint.h>
#include "model/model.h"


#define LOG_TARGET_SELF                         0
#define LOG_DESCRIPTION_NULL                    0
#define LOG_POWER_ON()                          log_event(LOG_EVENT_POWER_ON, LOG_TARGET_SELF, LOG_DESCRIPTION_NULL)
#define LOG_DEVICE_COMMUNICATION_ERROR(address) log_event(LOG_EVENT_COMMUNICATION_ERROR, address, LOG_DESCRIPTION_NULL)
#define LOG_DEVICE_ALARM(address, alarm)        log_event(LOG_EVENT_ALARM, address, alarm)



void     log_init(void);
void     log_event(log_event_code_t code, uint8_t target, uint16_t description);
uint32_t log_read(log_t *logs, uint32_t from, uint32_t num);

#endif
