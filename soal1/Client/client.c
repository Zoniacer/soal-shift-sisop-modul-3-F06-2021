#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#define PORT 1111
#define MAX_INFORMATION_LENGTH 200
#define EMPAT_KB 4096
#define MAX_FILE_CHUNK EMPAT_KB
#define FAIL_OR_SUCCESS_LENGTH 10
#define MAX_CREDENTIALS_LENGTH 100
#define LINE_COUNT_STR_LENGTH 20
char failMsg[] = "false";
char successMsg[] = "true";

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

void getlineRemoveNewline(char dest[]) {
    fgets(dest, MAX_INFORMATION_LENGTH, stdin);
    if(dest[strlen(dest) - 1] == '\n')
        dest[strlen(dest) - 1] = '\0';
}

void daftar(int socket) {
    char id[MAX_CREDENTIALS_LENGTH], password[45];
    printf("Masukkan id : ");
    getlineRemoveNewline(id);
    printf("Masukkan password : ");
    getlineRemoveNewline(password);
    char *credential = strcat(id, ":");
    strcat(credential, password);
    send(socket , credential, MAX_CREDENTIALS_LENGTH , 0);
    char message[50];
    int readStatus = read(socket , message, sizeof(message));
    if(readStatus > 0 && strcmp(message, "true") == 0) {
        printf("Daftar berhasil.\n");
        return;
    }
    printf("Daftar gagal.\n");
}

bool login(int socket) {
    char id[MAX_CREDENTIALS_LENGTH], password[45];
    printf("Masukkan id : ");
    getlineRemoveNewline(id);
    printf("Masukkan password : ");
    getlineRemoveNewline(password);
    char *credential = strcat(id, ":");
    strcat(credential, password);
    send(socket , credential, MAX_CREDENTIALS_LENGTH , 0);
    char message[50];
    int readStatus = read(socket , message, sizeof(message));
    if(readStatus > 0 && strcmp(message, "true") == 0) {
        printf("Login berhasil.\n");
        return true;
    }
    printf("Login gagal.\n");
    return false;
}

bool readBinaryFile(FILE * file, int chunk[], int size) {
    int i=0, charFromFile;
    // FILE * dbg = fopen("anu.txt", "a");
    for(i=0; i<size; i++) {
        // puts("masuk");
        charFromFile = fgetc(file);
        if(charFromFile == EOF) {
            chunk[i] = -1;
            break;
        }
        // fprintf(dbg, "%d ", charFromFile);
        chunk[i] = charFromFile;
    }
    if(charFromFile == EOF) return false;
    else return true;
}

bool readFileandSend(int socket, char filename[]) {
    int chunk[MAX_FILE_CHUNK];
    char message[FAIL_OR_SUCCESS_LENGTH];
    printf("Nama buku %s\n", filename);
    FILE * file = fopen(filename, "r");
    if(file == NULL) {
        printf("Tidak bisa membaca file.\n");
        return false;
    }
    while (true) {
        char isEOF[10];
        memset(isEOF, 0, sizeof(isEOF));
        memset(chunk, 0, sizeof(chunk));
        strcpy(isEOF, (readBinaryFile(file, chunk, sizeof(chunk) / sizeof(int)) ? "false" : "true"));
        send(socket, isEOF, sizeof(isEOF), 0);
        send(socket, chunk, sizeof(chunk), 0);
        if(strcmp(isEOF, "true") == 0)
            break;
    };
    memset(message, 0, FAIL_OR_SUCCESS_LENGTH);
    read(socket, message, FAIL_OR_SUCCESS_LENGTH);
    return strcmp(message, "true") == 0;
}

void writeToBinaryFile(FILE * file, int chunk[], int size) {
    int i=0;
    // FILE * dbg = fopen("anu.txt", "a");
    for(; i<size; i++) {
        if(chunk[i] == -1) break;
        // fprintf(dbg, "%d ", chunk[i]);
        fputc(chunk[i], file);
    }
}

FILE * readandSavefile(int socket, char filename[]) {
    char message[FAIL_OR_SUCCESS_LENGTH];
    memset(message, 0, FAIL_OR_SUCCESS_LENGTH);
    read(socket, message, FAIL_OR_SUCCESS_LENGTH);
    if(strcmp(message, failMsg) == 0) {
        printf("File tidak ada di dalam server.\n");
        return NULL;
    }
    char isEOF[10];
    int chunk[MAX_FILE_CHUNK];
    FILE * file = fopen(filename, "w");
    do {
    // printf("mantap\n");
        memset(isEOF, 0, sizeof(isEOF));
        read(socket, isEOF, sizeof(isEOF));
        memset(chunk, 0, sizeof(chunk));
        read(socket, chunk, sizeof(chunk));
        writeToBinaryFile(file, chunk, sizeof(chunk) / sizeof(int));
    } while (strcmp(isEOF, "true") != 0);
    if(file) printf("File %s selesai didownload.\n", filename);
    else printf("File %s gagal didownload.\n", filename);
    return file;
}

void addBuku(int socket) {
    char publisher[MAX_INFORMATION_LENGTH / 3], filepath[MAX_INFORMATION_LENGTH / 3];
    char bookInfo[MAX_INFORMATION_LENGTH];
    int tahun;
    printf("Publisher: ");
    getlineRemoveNewline(publisher);
    printf("Tahun Publikasi: ");
    scanf("%d", &tahun);
    getchar();
    printf("Filepath: ");
    getlineRemoveNewline(filepath);
    sprintf(bookInfo, "%s|%d|%s", publisher, tahun, filepath);
    send(socket , bookInfo, MAX_INFORMATION_LENGTH , 0);
    if(readFileandSend(socket, filepath)) {
        printf("Buku ditambahkan.\n");
    }
    // printf("Buku gagal ditambahkan.\n");
}

bool authentication(int sock) {
    const char loginPrompt[] = "Silahkan daftar atau login terlebih dahulu.";
    puts(loginPrompt);
    char action[10];
    printf("(login / register) : ");
    getlineRemoveNewline(action);
    if(strcmp(action, "register") == 0) {
        send(sock , action, strlen(action) , 0);
        daftar(sock);
    } else if(strcmp(action, "login") == 0) {
        send(sock , action, strlen(action) , 0);
        return login(sock);
    }
    return false;
}

void printBookInfo(char information[]) {
    char * publisher = strtok(information, "|");
    char * tahun = strtok(NULL, "|");
    char * filepath = strtok(NULL, "|");
    char copy_of_filepath[strlen(filepath) + 1];
    strcpy(copy_of_filepath, filepath);
    char * namaplusext = basename(copy_of_filepath);
    char * nama = strtok(namaplusext, ".");
    char * ext = strtok(NULL, ".");
    printf("Nama: %s\n", nama);
    printf("Publisher: %s\n", publisher);
    printf("Tahun publishing: %s\n", tahun);
    printf("Ekstensi File: %s\n", ext);
    printf("Filepath: %s\n\n", filepath);
}

void receiveFilesTsv(int socket) {
    char strLineCount[LINE_COUNT_STR_LENGTH];
    read(socket, strLineCount, LINE_COUNT_STR_LENGTH);
    int lineCount = atoi(strLineCount);
    // printf("%d\n", lineCount);
    while(lineCount--) {
        char information[MAX_INFORMATION_LENGTH];
        memset(information, 0, MAX_INFORMATION_LENGTH);
        read(socket, information, MAX_INFORMATION_LENGTH);
        printBookInfo(information);
        // puts(information);
    }
}

void deleteBook(int sock) {
    char filename[MAX_INFORMATION_LENGTH];
    getlineRemoveNewline(filename);
    send(sock, filename, sizeof(filename), 0);
    char message[FAIL_OR_SUCCESS_LENGTH];
    memset(message, 0, FAIL_OR_SUCCESS_LENGTH);
    read(sock, message, FAIL_OR_SUCCESS_LENGTH);
    if(strcmp(message, failMsg)) printf("Gagal menghapus file.\n");
    else printf("Sukses menghapus file.\n");
}

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    if(!setupClient(&sock)) {
        printf("Gagal inisiasi client\n");
        exit(EXIT_FAILURE);
    }
    
    while(!authentication(sock));

    printf("User authenticated.\n");
    while(true) {
        char action[MAX_INFORMATION_LENGTH];
        memset(action, 0, MAX_INFORMATION_LENGTH);
        printf("\nList perintah :\n");
        printf("1. add\n");
        printf("2. download x (x: nama file yang ingin didownload)\n");
        printf("3. delete x (x: nama file yang ingin dihapus)\n");
        printf("4. see\n");
        // printf("5. find x (x: nama file yang ingin dicari)\n");
        printf("6. exit\n");
        printf("Masukkan perintah : ");
        scanf("%s", action);
        getchar();
        if(strcmp("add", action) == 0) {
            send(sock, action, MAX_INFORMATION_LENGTH, 0);
            addBuku(sock);
        } else if(strcmp("see", action) == 0) {
            send(sock, action, MAX_INFORMATION_LENGTH, 0);
            receiveFilesTsv(sock);
        } else if(strcmp("download", action) == 0) {
            send(sock, action, MAX_INFORMATION_LENGTH, 0);
            char filename[MAX_INFORMATION_LENGTH];
            getlineRemoveNewline(filename);
            send(sock, filename, sizeof(filename), 0);
            FILE * buku = readandSavefile(sock, filename);
            if(buku) fclose(buku);
        } else if(strcmp("exit", action) == 0) {
            send(sock, action, sizeof(action), 0);
            exit(EXIT_SUCCESS);
        } else if(strcmp("delete", action) == 0) {
            send(sock, action, MAX_INFORMATION_LENGTH, 0);
            deleteBook(sock);
        } 
    }

    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read(sock , buffer, 1024);
    // printf("%s\n", buffer);
    // printf("%s\n",buffer );
    return 0;
}