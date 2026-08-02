#include "device_name_control.h"

// check if directory exists, create if doesn't X
// check if file exists , create if doesn't X
// add names to file
// delete names from file
// check if name is not taken (within the file)
// load names of discovered devices into an array of struct

int create_dir(char * filepath) {
    char path[64];
    sprintf(path,"%s/devices",filepath);
    mkdir("devices/devices",0777);
    return 0;
}

int create_file(char * filename,char * device_name) {
    char path[64];
    sprintf(path,"devices/devices/%s",filename);
    FILE *ptr = fopen(path,"r");
    if(ptr == NULL) {
        printf("Creating file %s\n",filename);
        FILE *ptr = fopen(path,"wr");
        if(ptr == NULL) {
            printf("Couldn't create file for %s!\n",filename);
            return -1;
        }
        fprintf(ptr,"DEVICE %s \nDEVICE NAME %s \n\n",filename,device_name);
        for(int i = 1;i < 9;i++) {
            fprintf(ptr,"*RELAY%d\n\n",i);
        }
        fclose(ptr);
        return 0;        
    }
    fclose(ptr);
    return 0;
}

// int relay should be 0 by default, if no other value is provided
// finish this tommorow :)
int add_relay_name(char * t,char * name,int relay) {
    char path[64];
    sprintf(path,"devices/devices/%s",t);
    FILE *ptr = fopen(path,"r+");
    if(ptr == NULL) {
        printf("Couldn't open a file %s\n", t);
        return -1;
    }
    // skip 3 lines + skip until you are on relay you need
    int star;
    char buff[128];
    char nr_buff[2];
    while((star = fgetc(ptr)) != EOF) {
        if(star == '*') {
            for(int i = 0;i < 8;i++) {
                fscanf(ptr,"%7s",buff);
                printf("buff : %s nr_buff : %c \n",buff,buff[6]);
                if(strcmp(buff,"RELAY") == 0 && buff[6] == relay+48) {
                    //fseek(ptr, 0, SEEK_CUR);
                    //fprintf(ptr,"%s\n",name);
                } else {
                    continue;
                }
            }
        } else {
            continue;
        }
    }
    
    fclose(ptr);
    return 0;
}