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
    1. init mosquitto and create new client instance
    2. set/configure callbacks
    3. Connect to broker
    4. subscribe to topics
    4.x. receive msgs
    5. clean up and exit
*/

void on_connect(struct mosquitto *mosq, void *obj, int reason_code){
    int rc;
    printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
    if(reason_code != 0){
		mosquitto_disconnect(mosq);
	}

    /* Making subscriptions in the on_connect() callback means that if the
	 * connection drops and is automatically resumed by the client, then the
	 * subscriptions will be recreated when the client reconnects. */
	rc = mosquitto_subscribe(mosq, NULL, "example/temperature", 1);
	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		/* We might as well disconnect if we were unable to subscribe */
		mosquitto_disconnect(mosq);
	}
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
    printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

int main(int argc,char *argv[]) {
    int rc;
    struct mosquitto *mosq;

    UNUSED(argc);
    UNUSED(argv);

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL,true,NULL);
    if(mosq == NULL) {
        fprintf(stderr, "Error: Out of memory.\n");
        return 1;
    }

    mosquitto_connect_callback_set(mosq,on_connect);
    mosquitto_subscribe_callback_set(mosq,on_subscribe);
    mosquitto_message_callback_set(mosq,on_message);
        
    rc = mosquitto_connect(mosq, "test.mosquitto.org", 1883, 60);
    if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

    mosquitto_loop_forever(mosq, -1, 1);
	mosquitto_lib_cleanup();

    return 0;
}