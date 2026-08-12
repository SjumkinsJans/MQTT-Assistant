#include "../command_control.h"


// message should comes in format : *t* *relay* *name* 
int name(struct mosquitto* mosq,char * message,char * command,struct device_name_pair *device_name,int max_pair_count) {
    char *payload = remove_trailing_whitespace(trim(strremove(message,command)));
    char* t = strtok(payload," ");
    char *a = strtok(NULL," ");
    if(a[0] >= '0' && a[0] <= '9') {
        char *b = strtok(NULL," ");
        int relay = a[0]-'0';
        add_relay_name(t,b,relay,device_name,max_pair_count);
        return 0;
    }
    add_relay_name(t,a,1,device_name,max_pair_count);
    return 0;
}