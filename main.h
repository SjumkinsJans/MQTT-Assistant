#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON/cJSON.h"
#include "mqtt-broker-info/broker_info.h"
#include "devices/device_name_control.h"

struct Device {
    char t[32];         // mac addr last 6 syms
    char dn[32];        // device name
    char fn[8][32];     // friendly name
                        // add user defined names ?
                        // add relay counter ? or count manually each time ?
    bool subscribed;    // is it currently subscribed to broker ?
};

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

void init_devices(struct Device* devices,int max_device_count) {
    for(int i = 0;i < max_device_count;i++) {
        for(int j = 0;j < 8;j++) {
            strcpy(devices[i].fn[j],"");
        }
    }
}