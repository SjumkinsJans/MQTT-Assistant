#include "../command_control.h"

// forget *name* or *name* forget
int forget(struct mosquitto* mosq,char * message,char * command,struct device_name_pair **device_name,int max_pair_count) {
    char *name = remove_trailing_whitespace(trim(strremove(message,command)));
    remove_relay_name(name,device_name,max_pair_count);
}