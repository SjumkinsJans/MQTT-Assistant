#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dlfcn.h>
#include "../mqtt-broker-info/broker_info.h"
#include "../devices/device_name_control.h"


struct Command {
    char command_variant[32];
    int command_id;
};

void init_command_list(struct Command *command_list,int command_count);
int load_command(char *name,int i);
void traverse_dirs(char *path);