#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#define ROW 4
#define COL 6
pthread_t tid[4];
int mx_pass[4][6],mx_batas[4][6],HASIL[4][6];

void* hitungfac (void *arg) {
    
    pthread_t id = pthread_self();

    if (pthread_equal(id, tid[0]))
        for (int i = 0; i < COL; i++) 
            HASIL[0][i];
    else if (pthread_equal(id, tid[1]))
        for (int i = 0; i < COL; i++)
            HASIL[1][i];
    else if (pthread_equal(id, tid[2]))
        for (int i = 0; i < COL; i++)
            HASIL[2][i];
    else if (pthread_equal(id, tid[3]))
        for (int i = 0; i < COL; i++)
            HASIL[3][i];
}

int main(){
    int (*share_matriks)[4],i,j;

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int[ROW][COL]), IPC_CREAT | 0666);
    share_matriks = shmat(shmid, NULL, 0);

    printf("MATRIKS PASSING\n");
    for(i=0;i<ROW;i++){
        for(j=0;j<COL;j++){
            mx_pass[i][j] = share_matriks[i][j];
            printf("%d\t",mx_pass[i][j]);
        }
        printf("\n");
    }
    printf("\nINPUT MATRIKS BATAS :\n");
    for(i=0;i<ROW;i++){
        for(j=0;j<COL;j++){
            scanf("%d",mx_batas[i][j]);
        }
    }

    for (i = 0; i < ROW; i++) {
        int err = pthread_create(&(tid[i]), NULL, &hitungfac, NULL);
        if(err!=0){
			printf("\n can't create thread : [%s]",strerror(err));
		}
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    return 0;
}