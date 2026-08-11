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

typedef void (*plugin_func)(struct mosquitto* mosq,char * message,struct device_name_pair *device_name,int max_pair_count);
struct Command {
    char command_variant[64];
    plugin_func command;
};

void init_command_list(struct Command *command_list,int command_count);
int load_command(char *path,struct Command *command_list,int *command_count);
void traverse_dirs(char *path,struct Command *command_list,int *command_count);