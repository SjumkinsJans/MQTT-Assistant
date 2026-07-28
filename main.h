#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

struct Device {
    char t[17];         // mac addr last 6 syms
    char dn[32];        // device name
    char fn[8][32];     // friendly name
                        // add user defined names ?
    bool subscribed;    // is it currently subscribed to broker ?
};