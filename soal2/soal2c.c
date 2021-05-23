#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 


int pid;
int pipe1[2];
int pipe2[2];

void comm1(){
    // output to pipe1
    dup2(pipe1[1], 1);
    // close fds
    close(pipe1[0]);
    close(pipe1[1]);
    // exec
    execlp("ps", "ps", "-aux", NULL);
    _exit(1);
}

void comm2(){
    // input from pipe1
    dup2(pipe1[0], 0);
    // output to pipe2
    dup2(pipe2[1], 1);
    // close fds
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    // exec
    execlp("sort", "sort","-nrk" , "3,3", NULL);
    _exit(1);
}

void comm3(){
    // input from pipe2
    dup2(pipe2[0], 0);
    // close fds
    close(pipe2[0]);
    close(pipe2[1]);
    // exec
    execlp("head", "head", "-5", NULL);
    _exit(1);
}

int main(){

    // create pipe1
    if (pipe(pipe1) == -1){
        perror("pipe1 failed");
        exit(1);
    }

    // fork (ps aux)
    if ((pid = fork()) == -1){
        perror("fork1 failed");
        exit(1);
    }
    else if (pid == 0){
        comm1();
    }

    // create pipe2
    if (pipe(pipe2) == -1){
        perror("pipe2 failed");
        exit(1);
    }

    // fork (sort -nrk 3,3)
    if ((pid = fork()) == -1){
        perror("fork2 failed");
        exit(1);
    }
    else if (pid == 0){
        comm2();
    }

    close(pipe1[0]);
    close(pipe1[1]);

    // fork (head 5)
    if ((pid = fork()) == -1){
        perror("fork3 failed");
        exit(1);
    }
    else if (pid == 0){
        comm3();
    }else{
	execlp("^C","^C",NULL);
    }

    return 0;
}

//ps aux | sort -nrk 3,3 | head -5
