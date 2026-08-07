#include "command_control.h"

void init_command_list(struct Command *command_list,int command_count) {
    for(int i = 0;i < command_count;i++) {
        strcpy(command_list[i].command_variant,"");
        command_list[i].command_id = 0;
    }
}

void get_commands(struct Command *command_list,int command_count) {
     struct dirent *dp;
     DIR *dfd;

     char *dir;
     strcpy(dir,".");
}