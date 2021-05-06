#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#define PORT 1111

bool setupClient(int * sock) {
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return false;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return false;
    }
  
    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return false;
    }
    return true;
}

void daftar(int socket) {
    char id[45], password[45];
    printf("Masukkan id : ");
    scanf("%s", id);
    printf("Masukkan password : ");
    scanf("%s", password);
    char *credential = strcat(id, ":");
    strcat(credential, password);
    send(socket , credential, strlen(credential) + 1 , 0);
    char message[50];
    int readStatus = read(socket , message, sizeof(message));
    if(readStatus > 0 && strcmp(message, "true") == 0) {
        printf("Daftar berhasil.\n");
        return;
    }
    printf("Daftar gagal.\n");
}

bool login(int socket) {
    char id[45], password[45];
    printf("Masukkan id : ");
    scanf("%s", id);
    printf("Masukkan password : ");
    scanf("%s", password);
    char *credential = strcat(id, ":");
    strcat(credential, password);
    send(socket , credential, strlen(credential) + 1 , 0);
    char message[50];
    int readStatus = read(socket , message, sizeof(message));
    if(readStatus > 0 && strcmp(message, "true") == 0) {
        printf("Login berhasil.\n");
        return true;
    }
    printf("Login gagal.\n");
    return false;
}

bool authentication(int sock) {
    const char loginPrompt[] = "Silahkan daftar atau login terlebih dahulu.";
    puts(loginPrompt);
    char action[10];
    printf("(login / register) : ");
    scanf("%s", action);
    if(strcmp(action, "register") == 0) {
        send(sock , action, strlen(action) , 0);
        daftar(sock);
    } else if(strcmp(action, "login") == 0) {
        send(sock , action, strlen(action) , 0);
        return login(sock);
    }
    return false;
}

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    if(!setupClient(&sock)) {
        printf("Gagal inisiasi client\n");
        exit(EXIT_FAILURE);
    }
    
    while(!authentication(sock));

    printf("User authenticated.\n");

    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read(sock , buffer, 1024);
    // printf("%s\n", buffer);
    // printf("%s\n",buffer );
    return 0;
}