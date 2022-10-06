#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define POOT_NAME "/proc"
#define MAX_PROC_NUM 1024
#define MAX_PROC_NAME_LEN 256

typedef struct _proc{
    int pid,ppid;
    char proc_name[MAX_PROC_NAME_LEN];
    int count;

} Proc;

Proc  proc[MAX_PROC_NUM];
int proc_count=0;

enum {
    VERSION, BYNAME, PIDCONTAIN
};


const char *version = "pstree (PSmisc) BLmimic\n\
                       Copyright (C) 1993-2019 Werner Almesberger and Craig Small\n \
                       \n\
                       PSmisc comes with ABSOLUTELY NO WARRANTY.\n\
                       This is free software, and you are welcome to redistribute it under\n\
                       the terms of the GNU General Public License.\n\
                       For more information about these matters, see the files named COPYING.";
int flag = BYNAME;

void print_pstree();

DIR *FD;

struct dirent *in_file;

void print_pstree() {
    if (flag == VERSION) {
        printf("%s\n", version);
        return;
    }

    if (NULL == (FD = opendir(POOT_NAME))) {
        fprintf(stderr, "Error: Failed to open input directory - %s\n",
                strerror(errno));
        return;
    }
    FD = opendir(POOT_NAME);

    /* for pnode data store*/
    int proc_count = 0;
    while (NULL != (in_file = readdir(FD))) {

        if (!strcmp(in_file->d_name, "."))
            continue;
        if (!strcmp(in_file->d_name, ".."))
            continue;
        if (in_file->d_type != 4)
            continue;
        if (!isdigit(in_file->d_name[0]))
            continue;
        char stat_path[100];
        stat_path[0] = '\0';
        strcat(stat_path, POOT_NAME);
        strcat(stat_path, "/");
        strcat(stat_path, in_file->d_name);
        strcat(stat_path, "/stat");

        int c;
        long lSize;
        char *buffer = malloc(512 * sizeof(char));

        FILE *fp = fopen(stat_path, "r");
        if (!fp) {
            perror("File opening failed");
        }

        int cur_idx = 0;
        while (EOF != (c = fgetc(fp))) {
            buffer[cur_idx] = c;
            cur_idx++;
        }

        int space_idx = 0;
        char *token;
        char *string;
        char *tofree;

        // ptree *leaf = malloc(sizeof(ptree));
        string = strdup(buffer);
        tofree = string;

        size_t pid = 114514, ppid = 1919810;
        char *pname = malloc(64);


        while (NULL != (token = strsep(&string, " "))) {
            // printf("space_idx:%d\n",space_idx);
            if (space_idx == 4)
                break;

            if (space_idx == 0) {
                pid = atoi(token);
            }
            if (space_idx == 1) {
                strcpy(pname, token);
            }
            if (space_idx == 3) {
                ppid = atoi(token);
            }

            space_idx++;
        }
        printf("pid:%d\t", pid);
        printf("token:%-30s", pname);
        printf("ppid:%d\n", ppid);

        proc[proc_count].pid = pid;
        proc[proc_count].ppid = ppid;
        strcpy(proc[proc_count].proc_name, pname);

        proc_count++;

        free(tofree);
        fclose(fp);
        free(buffer);
        free(pname);
    }

    printf("%d(%s)\n", 0, proc[0].proc_name);
    proctree_print(0, 1);
    return;
}

void proctree_print(int pid, int step){
    
    for(int i=0;i<proc_count;i++){
        if(proc[i].ppid== pid){
            for(int j=0;j<step;j++){
                printf("      ");
            }
            printf("%d(%s)\n", proc[i].pid, proc[i].proc_name);
            proctree_print(proc[i].pid, step+1);
            
        }
    }
    return;
}


int main(int argc, char *argv[]) {
    assert(argv[0]);

    for (int i = 1; i < argc; i++) {
        assert(argv[i]);

        if (!strcmp(argv[i], "-V")) {
            printf("%s\n", version);
        } else if (!strcmp(argv[i], "-p")) {
            flag = PIDCONTAIN;
            print_pstree();
        } else if (!strcmp(argv[i], "-n")) {
            flag = BYNAME;
            print_pstree();
        } else {
            printf("No such argument: %s\n", argv[i]);
        }
    }
    if (argc < 2) {
        print_pstree();

        printf("No  argument found!\n");
    }
    assert(!argv[argc]);
    printf("**** finish correctly! *****\n");
    return 0;
}