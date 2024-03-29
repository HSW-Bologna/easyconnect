#ifndef GEL_CONF_H_INCLUDED
#define GEL_CONF_H_INCLUDED

#include "model/model.h"
#include "view/view_types.h"

/*
 * Page manager
 */
#define PMAN_NAVIGATION_DEPTH 4
#define PMAN_VIEW_NULL        0
#define PMAN_DATA_NULL        NULL

typedef view_message_t pman_message_t;

typedef view_event_t pman_event_t;

typedef void *pman_page_data_t;

typedef model_t *pman_model_t;

typedef int pman_view_t;

/*
 * PID
 */
typedef int pid_control_type_t;
typedef int pid_output_type_t;

#define GEL_PID_INTEGRAL_HISTORY_SIZE 1
#define GEL_PID_DERIVATIVE_DELTA      1

/*
 *  Watcher
 */
#define GEL_MALLOC_AVAILABLE  1

/*
 * Wear leveling
 */

#define WL_BLOCK_SIZE 4

// funzione scrivi blocco
// 1 no blocco
// -1

#endif
