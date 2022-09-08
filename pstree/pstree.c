#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>


typedef struct{

    size_t pid, ppid;
    char *pname;

}PData;


/* hashmap related */
typedef struct{
    size_t key;
    PData pdata;

}DataType; 

typedef struct{
    DataType data;
    struct HashNode *next;
}HashNode;
typedef struct{
    int size;
    HashNode *table;
}HashMap;


/* hashmap end */
typedef struct ProcTree{
    PData pdata;
    struct ProcTree *parent;
    struct ProcTree **childs;
}ptree;


enum {
    VERSION, BYNAME, PIDCONTAIN
};


const char *proc_dir = "/proc";
const char *version = "pstree (PSmisc) BLmimic\n\
                       Copyright (C) 1993-2019 Werner Almesberger and Craig Small\n \
                       \n\
                       PSmisc comes with ABSOLUTELY NO WARRANTY.\n\
                       This is free software, and you are welcome to redistribute it under\n\
                       the terms of the GNU General Public License.\n\
                       For more information about these matters, see the files named COPYING.";
int flag=BYNAME;
void print_pstree();

DIR * FD;


struct dirent * in_file;

int main(int argc, char *argv[]) {
    assert(argv[0]);


    for (int i = 1; i < argc; i++) {
        assert(argv[i]);

        if(!strcmp(argv[i], "-V")){
            printf("%s\n", version);
            return 0;
        }
        else if(!strcmp(argv[i], "-p")){
            flag=PIDCONTAIN;
            print_pstree();
            return 0;
        }else if(!strcmp(argv[i], "-n")){
            flag = BYNAME;
            print_pstree();
        }else{
            printf("No such argument: %s\n", argv[i]);
        }
    }
    assert(!argv[argc]);
    return 0;
}

void print_pstree() {
    if(flag==VERSION){

        printf("%s\n", version);
        return;
    }

    if(NULL == (FD = opendir(proc_dir))){
        fprintf(stderr, "Error: Failed to open input directory - %s\n", strerror(errno));
        return ; 
    }
    FD = opendir(proc_dir);


    /* for pnode data store*/
    int pdata_idx=0;
    PData pdata_store[128];


    while(NULL!=(in_file= readdir(FD))){
        if(!strcmp(in_file->d_name, "."))
            continue;
        if(!strcmp(in_file->d_name, ".."))
            continue;
        if(in_file->d_type != 4)
            continue;
        if(!isdigit(in_file->d_name[0]))
            continue; 
        char stat_path[100];
        stat_path[0] = '\0';
        strcat(stat_path, proc_dir);
        strcat(stat_path, "/");
        strcat(stat_path, in_file->d_name);
        strcat(stat_path, "/stat");
        //printf("%s\n", stat_path);
        /***********************************************/



        int c;
        long  lSize;
        char * buffer  = malloc(512*sizeof(char));

        FILE *fp = fopen(stat_path,"r");
        if(!fp){
            perror("File opening failed");
            continue;
            //exit(1);
        }


        int cur_idx = 0;
        while(EOF!=(c = fgetc(fp))){
            buffer[cur_idx] = c;
            cur_idx++; 

        }

        int space_idx=0;
        char* token;
        char* string;
        char* tofree;

        //ptree *leaf = malloc(sizeof(ptree));
        string = strdup(buffer);
        tofree = string;

        size_t pid,ppid;
        char *pname=malloc(64);

        // printf("total string:\n%s\n",string);

        while(NULL!=(token = strsep(&string, " "))){
            //printf("space_idx:%d\n",space_idx);
            if(space_idx==4)break;

            if(space_idx==0){
                pid = atoi(token); 
            }
            if(space_idx==1){
                strcpy(pname, token); 
            }
            if(space_idx==3){
                ppid = atoi(token);
            }



            space_idx++;
        }
        //printf("proc_stat:%d      ",atoi(in_file->d_name));
        printf("pid:%zu\t",pid);
        printf("token:%s\t", pname);
        printf("ppid:%zu\n", ppid);

        pdata_store[pdata_idx].pid = pid;
        pdata_store[pdata_idx].ppid = ppid;
        pdata_store[pdata_idx].pname = malloc(strlen(pname));
        strcpy(pdata_store[pdata_idx].pname, pname);
        printf("pdata_name: %s\n", pdata_store[pdata_idx].pname);


        free(tofree);
        fclose(fp);
        free(buffer);
        free(pname);
        //exit(1);

    } 



}

//HashMap * createHashMap(){


//}
