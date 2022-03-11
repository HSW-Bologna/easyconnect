#include <time.h>
#include "utils.h"


static const char *TAG = "Utils";


void utils_get_sys_time(struct tm *systm) {
    time_t t = time(NULL);
    *systm   = *localtime(&t);
}


void utils_set_system_time(struct tm systm) {
    struct timeval timeval;
    timeval.tv_sec  = mktime(&systm);
    timeval.tv_usec = 0;
    settimeofday(&timeval, NULL);
}