#include "../command_control.h"


void turn_off(struct mosquitto* mosq,char * message,char * command,struct device_name_pair **device_name,int *max_pair_count,struct Device **devices,int max_device_count) {
    char *name = remove_trailing_whitespace(trim(strremove(message,command)));
    int pos = find_relay_name(name,(*device_name),(*max_pair_count));
    if(pos == -1) {
        return;
    }
    char topic[256];
    
    int type;
    struct Device device;
    for(int i = 0;i < max_device_count;i++) {
        if(strcmp((*devices)[i].t,(*device_name)[pos].t)==0) {
            type = (*devices)[i].type;
            device = (*devices)[i];
        }
    }
    printf("%s type is : %d\n",(*device_name)[pos].t,type);
    switch(type) {
        case TASMOTA:
            sprintf(topic,"cmnd/%s/POWER%d",(*device_name)[pos].t,(*device_name)[pos].relay);
            publish(mosq,topic,"OFF");
            break;
        case ZIGBEE:
            char payload[256];
            char *shortaddr;
            shortaddr = strtok(device.t,"_");
            shortaddr = strtok(NULL,"_");
            shortaddr = strtok(NULL,"");
            sprintf(topic,"cmnd/%s/ZbSend",device.parent);
            sprintf(payload,"{\"Device\":\"%s\",\"Send\":{\"Power\":0}}",shortaddr);
            //printf("Publishing : %s %s\n",topic,payload);
            publish(mosq,topic,payload);
            break;   
    } 

    return;
}