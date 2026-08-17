#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON/cJSON.h"
#include "mqtt-broker-info/broker_info.h"
#include "commands/command_control.h"
//#include "devices/device_name_control.h" // imported from command_control.h

// using calloc, so no need for this func
void init_devices(struct Device* devices,int max_device_count) {
    for(int i = 0;i < max_device_count;i++) {
        for(int j = 0;j < 8;j++) {
            strcpy(devices[i].fn[j],"");
        }
    }
}

void print_device_list(struct Device *devices,int max_device_count) {
    printf("Printing device list : \n");
    for(int i = 0;i < max_device_count;i++) {
        if(devices[i].subscribed == true) {
            printf("============\n");
            printf("t : %s\n",devices[i].t);
            printf("Device name : %s\n",devices[i].dn);
            for(int j = 0;j < 8;j++) {
                if(strcmp(devices[i].fn[j],"") != 0) {
                    printf("RELAY%d : %s\n",j+1,devices[i].fn[j]);
                }
            }
            printf("============\n");
        }
    }
    return;
}

int find_substring(char * source,char * substring) {
    if(strcmp(source,"") == 0 || strcmp(substring,"") == 0) {
        printf("find_substring : Neither of input strings can be empty !\n");
        return -1;
    }
    int src_len = strlen(source);
    int ss_len = strlen(substring);

    if(ss_len > src_len) {
        //printf("Substring cannot be longer than source string ! \n");
        return -1;
    }

    if(ss_len == src_len) {
        if(strcmp(source,substring) == 0) {
            return 0;
        } else {
            //printf("Substring not found !\n");
            return -1;
        }
    }

    for(int i = 0;i <= src_len-ss_len;i++) {      
        int found = 1;
        for(int j = 0;j < ss_len;j++) {
            //printf("%c %c \n",source[i+j],substring[j]);
            if(source[i+j] != substring[j] ) {
                found = 0;
                break;
            }
        }
        if(found) { 
            // return index where substring starts in source string
            return i;
        }
    }
    return -1;
}