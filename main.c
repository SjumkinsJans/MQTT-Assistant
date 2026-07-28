#include "main.h"

struct Device *devices;
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
    printf("Received msg : %s %s\n", msg->topic,(char *)msg->payload);
    if(strncmp(msg->topic,"tasmota/discovery",17)==0) {
        //check if it is config topic
        int last_ind = strlen(msg->topic)-1;
        char conf[6] = "config";
        int j = 5;
        for(int i = last_ind;i > last_ind-6;i--) {
            printf("%c %c \n",msg->topic[i],conf[j]);
            if(conf[j] == msg->topic[i]) {
                j--;
                continue;
            }
            printf("Isn't a config\n");
            return;
        }
        printf("Is a config\n");
        for(int i = 0;i < max_device_count;i++) {
            if(devices[i].subscribed == false) {
                cJSON *json = cJSON_Parse((char *)msg->payload);
                printf("Parsing json\n");
                cJSON *t = cJSON_GetObjectItem(json, "t");
                printf("Parsing t : %s\n",t->valuestring);
                cJSON *dn = cJSON_GetObjectItem(json, "dn");
                printf("Parsing dn\n");
                cJSON *fn = cJSON_GetObjectItem(json, "fn"); // an array
                for(int j = 0;j < 7;j++) {
                    cJSON *fname = cJSON_GetArrayItem(fn,j);
                    if(cJSON_IsString(fname)) {
                        printf("%s\n",fname->valuestring);
                    } else {
                        break;
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

                // Map names to ids' ?
                
                // Notify about new device

                cJSON_Delete(json);
                break;
            }
        }
        return;
    }

    if(strncmp(msg->topic,"stat",4)){
        // Get power value
        printf("Power Changed : %s\n",(char *)msg->payload);
        return;
    }
}

void print_device_list(struct Device *devices,int max_device_count) {
    printf("Printing device list : \n");
    for(int i = 0;i < max_device_count;i++) {
        if(devices[i].subscribed == true) {
            printf("============\n");
            printf("t : %s\n",devices[i].t);
            printf("Device name : %s\n",devices[i].dn);
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

    mosquitto_username_pw_set(mosq,"alex","3RrG-+H+WG");

    mosquitto_connect_callback_set(mosq,on_connect);
    mosquitto_message_callback_set(mosq,on_message);
    mosquitto_publish_callback_set(mosq,on_publish);
    mosquitto_subscribe_callback_set(mosq,on_subscribe);

    rc = mosquitto_connect(mosq,"e.tgt.lv",1883,60);

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

    //topic device_id payload
    while(1) {
        printf("Awaiting command : \n");
        char input[256];
        char topic[128] = "";
        char dev_id[32] = "";
        char command[32] = "";
        char payload[32] = "";

        fgets(input, sizeof(input), stdin);

        int count = sscanf(input, "%s %s %s %s",
                           topic, dev_id, command, payload);

        if(count < 3)
            continue;

        if(strcmp(topic,"cmnd") == 0) {
            sprintf(topic, "cmnd/%s/%s", dev_id, command);
        }
        else if(strcmp(topic,"stat") == 0) {
            sprintf(topic, "stat/%s/%s", dev_id, command);
        }

        if(count == 4)
            printf("Payload: %s\n", payload);
        else
            printf("Payload is empty\n");

        publish(mosq, topic, count == 4 ? payload : "");
    }

    mosquitto_lib_cleanup();
    free(devices);
    return 0;
}