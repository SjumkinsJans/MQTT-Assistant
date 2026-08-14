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
    char line[256];

    while (fgets(line, sizeof(line), info_file))
    {
        char key[128];
        char value[128];

        if (sscanf(line, "%127s %127s", key, value) == 2)
        {
            if (strcmp(key, "username") == 0)
                strcpy(info[0], value);

            else if (strcmp(key, "password") == 0)
                strcpy(info[1], value);

            else if (strcmp(key, "host") == 0)
                strcpy(info[2], value);

            else if (strcmp(key, "port") == 0)
                strcpy(info[3], value);
        }
    }

    fclose(info_file);
    
    printf("username : %s\n",info[0]);
    printf("password : %s\n",info[1]);
    printf("host : %s\n",info[2]);
    printf("port : %s\n",info[3]);   

    int rewrite = 0;
    while (strlen(info[2]) == 0) {
        rewrite = 1;
        printf("Please enter a host: ");

        if (fgets(info[2], sizeof(info[2]), stdin) == NULL)
            return -1;

        info[2][strcspn(info[2], "\n")] = '\0';

        if (strlen(info[2]) > 0)
            break;
    }

    while (strlen(info[3]) == 0) {
        rewrite = 1;
        printf("Please enter port: ");

        if (fgets(info[3], sizeof(info[3]), stdin) == NULL)
            return -1;

        info[3][strcspn(info[3], "\n")] = '\0';

        if (strlen(info[3]) > 0)
            break;
    }

    if(rewrite) {
        FILE *ptr = fopen("./mqtt-broker-info/mqtt-broker.txt","w");
        fprintf(ptr,"username %s\n",info[0]);
        fprintf(ptr,"password %s\n",info[1]);
        fprintf(ptr,"host %s\n",info[2]);
        fprintf(ptr,"port %s\n",info[3]);
        fclose(ptr);
    }
 
    
    
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