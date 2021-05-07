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
int M1[4][3] = { {1, 2, 3},
                {3, 4, 5},
                {5, 6, 7},
                {7, 8, 9}},
    M2[3][6] = { {1, 3, 5, 7, 9, 11},
                {2, 4, 6, 8, 10},
                {3, 6, 9, 12, 15, 18}},
    HASIL[4][6];

void* hitungmatriks (void *arg) {

    pthread_t id = pthread_self();

    if (pthread_equal(id, tid[0]))      // baris 1 matriks Hasil
        for (int i = 0; i < COL; i++) 
            HASIL[0][i] = M1[0][0]*M2[0][i] + M1[0][1]*M2[1][i];
    else if (pthread_equal(id, tid[1])) // baris 2 matriks Hasil
        for (int i = 0; i < COL; i++)
            HASIL[1][i] = M1[1][0]*M2[0][i] + M1[1][1]*M2[1][i];
    else if (pthread_equal(id, tid[2])) // baris 3 matriks Hasil
        for (int i = 0; i < COL; i++)
            HASIL[2][i] = M1[2][0]*M2[0][i] + M1[2][1]*M2[1][i];
    else if (pthread_equal(id, tid[3])) // baris 4 matriks Hasil
        for (int i = 0; i < COL; i++)
            HASIL[3][i] = M1[3][0]*M2[0][i] + M1[3][1]*M2[1][i];

}

int main () {
    int i,j;
    for (i = 0; i < ROW; i++) {
        int err = pthread_create(&(tid[i]), NULL, &hitungmatriks, NULL);
        if(err!=0){
			printf("\n can't create thread : [%s]",strerror(err));
		}
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    printf("MATRIKS HASIL\n");
    int (*share_matriks)[4];

    key_t key = 1234;
    int shmid = shmget(key, sizeof(int[ROW][COL]), IPC_CREAT | 0666);
    share_matriks = shmat(shmid, NULL, 0);

    for (i = 0; i < ROW; i++){
        for (j = 0; j < COL; j++){
            share_matriks[i][j] = HASIL[i][j];
            printf("%d\t",share_matriks[i][j]);
        }
        printf("\n");   
    }

    sleep(10);

    shmdt(share_matriks);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}