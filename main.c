#include "main.h"

struct Device *devices;
struct device_name_pair *device_name;
int max_device_count;
int curr_device_count;

void publish(struct mosquitto *mosq,char topic[],char payload[]){
    int rc;
    int payload_len = strlen(payload);
    rc = mosquitto_publish(mosq,NULL,topic,payload_len,payload,2,false);
    if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
	}
}

void subscribe(struct mosquitto *mosq,char topic[]) {
    int rc;
    rc = mosquitto_subscribe(mosq,NULL,topic,0);
    if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
	}
}

void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
    printf("Client on_connect : %s\n",mosquitto_connack_string(reason_code));
    if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}
}

void on_publish(struct mosquitto *mosq, void *obj, int mid){
    //printf("Message with mid %d has been published.\n",mid);
}

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos){
    int i;
	bool have_subscription = false;

    // connect to topics
    for(i=0; i<qos_count; i++){
		printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
		if(granted_qos[i] <= 2){
			have_subscription = true;
		}
	}

    // if no connections remain - disconnect
	if(have_subscription == false){
		fprintf(stderr, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
	}
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg){
    printf("Receiveddd msg : %s %s\n\n", msg->topic,(char *)msg->payload);
    
    //device discovery
    if(strncmp(msg->topic,"tasmota/discovery",17)==0) {
        //check if it is config topic
        int last_ind = strlen(msg->topic)-1;
        char conf[6] = "config";
        int j = 5;
        for(int i = last_ind;i > last_ind-6;i--) {
            //printf("%c %c \n",msg->topic[i],conf[j]);
            if(conf[j] == msg->topic[i]) {
                j--;
                continue;
            }
            //printf("Isn't a config\n");
            return;
        }
        //printf("Is a config\n");
        for(int i = 0;i < max_device_count;i++) {
            if(devices[i].subscribed == false) {
                cJSON *json = cJSON_Parse((char *)msg->payload);
                printf("Parsing json\n");
                cJSON *t = cJSON_GetObjectItem(json, "t");
                printf("Parsing t : %s\n",t->valuestring);
                cJSON *dn = cJSON_GetObjectItem(json, "dn");
                printf("Parsing dn\n");
                cJSON *fn = cJSON_GetObjectItem(json, "fn"); // an array

                create_dir("devices");
                create_file(t->valuestring,dn->valuestring);
                for(int j = 0;j < 8;j++) {
                    cJSON *fname = cJSON_GetArrayItem(fn,j);
                    if(!cJSON_IsNull(fname)) {
                        printf("%s\n",fname->valuestring);
                        strcpy(devices[i].fn[j],fname->valuestring);
                        //add_relay_name(t->valuestring,fname->valuestring,j+1);
                    }
                }
                
                strcpy(devices[i].t,t->valuestring);
                strcpy(devices[i].dn,dn->valuestring);
                

                // printf("value test %s %s \n",devices[i].t,t->valuestring);
                // printf("dn test %s %s \n",devices[i].dn,dn->valuestring);
                
                devices[i].subscribed = true;
                char subscribe_to[64];
                snprintf(subscribe_to,sizeof(subscribe_to),"+/%s/#",t->valuestring);
                subscribe(mosq,subscribe_to);
                curr_device_count++;
                // Map names to ids' ?
                
                // Notify about new device

                cJSON_Delete(json);
                break;
            }
        }
        return;
    }

    // commands for now :
    // name *t* *name*
    if(strncmp(msg->topic,"assistant",9) == 0) {
        printf("Command received yo !\n");
        //printf("Receiveddd msg : %s %s\n\n", msg->topic,(char *)msg->payload);
        
        //command , relay_name
        // включить *имя*
        // выключить *имя*
        // назвать *t* *relay* *имя* . 
        // забыть *имя*
        char message[256];
        strcpy(message,(char*)msg->payload);
        //printf("Payload : %s\n",message);
        char *command = strtok(message," ");
        printf("Command : %s\n",command);
        //printf("назвать strcmp : %d",strcmp("назвать",command));
        if(strcmp("включить",command) == 0 || strcmp("turnon",command) == 0) {
            char *name = strtok(NULL,"");
            int pos = find_relay_name(name,device_name,MAX_LINES*max_device_count);
            if(pos == -1) {
                return;
            }
            char topic[256];
            sprintf(topic,"cmnd/%s/POWER%d",device_name[pos].t,device_name[pos].relay);
            publish(mosq,topic,"ON");
            return;
        }

        if(strcmp("выключить",command)  == 0 || strcmp("turnoff",command) == 0) {
            char *name = strtok(NULL,"");
            int pos = find_relay_name(name,device_name,MAX_LINES*max_device_count);
            if(pos == -1) {
                return;
            }
            char topic[256];
            sprintf(topic,"cmnd/%s/POWER%d",device_name[pos].t,device_name[pos].relay);
            publish(mosq,topic,"OFF");
            return;
        }

        if(strcmp("назвать",command)  == 0 || strcmp("name",command) == 0) {

            char *t = strtok(NULL," ");
            char *name = strtok(NULL,"");
            printf("%s | %s\n",t,name);
            int relay = 1;
            if(name[0] >= '0' && name[0] <= '8') {
                printf("Parsing relay\n");
                relay = name[0]-'0';
                printf("Relay : %d\n",relay);
                char *asd = strtok(name," ");
                asd = strtok(NULL,"");
                strcpy(name,asd);
            }
            printf("%s %s %d\n",t,name,relay);
            add_relay_name(t,name,relay,device_name,MAX_LINES*max_device_count);

            return;
        }

        if(strcmp("забыть",command)  == 0 || strcmp("forget",command) == 0) {
            char *name = strtok(NULL," ");
            remove_relay_name(name,device_name,MAX_LINES*max_device_count);
            return;
        }
        
    }

    //answers from broker.
    if(strncmp(msg->topic,"stat",4) == 0){
        // Get power value
        printf("%s\n",(char *)msg->payload);
        printf("Power Changed : %s\n",(char *)msg->payload);
        return;
    }

    if(strncmp(msg->topic,"tele",4) == 0) {

    }

    
}

void find_command() {

}

void execute_command() {

}

// mosq topic payload
int main() {
    max_device_count = 20;
    devices = (struct Device*)calloc(max_device_count,sizeof(struct Device));
    curr_device_count = 0;
    init_devices(devices,max_device_count);

    int rc;
    struct mosquitto* mosq;
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL,true,NULL);
    if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

    mosquitto_connect_callback_set(mosq,on_connect);
    mosquitto_message_callback_set(mosq,on_message);
    mosquitto_publish_callback_set(mosq,on_publish);
    mosquitto_subscribe_callback_set(mosq,on_subscribe);

    // connect to broker
    char info[4][128];
    int ans = get_broker_info(info);
    if(ans == -1) {
        return -1;
    }
    if(strcmp(info[0],"") !=0 && strcmp(info[1],"") !=0) {
        mosquitto_username_pw_set(mosq,info[0],info[1]);
    }
    
    rc = mosquitto_connect(mosq,info[2],atoi(info[3]),60);

    if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Client Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

    rc = mosquitto_loop_start(mosq);
    if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Client Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}


    // subscribe to discovery topic
    subscribe(mosq,"tasmota/discovery/#");

    //print device list
    sleep(1);
    print_device_list(devices,max_device_count);
    // subscribe to topic of every device
    // subscribe to assistants topic
    subscribe(mosq,"assistant/#");

    
    // read all names given to devices and save them to aray
    device_name = (struct device_name_pair*)calloc(MAX_LINES*max_device_count,sizeof(struct device_name_pair));
    init_device_name_pair(device_name,devices,MAX_LINES*max_device_count,curr_device_count);

    //topic device_id payload
    while(1) {
        //printf("%d - dev count \n",curr_device_count);
        printf("Awaiting command : \n");
        char input[256];
        //make a config file for assistant later :)
        // maybe include it in mqtt-broker.txt 
        char topic[] = "assistant";

        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Remove trailing newline
            input[strcspn(input, "\n")] = '\0';
        
            printf("You entered: '%s'\n", input);
        }

        // all commands are sent to topic cmnd/#
        publish(mosq, topic, input);
    }

    mosquitto_lib_cleanup();
    free(devices);
    free(device_name);
    return 0;
}