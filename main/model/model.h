#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <stdint.h>

typedef struct {
    int slider;

    int temperature;

} model_t;


void model_init(model_t *model);
int  model_get_temperature(model_t *pmodel);

#endif