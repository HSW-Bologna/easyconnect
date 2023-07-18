#ifndef TEMPERATURE_H_INCLUDED
#define TEMPERATURE_H_INCLUDED


void   temperature_init(void);
double temperature_get(void);
double temperature_get_humidity(void);
uint8_t temperature_error(void);


#endif
