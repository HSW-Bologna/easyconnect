#ifndef INTL_H_INCLUDED
#define INTL_H_INCLUDED

#include "AUTOGEN_FILE_strings.h"
#include "model/model.h"


const char *view_intl_get_string(model_t *pmodel, strings_t id);
const char *view_intl_get_string_from_language(uint16_t language, strings_t id);


#endif