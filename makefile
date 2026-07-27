all:
	gcc main.c yasub.c yapub.c -lmosquitto -lpthread -o main yasub yapub

clean:
	rm main yasub yapub