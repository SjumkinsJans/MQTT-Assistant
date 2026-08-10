#include "command_control.h"

void init_command_list(struct Command *command_list,int command_count) {
    for(int i = 0;i < command_count;i++) {
        strcpy(command_list[i].command_variant,"");
        command_list[i].command_id = 0;
    }
}


typedef void (*plugin_func)();
plugin_func plugs[2];
int load_command(char *name,int i) {
    char path[256];
    //sprintf(path,"./%s/%s.os",name,name);
    sprintf(path,"./hello/%s.os",name);
    void *handle = dlopen(path,RTLD_LAZY);
    if(handle == NULL) {
        printf("handle ERROR %s!\n",dlerror());
        return EXIT_FAILURE;
    }
    plugin_func command = dlsym(handle,name);
    if(command == NULL) {
        printf("g ERROR %s!\n",dlerror());
        return EXIT_FAILURE;
    }

    plugs[i] = command;
    //g();

    //dlclose(handle);
}

void traverse_dirs(char *path) {
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

        if(!S_ISDIR(path_stat.st_mode)) {
            printf("%s\n",de->d_name);
            continue;
        }
        traverse_dirs(longpath);
        free(longpath);
    }

    //path to file
    
    closedir(dir);
}

//typedef void (*hello)();
int main() {
    // load_command("hello",0);
    // load_command("hi",1);

    // plugs[0]();
    // plugs[1]();
    traverse_dirs(".");
}