#include "../command_control.h"

int name(struct mosquitto* mosq,char * message,struct device_name_pair *device_name,int max_pair_count) {
    printf("Name command received !\nPayload : %s\n",message);
    // char *name = strtok(NULL,"");
    // int pos = find_relay_name(name,device_name,MAX_LINES*max_device_count);
    // if(pos == -1) {
    //     return;
    // }
    // char topic[256];
    // sprintf(topic,"cmnd/%s/POWER%d",device_name[pos].t,device_name[pos].relay);
    // publish(mosq,topic,"ON");
    return 0;
}