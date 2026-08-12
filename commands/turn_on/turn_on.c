#include "../command_control.h"

void turn_on(struct mosquitto* mosq,char * message,char * command,struct device_name_pair *device_name,int max_pair_count) {
    char *name = remove_trailing_whitespace(trim(strremove(message,command)));
    int pos = find_relay_name(name,device_name,max_pair_count);
    if(pos == -1) {
        return;
    }
    char topic[256];
    sprintf(topic,"cmnd/%s/POWER%d",device_name[pos].t,device_name[pos].relay);
    publish(mosq,topic,"ON");
    return;
}