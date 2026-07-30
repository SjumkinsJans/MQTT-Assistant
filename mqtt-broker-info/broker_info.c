#include "broker_info.h"


int get_broker_info(char info[4][128]) {
    FILE* info_file = fopen("./mqtt-broker-info/mqtt-broker.txt","r");
    if(info_file == NULL) {
        printf("Couldn't open mqtt-broker file !\n");
        printf("Check if mqtt-broker.txt file exitsts !\n");
        return -1;
    }
    for(int i = 0;i < 4;i++) {
        strcpy(info[i],"");
    }
    char buff[128];
    while(fscanf(info_file,"%127s",buff) == 1) {
        //printf("%s\n",buff);
        if(strcmp("username",buff) == 0) {
            fscanf(info_file,"%127s",info[0]);
        }
        else if(strcmp("password",buff) == 0) {
            fscanf(info_file,"%127s",info[1]);   
        }
        else if(strcmp("host",buff) == 0) {
            fscanf(info_file,"%127s",info[2]);
        }
        else if(strcmp("port",buff) == 0) {
            fscanf(info_file,"%127s",info[3]);
        }
    }
    if(strcmp(info[2],"")==0) {
        printf("Please configure your mqtt host !\n");
        return -1;
    }

    if(strcmp(info[3],"")==0) {
        printf("Please configure your mqtt port !\n");
        return -1;
    }
    fclose(info_file);
    return 0;
}