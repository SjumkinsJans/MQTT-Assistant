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

typedef void (*plugin_func)(struct mosquitto* mosq,char * message,char * command,struct device_name_pair **device_name,int *max_pair_count,struct Device **devices,int device_count);
struct Command {
    char command_variant[64];
    plugin_func command;
};

static char *strremove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
};

static char *trim(char *str) {
    size_t count = 0;
    while (str[count] == ' '){
        count++;
    }

    memmove(str, str + count, strlen(str + count) + 1);
    return str;
}

static char *remove_trailing_whitespace(char *str) {
    size_t len = strlen(str);
    int pos = len-1;
    while(str[pos] == ' ') {
        pos--;
    }
    
    if(pos == len-1) {
        return str;
    }

    str[++pos] = '\0';
    return str;
}

static void publish(struct mosquitto *mosq,char topic[],char payload[]){
    int rc;
    int payload_len = strlen(payload);
    rc = mosquitto_publish(mosq,NULL,topic,payload_len,payload,2,false);
    if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
	}
}

static int device_type(char *device_t,struct Device *devices,int max_device_count) {
    for(int i = 0;i < max_device_count;i++) {
        if(strcmp(devices[i].t,device_t)==0) {
            return devices[i].type;
        }
    }
    return -1;
}

void init_command_list(struct Command *command_list,int command_count);
int load_command(char *path,struct Command **command_list,int *command_count,int *max_command_count);
void traverse_dirs(char *path,struct Command **command_list,int *command_count,int *max_command_count);