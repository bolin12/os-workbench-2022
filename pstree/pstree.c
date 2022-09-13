#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    size_t ppid, pid;
    char *pname;

} PData;

/* hashmap related */
typedef struct {
    size_t key;
    PData pdata_val;
} DataType;

typedef struct _HashNode {
    DataType data;
    struct _HashNode *next;
} HashNode;

typedef struct {
    int size;
    HashNode *table;
} HashMap;

/* hashmap end */
typedef struct ProcTree {
    int child_num;
    PData pdata;
    struct ProcTree *parent;
    struct ProcTree **child;

} PTree;

typedef struct _PTArr {
    PTree *ptarr_data[512];

} PTArr;

enum {
    VERSION, BYNAME, PIDCONTAIN
};

HashMap *CreateHashMap(PTArr ptarr, size_t size);

int Get(HashMap *hashmap, PData pdata);

void DestoryHashMap(HashMap *hashmap);

void PrintHashMap(HashMap *hashMap);


const char *proc_dir = "/proc";
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

void print_pstree() {
    if (flag == VERSION) {

        printf("%s\n", version);
        return;
    }

    if (NULL == (FD = opendir(proc_dir))) {
        fprintf(stderr, "Error: Failed to open input directory - %s\n",
                strerror(errno));
        return;
    }
    FD = opendir(proc_dir);

    /* for pnode data store*/
    int pdata_idx = 0;
    PTArr ptarr;
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
        strcat(stat_path, proc_dir);
        strcat(stat_path, "/");
        strcat(stat_path, in_file->d_name);
        strcat(stat_path, "/stat");
        // printf("%s\n", stat_path);
        /***********************************************/

        int c;
        long lSize;
        char *buffer = malloc(512 * sizeof(char));

        FILE *fp = fopen(stat_path, "r");
        if (!fp) {
            perror("File opening failed");

            // exit(1);
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

        // printf("total string:\n%s\n",string);

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
        printf("pid:%zu\t", pid);
        printf("token:%-30s", pname);
        printf("ppid:%zu\n", ppid);

        PData tmp_pdata;
        tmp_pdata.pid = pid;
        tmp_pdata.ppid = ppid;
        tmp_pdata.pname = malloc(strlen(pname));
        strcpy(tmp_pdata.pname, pname);

        ptarr.ptarr_data[pdata_idx] = (PTree *) malloc(sizeof(PTree));
        ptarr.ptarr_data[pdata_idx]->child_num = 0;
        ptarr.ptarr_data[pdata_idx]->child = NULL;
        ptarr.ptarr_data[pdata_idx]->parent = NULL;
        ptarr.ptarr_data[pdata_idx]->pdata = tmp_pdata;
        // printf("pdata_name: %s\n", pdata_store[pdata_idx].pname);
        pdata_idx++;

        free(tofree);
        fclose(fp);
        free(buffer);
        free(pname);
        //        printf("***   this loop been on %d times! *** \n", pdata_idx);
    }

    /* build hashmap realted */

    HashMap *hashmap = CreateHashMap(ptarr, pdata_idx);
    /* build hashmap OK */
    printf("HashMap created OK!\n");

    PTree *root = ptarr.ptarr_data[0];
    PTree *cur_node;
    PTree *cur_parent_node;
    PrintHashMap(hashmap);
    for (int i = 1; i < pdata_idx; i++) {
        cur_node = ptarr.ptarr_data[i];
        int cur_pid = cur_node->pdata.pid;
        int cur_parent_pid = cur_node->pdata.ppid;
        int parent_key = Get(hashmap, cur_node->pdata);
        if (parent_key == -1) {
            fprintf(stderr, "No such process ID!\n");
            return;
        }
        cur_parent_node = ptarr.ptarr_data[parent_key];

        cur_node->parent = cur_parent_node;

        cur_node->child_num++;
        realloc(cur_node->child, sizeof(PTree *) * cur_node->child_num);
        cur_node->

    }

    DestoryHashMap(hashmap);
    return;
}

HashMap *CreateHashMap(PTArr ptarr, size_t size) {

    HashMap *hashmap = (HashMap *) malloc(sizeof(HashMap));
    hashmap->size = 2 * size;
    hashmap->table = (HashNode *) malloc(sizeof(HashNode) * hashmap->size);

    int j = 0;
    for (j = 0; j < hashmap->size; j++) {
        hashmap->table[j].data.pdata_val.pid = INT_MIN;
        hashmap->table[j].next = NULL;
    }
    int i = 0;
    printf("size:%zu\n", size);
    while (i < size) {
        int cur_pid = ptarr.ptarr_data[i]->pdata.pid;
        PData cur_pdata = ptarr.ptarr_data[i]->pdata;
        int pos = abs(cur_pid) % hashmap->size;

        // judge if not conflict
        if (hashmap->table[pos].data.pdata_val.pid == INT_MIN) {
            hashmap->table[pos].data.key = i;
            hashmap->table[pos].data.pdata_val = cur_pdata;
        } else {
            HashNode *lnode = (HashNode *) malloc(sizeof(HashNode)), *hashnode;
            lnode->data.key = i;
            lnode->data.pdata_val = cur_pdata;
            lnode->next = NULL;
            hashnode = &(hashmap->table[pos]);
            while (hashnode->next != NULL) {
                hashnode = hashnode->next;
            }
            hashnode->next = lnode;
        }
        i++;
    }
    return hashmap;
}

int Get(HashMap *hashmap, PData pdata) {
    int pos = abs(pdata.pid) % hashmap->size;
    HashNode *pointer = &(hashmap->table[pos]);
    while (pointer != NULL) {
        if (pointer->data.pdata_val.pid == pdata.pid) {
            return pointer->data.key;
        } else {
            pointer = pointer->next;
        }
    }
    return -1;

}


void DestoryHashMap(HashMap *hashmap) {

    int i = 0;
    HashNode *hpointer;
    while (i < hashmap->size) {
        hpointer = hashmap->table[i].next;
        while (hpointer != NULL) {
            hashmap->table[i].next = hpointer->next;

            free(hpointer);
            hpointer = hashmap->table[i].next;
        }
        i++;
    }
    free(hashmap->table);
    free(hashmap);
    printf("Destory hashmap Success!\n");
}

void PrintHashMap(HashMap *hashmap) {
    printf("===========PrintHashMap==========\n");
    int i = 0;
    HashNode *pointer;
    while (i < hashmap->size) {
        pointer = &(hashmap->table[i]);
        while (pointer != NULL) {
            if (pointer->data.pdata_val.pid != INT_MIN)
                printf("%zu", pointer->data.pdata_val.pid);
            else
                printf("        [ ]");
            pointer = pointer->next;
        }
        printf("\n---------------------------------");
        i++;
        printf("\n");
    }
    printf("===============End===============\n");
}