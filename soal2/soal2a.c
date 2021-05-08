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

pthread_t tid[4]; // Deklarasi jumlah thread

//Deklarasi Matriks
int M1[4][3],M2[3][6],HASIL[4][6];

void* hitungmatriks (void *arg) {

    pthread_t id = pthread_self();

    for(int i = 0; i<ROW; i++){
        if (pthread_equal(id, tid[i])){
            for (int j = 0; j < COL; j++) 
                HASIL[i][j] = M1[i][0]*M2[0][j] + M1[i][1]*M2[1][j] + M1[i][2]*M2[2][j];
            break;
        }       
    }
}

int main () {
    int i,j;
    printf("\nInput Matriks pertama (4X3) :\n");
    for(i=0;i<4;i++){
        for(j=0;j<3;j++){
            scanf("%d",&M1[i][j]);
        }
    }
    printf("\nInput Matriks kedua (3X6) :\n");
    for(i=0;i<3;i++){
        for(j=0;j<6;j++){
            scanf("%d",&M2[i][j]);
        }
    }
    for (i = 0; i < ROW; i++) {
        int err = pthread_create(&(tid[i]), NULL, &hitungmatriks, NULL);
        if(err!=0){
			printf("\n can't create thread : [%s]",strerror(err));
		}
    }

    for(i=0;i<ROW-1;i++){
        pthread_join(tid[0], NULL);
    }

    printf("\nMATRIKS HASIL\n");
    int *share_matriks;

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int[ROW][COL]), IPC_CREAT | 0666);
    share_matriks = (int*)shmat(shmid, NULL, 0);

    for (i = 0; i < ROW; i++){
        for (j = 0; j < COL; j++){
            share_matriks[i*COL+j] = HASIL[i][j];
            printf("%d\t",share_matriks[i*COL+j]);
        }
        printf("\n");   
    }

    sleep(10);

    shmdt(share_matriks);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

/*
Test Case :
Matriks 1 :
2 4 1
7 0 3
1 4 8
9 4 1

Matriks 2:
1 4 8 4 6 3
9 4 1 1 4 1
8 3 6 2 1 7
*/