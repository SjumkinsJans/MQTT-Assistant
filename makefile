all:
	gcc main.c cJSON.c -lmosquitto -lpthread -o main

clean:
	rm main