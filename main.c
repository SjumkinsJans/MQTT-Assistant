#include "main.h"

struct Device *devices;
struct device_name_pair *device_name;
int max_device_count;
int curr_device_count;
struct Command *command_list;
int curr_command_count;
int max_command_count;
int max_device_name_pairs;


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
		//printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
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
    //printf("Receiveddd msg : %s %s\n\n", msg->topic,(char *)msg->payload);
    
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
        
        //if couldn't find place for device - realloc AND init memory !
        int found = 0;
        int pos = 0;
        for(pos;pos < max_device_count;pos++) {
            if(devices[pos].subscribed == false) {
                found = 1;
                break;
            }
        }
        
        if(!found) {
            size_t old_count = max_device_count;
            max_device_count +=10;
            size_t new_count = max_device_count;
            printf("on_message : Allocating more memory for devices !\n");
            
            struct Device *tmp = (struct Device*)realloc(devices,max_device_count*sizeof(struct Device));
            if(tmp != NULL) {
                devices = tmp;
                memset(devices+old_count,0,(new_count-old_count)*sizeof(*devices));
            }

            printf("on_message : memory reallocated : %d\n",max_device_count);
        }

        // add device
        cJSON *json = cJSON_Parse((char *)msg->payload);
        //printf("Parsing json\n");
        cJSON *t = cJSON_GetObjectItem(json, "t");
        //printf("Parsing t : %s\n",t->valuestring);
        cJSON *dn = cJSON_GetObjectItem(json, "dn");
        //printf("Parsing dn\n");
        cJSON *fn = cJSON_GetObjectItem(json, "fn"); // an array

        create_dir("devices");
        create_file(t->valuestring,dn->valuestring);
        for(int j = 0;j < 8;j++) {
            cJSON *fname = cJSON_GetArrayItem(fn,j);
            if(!cJSON_IsNull(fname)) {
                //printf("%s\n",fname->valuestring);
                strcpy(devices[pos].fn[j],fname->valuestring);
                //add_relay_name(t->valuestring,fname->valuestring,j+1);
            }
        }
        
        strcpy(devices[pos].t,t->valuestring);
        strcpy(devices[pos].dn,dn->valuestring);
        

        // printf("value test %s %s \n",devices[i].t,t->valuestring);
        // printf("dn test %s %s \n",devices[i].dn,dn->valuestring);
        
        devices[pos].subscribed = true;
        char subscribe_to[64];
        snprintf(subscribe_to,sizeof(subscribe_to),"+/%s/#",t->valuestring);
        subscribe(mosq,subscribe_to);
        curr_device_count++;
        // Notify about new device

        cJSON_Delete(json);


        return;
    }

    // commands
    if(strncmp(msg->topic,"assistant",9) == 0) {
        char message[256];
        strcpy(message,(char*)msg->payload);
        
        for(int i = 0;i < curr_command_count;i++) {
            if(find_substring(message,command_list[i].command_variant) >= 0) {
                printf("Command found: %s\n",command_list[i].command_variant);

                // execute command
                command_list[i].command(mosq,message,command_list[i].command_variant,&device_name,max_device_name_pairs);
                break;
            }
        }

        // check if it is reload command. It is easier to call it there.
        if(strcmp(message,"reload") == 0) {
            printf("Reloading plugins !\n");
            traverse_dirs("./commands",&command_list,&curr_command_count,&max_command_count);
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

// mosq topic payload
int main() {
    max_device_count = 20;
    devices = (struct Device*)calloc(max_device_count,sizeof(struct Device));
    if(devices == NULL) {
        printf("devices : Couldn't allocate memory !\n");
        return -1;
    }
    curr_device_count = 0;
    //init_devices(devices,max_device_count);

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
    add_broker();
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
    //print_device_list(devices,max_device_count);

    // subscribe to topic of every device
    // subscribe to assistants topic
    subscribe(mosq,"assistant/#");

    
    // read all names given to devices and save them to aray
    max_device_name_pairs = 10;
    device_name = (struct device_name_pair*)calloc(max_device_name_pairs,sizeof(struct device_name_pair));
    if(device_name == NULL) {
        printf("device_name : Couldn't allocate memory !\n");
        return -1;
    }    
    init_device_name_pair(device_name,devices,max_device_name_pairs,curr_device_count);

    // load in available commands
    curr_command_count = 0;
    max_command_count = 1000;
    command_list = (struct Command*)calloc(max_command_count,sizeof(struct Command));
    if(command_list == NULL) {
        printf("command_list : Couldn't allocate memory !\n");
        return -1;
    }    
    //init_command_list(command_list,1000);
    traverse_dirs("./commands",&command_list,&curr_command_count,&max_command_count);

    // for(int i = 0; i < 50;i++) {
    //     printf("%s %s\n",device_name[i].name,device_name[i].t);
    // }

    printf("Command count : %d\n",curr_command_count);

    printf("curr device count : %d\n",curr_device_count);
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
    free(command_list);
    return 0;
}