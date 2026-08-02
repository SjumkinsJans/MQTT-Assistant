#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

struct device_name_pair {
    char t[32];
    int relay;
    char name[32];
};

int create_dir();
int create_file(char * filename,char * device_name);
int check_file_exist(char * dir,char * filename);
int add_relay_name(char * t,char * name,int relay);