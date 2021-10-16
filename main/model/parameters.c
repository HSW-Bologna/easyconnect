#include <assert.h>
#include "model.h"


#define LVL_USER 0x01


void model_parameters_init(model_t *pmodel) {
    assert(pmodel != NULL);

    pmodel->parameters[0] = PARAMETER(&pmodel->active_backlight, 0, 100, 100, 0, LVL_USER);
    pmodel->parameters[1] = PARAMETER(&pmodel->standby_backlight, 0, 100, 100, 0, LVL_USER);
    pmodel->parameters[2] = PARAMETER(&pmodel->standby_time, 0, 100, 100, 0, LVL_USER);

    parameter_check_ranges(pmodel->parameters, NUM_PARAMETERS);
}