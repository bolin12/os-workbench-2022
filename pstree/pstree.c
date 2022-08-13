#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>

struct ProcTree{
    size_t pid;
    struct ProcTree *parent;

};




const char *proc_dir = "/proc";
const char *version = "pstree (PSmisc) BLmimic\n\
                       Copyright (C) 1993-2019 Werner Almesberger and Craig Small\n \
                       \n\
                       PSmisc comes with ABSOLUTELY NO WARRANTY.\n\
                       This is free software, and you are welcome to redistribute it under\n\
                       the terms of the GNU General Public License.\n\
                       For more information about these matters, see the files named COPYING.";
int flag=2;
void print_pstree();

DIR * FD;


struct dirent * in_file;

int main(int argc, char *argv[]) {
    assert(argv[0]);


    // printf("%s\n", argv[0]);
    for (int i = 1; i < argc; i++) {
        assert(argv[i]);

        if(!strcmp(argv[i], "-V")){
            printf("%s\n", version);
            return 0;
        }
        else if(!strcmp(argv[i], "-p")){
            flag=0;
            print_pstree();
            return 0;
        }else if(!strcmp(argv[i], "-n")){
            flag = 1;
            print_pstree();
        }else{
            printf("No such argument: %s\n", argv[i]);
        }
    }
    assert(!argv[argc]);
    return 0;
}

void print_pstree() {
    //    if(flag==2){
    //    }
    //    else if(flag==0){
    //        printf("%s\n", version);
    //    }
    //    else if(flag==1){
    //        printf("%s\n", version);
    //    }

    if(NULL == (FD = opendir(proc_dir))){
        fprintf(stderr, "Error: Failed to open input directory - %s\n", strerror(errno));
        return ; 
    }
    FD = opendir(proc_dir);

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
        printf("%s\n", stat_path);
        /***********************************************/



        int c;
        long  lSize;
        char * buffer ;

        char * tmp_stat_path = "run.sh";
        FILE *fp = fopen(tmp_stat_path,"r");
        if(!fp){
            perror("File opening failed");
            exit(1);
        }
        fseek(fp, 0L, SEEK_END);
        lSize = ftell(fp);
        printf("lsize:%ld\n",lSize);
        fseek(fp, 0, SEEK_SET);
        //rewind(fp);

        /*allocate memory for the process stat*/
        buffer = malloc(lSize); 
        if(!buffer)fclose(fp),fputs("memory alloc fails1\n",stderr), exit(1);

        /*copy the file into the buffer*/
        //if(lSize!=fread(buffer, sizeof(char), lSize, fp))
        //fclose(fp),free(buffer),fputs("entire read fails2\n", stderr),exit(1);
        fread(buffer, 1,  lSize, fp);

        /* do some work here*/

        printf("*********************\n%s*********************\n",buffer); 
        
        for(int i=0;i<lSize;i++){
            printf("%c", buffer[i]);
        }

        while(EOF!=(c = fgetc(fp))){
            putchar(c);
        }

        fclose(fp);
        free(buffer);
        exit(1);

    } 

}

