#include "command_control.h"



// traverse plugin folders
// find .so files
// use load commands on .so files
// read command names from their .txt files
// add pairs of {name;function}

// using calloc, so no need for this func
void init_command_list(struct Command *command_list,int command_count) {
    for(int i = 0;i < command_count;i++) {
        strcpy(command_list[i].command_variant,"");
    }
}

void get_command_name(char *name) {
    int last_pos = -1;
    int len = strlen(name);
    
    for(int i = 0;i < len;i++) {
        if (name[i] == '/') {
            last_pos = i;
        }
    }

    char command[64];
    int j = last_pos+1;
    int pos = 0;
    while(name[j] != '.') {
        //printf("A\n");
        command[pos] = name[j];
        j++;
        pos++;
    }
    command[pos] = '\0';
    strcpy(name,command);
}

int load_command(char *path,struct Command **command_list,int *command_count,int *max_command_count) {
    //printf("Path :%s \n",name);
    void *handle = dlopen(path,RTLD_LAZY);
    if(handle == NULL) {
        printf("handle ERROR %s!\n",dlerror());
        return EXIT_FAILURE;
    }

    char name[128];
    strcpy(name,path);
    get_command_name(name);

    plugin_func command = dlsym(handle,name);
    if(command == NULL) {
        printf("g ERROR %s!\n",dlerror());
        return EXIT_FAILURE;
    }

    // now, load all the names :)
    size_t len = strlen(path)+2;
    char *filename = malloc(len);
    snprintf(filename,len-4,"%s",path);
    sprintf(filename,"%s.txt",filename);
    FILE *ptr = fopen(filename,"r");
    
    if(ptr == NULL) {
        printf("load_command : Couldn't open a file %s\n", filename);
        free(filename);
        return -1;
    }
    
    char command_name[64];
    while(fgets(command_name,64,ptr)) {
        //printf("%d %s\n",(*command_count),command_name);

        command_name[strlen(command_name)-1] = '\0';
        (*command_list)[(*command_count)].command = command;
        strcpy((*command_list)[(*command_count)].command_variant,command_name);
        (*command_count)++;
        
        // if command count == max command count, realloc
        if(*command_count == *max_command_count) {
            printf("load command : allocating more memory for commands !\n");
            size_t old_count = *max_command_count;
            *max_command_count +=100;
            size_t new_count = *max_command_count;
            
            struct Command *tmp = (struct Command*)realloc(*command_list,*max_command_count*sizeof(struct Command));
            if(tmp != NULL) {
                *command_list = tmp;
                memset((*command_list)+old_count,0,(new_count-old_count)*sizeof(*command_list));
            }

            printf("load_command : memory reallocated : %d\n",*max_command_count);
        }
    
    }
    free(filename);
    fclose(ptr);
    //g();

    //dlclose(handle);
}

void traverse_dirs(char *path,struct Command **command_list,int *command_count,int *max_command_count) {
    DIR *dir = opendir(path);
    struct dirent *de;
    if(dir == NULL) {
        return;
    }

    int error_check;
    struct stat path_stat;
    while((de = readdir(dir)) != NULL) {
        if(strcmp(de->d_name,".") == 0 || strcmp(de->d_name,"..") == 0) {
            continue;
        }

        // two additional bytes for '\0'
        size_t len = strlen(path)+strlen(de->d_name)+2;
        char *longpath = malloc(len);
        snprintf(longpath, len, "%s/%s", path, de->d_name);


        error_check = lstat(longpath,&path_stat);
        if(error_check == -1) {
            perror("lstat");
            free(longpath);
            continue;
        }

        //ignore symbolic-links
        if(S_ISLNK(path_stat.st_mode)) {
            free(longpath);
            continue;
        }

        // check if file is .so file
        if(!S_ISDIR(path_stat.st_mode)) {
            int d_name_len = strlen(de->d_name);
            if(d_name_len < 3) {
                continue;
            }
          
            if(de->d_name[d_name_len-1] != 's' || de->d_name[d_name_len-2] != 'o' || de->d_name[d_name_len-3] != '.') {
                continue;
            }
            //printf("%s\n",longpath);
            load_command(longpath,command_list,command_count,max_command_count);
            continue;
        }
        traverse_dirs(longpath,command_list,command_count,max_command_count);
        free(longpath);
    }

    //path to file
    
    closedir(dir);
}