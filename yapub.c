#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef UNUSED
#  define UNUSED(A) (void)(A)
#endif

/*
    -1. init mosquitto
    0. Create callbacks
    1. init mosquitto and create a new client instance
    2. configure callbacks
    3. Connect to broker
    4. publish msgs
    5. clean up and exit 
*/

void on_connect(struct mosquitto *mosq, void *obj, int reason_code){
    printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
    if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}
}

void on_publish(struct mosquitto *mosq, void *obj, int mid){
    printf("Message with mid %d has been published.\n",mid);
}

int get_data() {
    sleep(3);
    return (int)random()%100;
}

void publish_something(struct mosquitto *mosq) {
    char payload[20];
    int temp;
    int rc;
    temp = get_data();
    snprintf(payload, sizeof(payload), "%d", temp);
    rc = mosquitto_publish(mosq,NULL,"example/temperature",(int)strlen(payload),payload,2,false);
    if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
	}
}

int main(int argc, char *argv[]) {
    struct mosquitto *mosq;
    int rc;

    UNUSED(argc);
    UNUSED(argv);
    
    mosquitto_lib_init();

	mosq = mosquitto_new(NULL,true,NULL);
    if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

    mosquitto_connect_callback_set(mosq,on_connect);
    mosquitto_publish_callback_set(mosq,on_publish);

    
	rc = mosquitto_connect(mosq, "test.mosquitto.org", 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

    // loop should actually be on a separate thread
    rc = mosquitto_loop_start(mosq);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

    // publish
    while(1) {
        publish_something(mosq);
    }
        
    mosquitto_lib_cleanup();
    return 0;
}