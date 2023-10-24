#include "esp_err.h"
#include "esp_console.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "device_commands.h"
#include "model/model.h"


static int device_commands_set_wifi_configured(int argc, char **argv);


static model_t *model_ref = NULL;


void device_commands_register(model_t *pmodel) {
    model_ref = pmodel;

    const esp_console_cmd_t wifi_cmd = {
        .command = "SetWifi",
        .help    = "Set wifi",
        .hint    = NULL,
        .func    = &device_commands_set_wifi_configured,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&wifi_cmd));
}


static int device_commands_set_wifi_configured(int argc, char **argv) {
    struct arg_end *end;
    struct arg_int *rele;
    /* the global arg_xxx structs are initialised within the argtable */
    void *argtable[] = {
        rele = arg_int1(NULL, NULL, "<value>", "Wifi"),
        end  = arg_end(1),
    };

    int nerrors = arg_parse(argc, argv, argtable);
    if (nerrors == 0) {
        model_set_wifi_configured(model_ref, rele->ival[0]);
    } else {
        arg_print_errors(stdout, end, "Wifi");
    }

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return nerrors ? -1 : 0;
}
