#include "controller.h"
#include "model/model.h"
#include "view/view.h"


void controller_init(model_t *model) {
    (void)model;
    view_start(model);
}