#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

enum comm_id {
    FORGET = 1,
    NAME = 2,
    TURN_ON = 3,
    TURN_OFF = 4
};

struct Command {
    char command_variant[32];
    int command_id;
};

void init_command_list(struct Command *command_list,int command_count);

void get_commands(struct Command *command_list,int command_count);