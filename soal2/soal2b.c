#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#define ull unsigned long long
#define ROW 4
#define COL 6
pthread_t tid[25];
ull mx_pass[4][6],mx_batas[4][6],HASIL[4][6];

ull fact(ull n){

    if(n==1 || n==0) return 1;
    
    ull temp = n;
    for(int i=n-1;i>=1;i--){
        temp*=i;
    }
    return temp;
}

ull mtx_fact(ull n,ull batas){
    if(n == 0 || batas == 0) return 0;
    if(n<batas) return fact(n);
    if(n>=batas) return fact(n)/fact(n-batas);
}

void* mtx_thread (void *arg) {
    int status;
    pthread_t id = pthread_self();
    for(int i = 0; i<ROW; i++){
        status=0;
        for (int j = 0; j < COL; j++){
            if (pthread_equal(id, tid[i*COL+j])){
                HASIL[i][j] = mtx_fact(mx_pass[i][j],mx_batas[i][j]);
                status=1;
                break;
            }   
        }
        if(status==1)break;
    }
}

int main(){
    int *share_matriks,i,j;

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int[ROW][COL]), IPC_CREAT | 0666);
    share_matriks = (int*)shmat(shmid, NULL, 0);

    printf("MATRIKS PASSING\n");
    for(i=0;i<ROW;i++){
        for(j=0;j<COL;j++){
            mx_pass[i][j] = share_matriks[i*COL+j];
            printf("%llu\t",mx_pass[i][j]);
        }
        printf("\n");
    }
    printf("\nINPUT MATRIKS BATAS :\n");
    for(i=0;i<ROW;i++){
        for(j=0;j<COL;j++){
            scanf("%llu",&mx_batas[i][j]);
        }
    }

    for (i = 0; i < ROW; i++) {
        for(j = 0; j < COL; j++){
            int err = pthread_create(&(tid[i*COL+j]), NULL, &mtx_thread, NULL);
            if(err!=0){
                printf("\n can't create thread : [%s]",strerror(err));
            }
        }
    }
    for(i=0;i<ROW*COL-1;i++){
        pthread_join(tid[i], NULL);
    }
    printf("\nMATRIKS HASIL FACTORIAL :\n");
    for(i=0;i<ROW;i++){
        for(j=0;j<COL;j++){
            printf("%llu\t",HASIL[i][j]);
        }
        printf("\n");
    }
    return 0;
}