#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

struct Device {
    char id[32];
    char name[32];
    char topic[64];
    bool subscribed;
};