all:
	gcc main.c cJSON/cJSON.c mqtt-broker-info/broker_info.c -lmosquitto -o main

clean:
	rm main