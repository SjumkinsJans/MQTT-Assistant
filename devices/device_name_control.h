#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAX_LINES 80
#define MAX_LINE_LEN 64
#define RELAY_COUNT 8

typedef enum {
    TASMOTA = 1,
    ZIGBEE = 2,
} DEVICE_TYPE;

struct device_name_pair {
    char t[48];
    int relay;
    char name[32];
};

struct Device {
    char t[48];         // mac addr last 6 syms
    char dn[64];        // device name
    char fn[8][64];     // friendly name
    DEVICE_TYPE type;           // tasmota or zigbee
    char parent[32];
    bool subscribed;    // is it currently subscribed to broker ?
};

int create_dir(char * filepath);
int create_file(char * filename,char * device_name);
int check_file_exist(char * dir,char * filename);
void init_device_name_pair(struct device_name_pair **pairs,struct Device *devices,int *max_pairs,int device_count);
void print_device_name_pair(struct device_name_pair *arr,int max_pairs);
int check_name_availability(char *name,struct device_name_pair *pairs,int max_pairs);
int add_relay_name(char * t,char * name,int relay,struct device_name_pair **pairs,int *max_pairs);
int find_relay_name(char * name,struct device_name_pair *pairs,int max_pairs);
int remove_relay_name(char * name,struct device_name_pair *pairs,int max_pairs);