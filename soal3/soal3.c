#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>
#include<errno.h>
#include<ctype.h>

pthread_mutex_t signal;
int counter_files=1;
char catepath[100][100];

char *checkName(char *dir){
    char *name = strrchr(dir, '/');
    if(name == dir)
        return "";
    return name + 1;
}

char *lowercase(char *str){
    unsigned char *temp = (unsigned char *)str;

    while(*temp){
        *temp = tolower(*temp);
        temp++;
    }
    return str;
}

char *checkExt(char *dir){
    char *hid = {"Hidden"};
    char *extdot = strrchr(dir,'/');
    char dot[2]=".";
    if(extdot[1]==dot[0]){
    	return hid;
    }
    char *unk = {"Unknown"};
    char *ext = strchr(dir, '.');
    if(ext != NULL)
        return lowercase(ext + 1);
    return unk;
}


void* categorize(void *arg){
    char *src = (char *)arg;
    if(access(src, F_OK)!=0){
//    	printf("File %d : Sad, gagal :(\n",counter_files);
	pthread_mutex_lock(&signal);
    	pthread_mutex_unlock(&signal);
    	return (void *) 0;
    }
    else{
    char srcP[150];
    memcpy(srcP, (char*)arg, 150);
    char *srcExt = checkExt(src);
    
    DIR *dir = opendir(srcExt);
    if(dir == NULL){
        mkdir(srcExt, S_IRWXU);
    }

    char *srcName = checkName(srcP);
    char *curr = getenv("PWD");
    
    char *destP = malloc(strlen(srcExt) + 1 + strlen(srcName) + 1 + strlen(curr) + 1);
    strcpy(destP, curr);
    strcat(destP, "/");
    strcat(destP, srcExt);
    strcat(destP, "/");
    strcat(destP, srcName);
//    printf("File %d : Berhasil Dikategorikan\n",counter_files);
    pthread_mutex_lock(&signal);
    if(rename(srcP, destP) != 0){
        fprintf(stderr,"error: %s\n\n",strerror(errno));
    }
    pthread_mutex_unlock(&signal);
    return (void *) 1;
    }
    counter_files++;
    pthread_mutex_lock(&signal);
    pthread_mutex_unlock(&signal);
    
}

int countlistrec=0;
void listFilesRecursively(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    int count = 0;
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
	    if(dp->d_type == DT_REG)
	    {
	    	strcpy(catepath[countlistrec],path);
	    	countlistrec++;
	    }
            listFilesRecursively(path);
        }
    }

    closedir(dir);
}


int main(int argc, char *argv[]){
    if(argc == 0){
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "-f") == 0){
	void *flag=0;
	int x;
        pthread_t tid[argc-2];
	int count=0;
        for(int i=2; i<argc; i++){
            pthread_create(&tid[count], NULL, categorize, (void *)argv[i]);
            count++;
        }

        count=0;
        for(count=0; count<argc-2; count++){
            
            pthread_join(tid[count], &flag);
            x = (int)flag;
            if(x)printf("File %d : Berhasil Dikategorikan\n", count+1);
            else if(!x)printf("File %d : Sad, gagal :(\n",count+1);
        }

        return 0;

    }else if(strcmp(argv[1], "-d") == 0){
	void *flag=0;
	int x;
	listFilesRecursively(argv[2]);
	
	pthread_t tid[countlistrec];
	int count=0;
        for(int i=0; i<countlistrec; i++){
            pthread_create(&tid[count], NULL, categorize, (void *)catepath[i]);
            count++;
        }

        count=0;
        for(count=0; count<countlistrec; count++){
            
            pthread_join(tid[count], &flag);
            x = (int)flag;
        }
	if(x)printf("Direktori suskses disimpan!\n");
        else if(!x)printf("Yah, gagal disimpan :(\n");
        return 0;

    }else if (strcmp(argv[1], "*") == 0){

        char *curr = getenv("PWD");
        listFilesRecursively(curr);
        
        pthread_t tid[countlistrec];
	int count=0;
        for(int i=0; i<countlistrec; i++){
            pthread_create(&tid[count], NULL, categorize, (void *)catepath[i]);
            count++;
        }

        count=0;
        for(count=0; count<countlistrec; count++){
            int flag;
            pthread_join(tid[count], NULL);
            
        }

        return 0;       
    }
}


