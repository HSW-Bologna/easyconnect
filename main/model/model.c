#include <assert.h>
#include "model.h"


void model_init(model_t *pmodel) {
    assert(pmodel != NULL);

    pmodel->slider      = 0;
    pmodel->temperature = 32;
}


int model_get_temperature(model_t *pmodel) {
    assert(pmodel != NULL);
    return pmodel->temperature;
}