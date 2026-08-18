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
    sprintf(path,"devices/devices/%s.txt",filename);
    FILE *ptr = fopen(path,"r");
    if(ptr == NULL) {
        printf("Creating file %s\n",filename);
        FILE *ptr = fopen(path,"w+");
        if(ptr == NULL) {
            printf("Couldn't create file for %s!\n",filename);
            return -1;
        }
        fprintf(ptr,"DEVICE %s\nDEVICE NAME %s\n\n",filename,device_name);

        for(int i = 1;i < RELAY_COUNT+1;i++) {
            fprintf(ptr,"*RELAY%d\n\n",i);

        }
        fprintf(ptr,"#\n");
        for(int i = 0;i < MAX_LINES-RELAY_COUNT*2-4;i++) {
            fprintf(ptr,"\n");
        }
        fclose(ptr);
        return 0;        
    }
    fclose(ptr);
    return 0;
}

int get_file_content(char * filename, char lines[MAX_LINES][MAX_LINE_LEN]) {
    char buff[128];
    sprintf(buff,"devices/devices/%s.txt",filename); 
    
    FILE *ptr = fopen(buff,"r");
    if(ptr == NULL) {
        printf("Couldn't open a file %s\n", filename);
        return -1;
    }

    int count = 0;
    int where_names_end = 0; 
    int found_end = 0;
    while(count < MAX_LINES && fgets(lines[count],MAX_LINE_LEN,ptr)) {
        if(!found_end) {
            where_names_end++;
        }
        if(lines[count][0] == '#') {
            found_end = 1;
        }

        count++;
    }
    fclose(ptr);

    return where_names_end;
}

void init_device_name_pair(struct device_name_pair **pairs,struct Device *devices,int *max_pairs,int device_count) {
    int counter = 0;
    for(int i = 0;i < device_count;i++) {
        char txt[128];
        //strcpy(txt,devices[i].t);
        sprintf(txt,"devices/devices/%s.txt",devices[i].t);
        FILE* ptr = fopen(txt,"r");
        if(ptr == NULL) {
            printf("Config file for %s couldnt be found or openend !\n",devices[i].t);
            continue;
        }
       
        char buff[128];
        int current_relay;
        while(fgets(buff,MAX_LINE_LEN,ptr) && buff[0] != '#') {
            if(buff[0] == '*') {
                current_relay = buff[6];
                while(fgets(buff,MAX_LINE_LEN,ptr) && buff[0] != '\n') {
                    (*pairs)[counter].relay = current_relay-'0';
                    int index_of_newline = strcspn(buff,"\n"); 
                    buff[index_of_newline] = '\0';
                    strcpy((*pairs)[counter].name,buff);
                    strcpy((*pairs)[counter].t,devices[i].t);
                    counter++;
                    if(counter == (*max_pairs)) {
                        //realloc
                        printf("init_device_name_pair : allocating more memory for device_name pairs !\n");
                        size_t old_count = *max_pairs;
                        *max_pairs +=100;
                        size_t new_count = *max_pairs;

                        struct device_name_pair *tmp = (struct device_name_pair*)realloc(*pairs,*max_pairs*sizeof(struct device_name_pair));
                        if(tmp != NULL) {
                            *pairs = tmp;
                            memset((*pairs)+old_count,0,(new_count-old_count)*sizeof(**pairs));
                        }
                        printf("init_device_name_pair : memory reallocated for: %d pairs\n",*max_pairs);
                    }
                }
            }
        }

        fclose(ptr);
    }
    
    for(int i = counter;i < (*max_pairs);i++) {
        strcpy((*pairs)[i].name,"");
        (*pairs)[i].relay = 0;
        strcpy((*pairs)[i].t,"");
    }
}

void print_device_name_pair(struct device_name_pair *arr,int max_pairs) {
    printf("Printing list of Device-Name pairs :\n");
    for(int i = 0;i < max_pairs;i++) {
        if(arr[i].relay != 0) {
            printf("======Device-Name Pair========\n");
            printf("Device : %s\n",arr[i].t);
            printf("RELAY : %d\n",arr[i].relay);
            printf("Name : %s\n",arr[i].name);
        }
    }
    printf("==============================\n");
}

int check_name_availability(char *name,struct device_name_pair *pairs,int max_pairs) {
    for(int i = 0;i < max_pairs;i++) {
        // if(strcmp(pairs[i].name,"") != 0) {
        //     printf("strcmp : %d | %s %s \n",strcmp(name,pairs[i].name),pairs[i].name,name);
        // }
        if(strcmp(name,pairs[i].name) == 0) {
            printf("Name %s already belongs to device %s !\n",name,pairs[i].t);
            return -1;
        }
    }
    return 0;
}

// int relay should be 0 by default, if no other value is provided
// should also check whether the name is already taken by some device
int add_relay_name(char * t,char * name,int relay,struct device_name_pair **pairs,int *max_pairs) {
    if(relay > 8 || relay < 1) {
        return -1;
    }
    //check name availability
    if(check_name_availability(name,(*pairs),(*max_pairs)) == -1 ) {
        return -1;
    }
    char lines[MAX_LINES][MAX_LINE_LEN];
    int count = get_file_content(t,lines);
    printf("%d\n",count);
    if(count == -1) {
        return -1;
    }
    if(count == MAX_LINES) {
        printf("You cannot assign any more names to this device (%s) ! \n Try deleting some names !(forger *name*) \n",t);
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
    
    // for(int i = 0;i < MAX_LINES;i++) {
    //     printf("%s",lines[i]);
    // }

    char path[64];
    sprintf(path,"devices/devices/%s.txt",t);
    FILE *ptr = fopen(path,"w");
    if(ptr == NULL) {
        printf("Couldn't open a file %s\n", t);
        return -1;
    }
    
    for(int i = 0;i < MAX_LINES;i++) {
        fprintf(ptr,"%s",lines[i]);
    }
    fclose(ptr);

    int found = 0;
    for(int i = 0;i < (*max_pairs);i++) {
        if((*pairs)[i].relay == 0) {
            (*pairs)[i].relay = relay;
            strcpy((*pairs)[i].t,t);
            strcpy((*pairs)[i].name,name);
            found = 1;
            break;
        }
    }

    // realloc
    if(!found) {
        printf("add_relay_name : allocating more memory for device_name pairs !\n");
        size_t old_count = *max_pairs;
        *max_pairs +=100;
        size_t new_count = *max_pairs;
        
        struct device_name_pair *tmp = (struct device_name_pair*)realloc(*pairs,*max_pairs*sizeof(struct device_name_pair));
        if(tmp != NULL) {
            *pairs = tmp;
            memset((*pairs)+old_count,0,(new_count-old_count)*sizeof(**pairs));
        }
        printf("add_relay_name : memory reallocated for: %d pairs\n",*max_pairs);
        (*pairs)[old_count].relay = relay;
        strcpy((*pairs)[old_count].t,t);
        strcpy((*pairs)[old_count].name,name); 
    }
    return 0;
}

//returns the index of a name in pairs array if it exists in it.
int find_relay_name(char * name,struct device_name_pair *pairs,int max_pairs) {
    for(int i = 0;i < max_pairs;i++) {
        if(strcmp(name,pairs[i].name) == 0) {
            printf("Name %s belongs to %s relay %d \n",pairs[i].name,pairs[i].t,pairs[i].relay);
            return i;
        }
    }
    printf("Name %s coudln't be found !\n",name);
    return -1;
}

int remove_relay_name(char * name,struct device_name_pair *pairs,int max_pairs) {
    int pos = find_relay_name(name,pairs,max_pairs);
    if(pos == -1) {
        return -1;
    }

    char lines[MAX_LINES][MAX_LINE_LEN];
    int count = get_file_content(pairs[pos].t,lines);
    printf("%d\n",count);
    if(count == -1) {
        return -1;
    }

    int name_len = strlen(name);
    int found = 0;
    int index;
    for(int i = 0;i < count;i++) {
         for(int j = 0;j < name_len;j++) {
            if(lines[i][j] != name[j]) {
                break;
            }
            found = 1;
         }
         if(found) {
            index = i;
            break;
         }
    }
    if(!found) {
        printf("Couldn't find name to  remove ! \n");
        return -1;
    }
    printf("Name found !\n");
    for(int i = index;i < count;i++) {
        strcpy(lines[i],lines[i+1]);
    }

    char path[64];
    sprintf(path,"devices/devices/%s.txt",pairs[pos].t);
    FILE *ptr = fopen(path,"w");
    if(ptr == NULL) {
        printf("Couldn't open a file %s\n", pairs[pos].t);
        return -1;
    }
    
    for(int i = 0;i < MAX_LINES;i++) {
        fprintf(ptr,"%s",lines[i]);
    }
    fclose(ptr);

    //also delete from pairs. Or, rather, make the spot free to take
    strcpy(pairs[pos].name,"");
    pairs[pos].relay = 0;
    strcpy(pairs[pos].t,"");

    return 0;
}