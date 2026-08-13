#include "broker_info.h"

// if host or port are not configured - politely DEMAND that user inputs data :)
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

    
    
    fclose(info_file);
    return 0;
}

int add_broker() {
    FILE* ptr = fopen("./mqtt-broker-info/mqtt-broker.txt","r");
    // if files exists, check if mandatory fields are filled
    if(ptr != NULL) {
        fclose(ptr);
        return 0;
    }
    
    // if file doesn't exist, create one and ask user to input info
    
    printf("Alles good\n");
    FILE *ptr2 = fopen("./mqtt-broker-info/mqtt-broker.txt","w");
    char username[100];
    char password[100];
    char host[100];
    char port[16];
    printf("Enter username (Press enter to skip)\n");
    fgets(username,sizeof(username),stdin);
    fprintf(ptr2,"username %s\n",username);

    printf("Enter password (Press enter to skip)\n");
    fgets(password,sizeof(password),stdin);
    fprintf(ptr2,"password %s\n",password);

    // cannot be empty
    printf("Enter host (Mandatory)\n");
    do {
        fgets(host,sizeof(host),stdin);
        if(strlen(host) == 1) {
            printf("Hose cannot be empty ! Enter valid host !\n");
        }
    } while(strlen(host) == 1);
    fprintf(ptr2,"host %s\n",host);

    // cannot be empty
    printf("Enter port (Mandatory)\n");
    do {
        fgets(port,sizeof(port),stdin);
        if(strlen(port) == 1) {
            printf("Port cannot be empty ! Enter valid port(default - 1883) !\n");
        }
    } while(strlen(port) == 1);
    fprintf(ptr2,"port %s\n",port);

    fclose(ptr2);
    return 0;
}