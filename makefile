all:
	gcc main.c cJSON/cJSON.c mqtt-broker-info/broker_info.c devices/device_name_control.c commands/command_control.c -lmosquitto -rdynamic -o main

clean:
	rm main