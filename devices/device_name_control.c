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
        fprintf(ptr,"DEVICE %s\nDEVICE NAME %s\n\n",filename,device_name);
        for(int i = 1;i < 9;i++) {
            fprintf(ptr,"*RELAY%d\n\n",i);

        }
        fclose(ptr);
        return 0;        
    }
    fclose(ptr);
    return 0;
}


int get_file_content(char * filename, char lines[MAX_LINES][MAX_LINE_LEN]) {
    char buff[128];
    sprintf(buff,"devices/devices/%s",filename); 
    
    FILE *ptr = fopen(buff,"r");
    if(ptr == NULL) {
        printf("Couldn't open a file %s\n", filename);
        return -1;
    }

    int count = 0;
    while(count < MAX_LINES && fgets(lines[count],MAX_LINE_LEN,ptr)) {count++;}
    fclose(ptr);

    return count;
}

// int relay should be 0 by default, if no other value is provided
// finish this tommorow :)
int add_relay_name(char * t,char * name,int relay) {
    printf("Number : %d \n",relay);
    char lines[MAX_LINES][MAX_LINE_LEN];
    int count = get_file_content(t,lines);
    printf("%d\n",count);
    if(count == -1) {
        return -1;
    }
    


    char buff[128];
    sprintf(buff,"%s\n",name);
    // skip 3 lines + skip until you are on relay you need
    for(int i = 0;i < count;i++) {
        if(lines[i][0] == '*' && lines[i][6] == relay+'0') {
            for(int k = count ; k > i+1;k--)  {
                strcpy(lines[k],lines[k-1]);
            }
            strcpy(lines[i+1],buff);
            count++;
            break;
        }
    }
    
    for(int i = 0;i < count;i++) {
        printf("%s",lines[i]);
    }
    char path[64];
    sprintf(path,"devices/devices/%s",t);
    FILE *ptr = fopen(path,"w");
    if(ptr == NULL) {
        printf("Couldn't open a file %s\n", t);
        return -1;
    }

    for(int i = 0;i < count+1;i++) {
        fprintf(ptr,"%s",lines[i]);
    }
    
    fclose(ptr);
    return 0;
}