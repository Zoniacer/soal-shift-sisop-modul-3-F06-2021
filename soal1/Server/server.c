#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#define PORT 1111
#define MAX_CONN 50
#define MAX_CREDENTIALS_LENGTH 100
#define MAX_INFORMATION_LENGTH 200
#define EMPAT_KB 4096
#define MAX_FILE_CHUNK EMPAT_KB

bool isFileExists(char filename[]) {
    return access(filename, F_OK) == 0;
}

bool createFile(char filename[]) {
    FILE * file = fopen(filename, "a");
    if(file) {
        fclose(file);
        return true;
    }
    return false;
}

bool setupServer(int listen_port, int * server_fd, struct sockaddr_in * address) {
    int valread;
    int opt = 1;
    bool success = true;
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    success &= setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == 0;

    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons( listen_port );
      
    success &= bind(*server_fd, (const struct sockaddr *)address, sizeof(*address)) >= 0;

    return success & (listen(*server_fd, 3) == 0);
}

bool acceptConnection(int * new_socket, int * server_fd, struct sockaddr_in * address) {
    int addrlen = sizeof(address);
    return (*new_socket = accept(*server_fd, (struct sockaddr *)address, (socklen_t*)&addrlen)) != -1;
}

bool login(int socket, char authenticatedUser[]) {
    char credentials[MAX_CREDENTIALS_LENGTH], file_credentials[MAX_CREDENTIALS_LENGTH];
    memset(credentials, 0, MAX_CREDENTIALS_LENGTH);
    int readStatus = read(socket, credentials, MAX_CREDENTIALS_LENGTH);
    FILE * fileAkun = fopen("akun.txt", "r");
    while(readStatus > 0 && fileAkun && fscanf(fileAkun, "%s", file_credentials) != EOF) {
        if(strcmp(credentials, file_credentials) == 0) {
            printf("Akun %s telah login\n", strtok(credentials, ":"));
            send(socket, "true", sizeof("true"), 0);
            strcpy(authenticatedUser, credentials);
            fclose(fileAkun);
            return true;
        }
    }
    send(socket, "false", sizeof("false"), 0);
    fclose(fileAkun);
    return false;
}

bool daftar(int socket) {
    char credentials[MAX_CREDENTIALS_LENGTH];
    memset(credentials, 0, MAX_CREDENTIALS_LENGTH);
    int readStatus = read(socket, credentials, MAX_CREDENTIALS_LENGTH);
    printf("Akun %s telah didaftarkan.\n", credentials);
    FILE * fileAkun = fopen("akun.txt", "a");
    if(fileAkun && readStatus > 0 && fprintf(fileAkun, "%s\n", credentials)) {
        send(socket, "true", sizeof("true"), 0);
        fclose(fileAkun);
        return true;
    }
    send(socket, "false", sizeof("false"), 0);
    fclose(fileAkun);
    return false;
}

bool authentication(int socket, char authenticatedUser[]) {
    char action[10];
    memset(action, 0, sizeof(action));
    int readStatus = read(socket, action, sizeof(action));
    if(strcmp(action, "register") == 0) {
        daftar(socket);
        return false;
    } else if (strcmp(action, "login") == 0) {
        return login(socket, authenticatedUser);
    }
    return false;
}

bool isFolderExists(char foldername[]) {
    DIR * dir = opendir("FILES");
    return dir != NULL;
}

FILE * readandSavefile(int socket, char filename[]) {
    char isEOF[10];
    char chunk[MAX_FILE_CHUNK + 1];
    FILE * file = fopen(filename, "w");
    do {
        memset(isEOF, 0, sizeof(isEOF));
        read(socket, isEOF, sizeof(isEOF));
        memset(chunk, 0, MAX_FILE_CHUNK + 1);
        read(socket, chunk, sizeof(chunk));
        fprintf(file, "%s", chunk);
    } while (strcmp(isEOF, "true") != 0);
    return file;
}

bool addDataBuku(int socket, char *filename) {
    char information[MAX_INFORMATION_LENGTH];
    memset(information, 0, MAX_INFORMATION_LENGTH);
    int readStatus = read(socket, information, MAX_INFORMATION_LENGTH);
    printf("Buku %s telah didaftarkan.\n", information);
    FILE * fileFile = fopen("files.tsv", "a");
    if(fileFile && readStatus > 0 && fprintf(fileFile, "%s\n", information)) {
        fclose(fileFile);
        filename = strtok(information, "|");
        return true;
    }
    fclose(fileFile);
    return false;
}

void logging(const char event[], const char filename[], char authenticatedUser[]) {
    FILE * logFile = fopen("running.log", "a");
    if(logFile) {
        fprintf(logFile, "%s : %s (%s)\n", event, filename, authenticatedUser);
        fclose(logFile);
    }
}

void addBuku(int socket, char authenticatedUser[]) {
    char *filename;
    if(!addDataBuku(socket, filename)) {
        send(socket, "false", sizeof("false"), 0);
        return false;
    }
    FILE * file = readandSavefile(socket, filename);
    if(file == NULL) {
        send(socket, "false", sizeof("false"), 0);
    }
    fclose(file);
    send(socket, "true", sizeof("true"), 0);
    logging("Tambah", filename, authenticatedUser);
}

void app(int socket) {
    char authenticatedUser[MAX_CREDENTIALS_LENGTH];
    while(!authentication(socket, authenticatedUser));
    printf("User telah terautentikasi\n");
    while(true) {
        char action[MAX_INFORMATION_LENGTH];
        memset(action, 0, MAX_INFORMATION_LENGTH);
        int readStatus = read(socket, action, MAX_INFORMATION_LENGTH);
        if(readStatus > 0 && strcmp("add", action) == 0) {
            addBuku(socket, authenticatedUser);
        }
    }
}

int main() {
    struct sockaddr_in address;
    int server_fd;
    pthread_t tid;

    if(!setupServer(PORT, &server_fd, &address)) {
        printf("Gagal membuat server\n");
        exit(EXIT_FAILURE);
    }

    if(!isFileExists("akun.txt")) createFile("akun.txt");
    if(!isFileExists("files.tsv")) createFile("files.tsv");
    if(!isFolderExists("FILES")) mkdir("FILES", S_IRWXU);

    while(true) {
        int new_socket;
        if(!acceptConnection(&new_socket, &server_fd, &address)) {
            printf("Tidak dapat menerima koneksi.\n");
            continue;
        }
        printf("Koneksi diterima.\nAkan dibuatkan thread baru.\n");
        app(new_socket);
        // pthread_create(&tid, NULL, server, (void *)&new_socket);
    }
    return 0;
}