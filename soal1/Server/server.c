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
#include <libgen.h>
#define PORT 1111
#define MAX_CONN 50
#define MAX_CREDENTIALS_LENGTH 100
#define MAX_INFORMATION_LENGTH 200
#define EMPAT_KB 4096
#define MAX_FILE_CHUNK EMPAT_KB
#define FAIL_OR_SUCCESS_LENGTH 10
#define LINE_COUNT_STR_LENGTH 20
char failMsg[] = "false";
char successMsg[] = "true";

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

    return success & (listen(*server_fd, 10) == 0);
}

bool acceptConnection(int * new_socket, int * server_fd, struct sockaddr_in * address) {
    int addrlen = sizeof(address);
    return (*new_socket = accept(*server_fd, (struct sockaddr *)address, (socklen_t*)&addrlen)) != -1;
}

bool login(int socket, char authenticatedUser[]) {
    char credentials[MAX_CREDENTIALS_LENGTH], file_credentials[MAX_CREDENTIALS_LENGTH];
    memset(credentials, 0, MAX_CREDENTIALS_LENGTH);
    int readStatus = read(socket, credentials, MAX_CREDENTIALS_LENGTH);
    if(readStatus <= 0)
        return false;
    FILE * fileAkun = fopen("akun.txt", "r");
    while(readStatus > 0 && fileAkun && fgets(file_credentials, MAX_CREDENTIALS_LENGTH, fileAkun) != NULL) {
        if(file_credentials[strlen(file_credentials) - 1] == '\n')
            file_credentials[strlen(file_credentials) - 1] = '\0';
        if(strcmp(credentials, file_credentials) == 0) {
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
    if(readStatus <= 0)
        return false;
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
    if(readStatus <= 0)
        return -1;
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

void writeToBinaryFile(FILE * file, int chunk[], int size) {
    int i=0;
    // FILE * dbg = fopen("anu.txt", "a");
    for(; i<size; i++) {
        if(chunk[i] == -1) break;
        // fprintf(dbg, "%d ", chunk[i]);
        fputc(chunk[i], file);
    }
}

FILE * readandSavefile(int socket, char filepath[]) {
    chdir("FILES");
    char isEOF[10];
    int chunk[MAX_FILE_CHUNK];
    char copy_of_filepath[strlen(filepath) + 1];
    strcpy(copy_of_filepath, filepath);
    FILE * file = fopen(basename(copy_of_filepath), "w");
    do {
        memset(isEOF, 0, sizeof(isEOF));
        if(read(socket, isEOF, sizeof(isEOF)) <= 0)
            return NULL;
        memset(chunk, 0, sizeof(chunk));
        if(read(socket, chunk, sizeof(chunk)) <= 0)
            return NULL;
        writeToBinaryFile(file, chunk, sizeof(chunk) / sizeof(int));
    } while (strcmp(isEOF, "true") != 0);
    chdir("../");
    return file;
}

void logging(const char event[], const char filepath[], char authenticatedUser[]) {
    FILE * logFile = fopen("running.log", "a");
    char copy_of_filepath[strlen(filepath) + 1];
    strcpy(copy_of_filepath, filepath);
    if(logFile) {
        fprintf(logFile, "%s : %s (%s)\n", event, basename(copy_of_filepath), authenticatedUser);
        fclose(logFile);
    }
}

bool addDataBuku(int socket, char *filepath) {
    char information[MAX_INFORMATION_LENGTH];
    memset(information, 0, MAX_INFORMATION_LENGTH);
    int readStatus = read(socket, information, MAX_INFORMATION_LENGTH);
    if(readStatus <= 0)
        return false;
    FILE * fileFile = fopen("files.tsv", "a");
    if(fileFile && readStatus > 0 && fprintf(fileFile, "%s\n", information)) {
        fclose(fileFile);
        printf("Buku %s telah didaftarkan.\n", information);
        strcpy(filepath, information);
        return true;
    }
    fclose(fileFile);
    return false;
}

void addBuku(int socket, char authenticatedUser[]) {
    char filepath[MAX_INFORMATION_LENGTH];
    if(!addDataBuku(socket, filepath)) {
        send(socket, failMsg, sizeof(failMsg), 0);
        return;
    }
    char * token = strtok(filepath, "|");
    token = strtok(NULL, "|");
    token = strtok(NULL, "|");
    FILE * file = readandSavefile(socket, token);
    if(file == NULL) {
        send(socket, failMsg, FAIL_OR_SUCCESS_LENGTH, 0);
    }
    fclose(file);
    send(socket, successMsg, FAIL_OR_SUCCESS_LENGTH, 0);
    logging("Tambah", token, authenticatedUser);
}

int getLineCount(FILE * file) {
    int cnt = 0;
    char c;
    if(file) {
        while((c = getc(file)) != EOF) if(c == '\n') cnt++;
        rewind(file);
        return cnt;
    }
    return 0;
}

void seeFilesTsv(int socket) {
    FILE * file = fopen("files.tsv", "r");
    int lineCount = getLineCount(file);
    char strLineCount[LINE_COUNT_STR_LENGTH];
    char information[MAX_INFORMATION_LENGTH];
    sprintf(strLineCount, "%d", lineCount);
    send(socket, strLineCount, LINE_COUNT_STR_LENGTH, 0);
    while(file && lineCount--) {
        memset(information, 0, MAX_INFORMATION_LENGTH);
        fgets(information, MAX_INFORMATION_LENGTH, file);
        if(information[strlen(information) - 1] == '\n')
            information[strlen(information) - 1] = '\0';
        send(socket, information, MAX_INFORMATION_LENGTH, 0);
    }
    fclose(file);
}

void findSpecificName(int socket, char subStrName[]) {
    FILE * file = fopen("files.tsv", "r");
    int lineCount = getLineCount(file);
    char strLineCount[LINE_COUNT_STR_LENGTH];
    char information[MAX_INFORMATION_LENGTH];
    sprintf(strLineCount, "%d", lineCount);
    send(socket, strLineCount, LINE_COUNT_STR_LENGTH, 0);
    while(file && lineCount--) {
        memset(information, 0, MAX_INFORMATION_LENGTH);
        fgets(information, MAX_INFORMATION_LENGTH, file);
        if(information[strlen(information) - 1] == '\n')
            information[strlen(information) - 1] = '\0';
        char copy_of_information[strlen(information) + 1];
        strcpy(copy_of_information, information);
        char * publisher = strtok(information, "|");
        char * tahun = strtok(NULL, "|");
        char * filepath = strtok(NULL, "|");
        char copy_of_filepath[strlen(filepath) + 1];
        strcpy(copy_of_filepath, filepath);
        char * filenameInTsv = basename(copy_of_filepath);
        if(strstr(filenameInTsv, subStrName) == NULL) continue;
        send(socket, copy_of_information, MAX_INFORMATION_LENGTH, 0);
    }
    send(socket, failMsg, FAIL_OR_SUCCESS_LENGTH, 0);
    fclose(file);
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

bool isBookExistInTsv(char filename[]) {
    FILE * file = fopen("files.tsv", "r");
    int lineCount = getLineCount(file);
    while(lineCount--) {
        char information[MAX_INFORMATION_LENGTH];
        memset(information, 0, sizeof information);
        if(fgets(information, sizeof information, file) == NULL)
            continue;
        if(information[strlen(information) - 1] == '\n')
            information[strlen(information) - 1] = '\0';
        char * publisher = strtok(information, "|");
        char * tahun = strtok(NULL, "|");
        char * filepath = strtok(NULL, "|");
        char copy_of_filepath[strlen(filepath) + 1];
        strcpy(copy_of_filepath, filepath);
        char * filenameInTsv = basename(copy_of_filepath);
        if(strcmp(filenameInTsv, filename) == 0) return true;
    }
    return false;
}

bool readFileandSend(int socket, char filename[]) {
    int chunk[MAX_FILE_CHUNK];
    char message[FAIL_OR_SUCCESS_LENGTH];
    // printf("Nama buku %s\n", filename);
    if(!isBookExistInTsv(filename)) {
        send(socket, failMsg, FAIL_OR_SUCCESS_LENGTH, 0);
        return false;
    }
    chdir("FILES");
    FILE * file = fopen(filename, "r");
    if(file == NULL) {
        printf("Tidak bisa membaca file.\n");
        chdir("../");
        return false;
    }
    send(socket, successMsg, FAIL_OR_SUCCESS_LENGTH, 0);
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
    chdir("../");
    return strcmp(message, "true") == 0;
}

void deleteFromTsv(char filename[]) {
    FILE * tsv = fopen("files.tsv", "r");
    FILE * tsv2 = fopen("files2.tsv", "w");
    if(tsv == NULL) return;
    if(tsv2 == NULL) return;
    int lineCount = getLineCount(tsv);
    while(lineCount--) {
        char information[MAX_INFORMATION_LENGTH];
        memset(information, 0, sizeof information);
        fgets(information, MAX_INFORMATION_LENGTH, tsv);
        if(information[strlen(information) - 1] == '\n')
            information[strlen(information) - 1] = '\0';
        char copy_of_information[strlen(information) + 1];
        strcpy(copy_of_information, information);
        char * publisher = strtok(information, "|");
        char * tahun = strtok(NULL, "|");
        char * filepath = strtok(NULL, "|");
        char copy_of_filepath[strlen(filepath) + 1];
        strcpy(copy_of_filepath, filepath);
        char * filenameInTsv = basename(copy_of_filepath);
        if(strcmp(filenameInTsv, filename) == 0) continue;
        fprintf(tsv2, "%s\n", copy_of_information);
    }
    fclose(tsv); fclose(tsv2);
    remove("files.tsv"); rename("files2.tsv", "files.tsv");
}

void deleteBook(int socket, char filename[], char authenticatedUser[]) {
    if(!isBookExistInTsv(filename)) {
        send(socket, failMsg, FAIL_OR_SUCCESS_LENGTH, 0);
        return;
    }
    chdir("FILES");
    char newfilename[MAX_INFORMATION_LENGTH];
    sprintf(newfilename, "old-%s", filename);
    send(socket, (rename(filename, newfilename) != 0 ? successMsg : failMsg), FAIL_OR_SUCCESS_LENGTH, 0);
    chdir("../");
    deleteFromTsv(filename);
    logging("Hapus", filename, authenticatedUser);
    send(socket, successMsg, FAIL_OR_SUCCESS_LENGTH, 0);
}

void * app(void * vargp) {
    int socket = *((int *)vargp);
    char authenticatedUser[MAX_CREDENTIALS_LENGTH];
    int authStatus = false;
    while(!authStatus) {
        authStatus = authentication(socket, authenticatedUser);
        if(authStatus == -1) { // Connection closed
            return NULL;
        }
    }
    // printf("User telah terautentikasi\n");
    while(true) {
        char action[MAX_INFORMATION_LENGTH];
        memset(action, 0, MAX_INFORMATION_LENGTH);
        int readStatus = read(socket, action, MAX_INFORMATION_LENGTH);
        if(readStatus <= 0)
            return NULL;
        if(readStatus > 0 && strcmp("add", action) == 0) {
            addBuku(socket, authenticatedUser);
        } else if(readStatus > 0 && strcmp("see", action) == 0) {
            seeFilesTsv(socket);
        } else if(readStatus > 0 && strcmp("find", action) == 0) {
            char subfilename[MAX_INFORMATION_LENGTH];
            memset(subfilename, 0, sizeof(subfilename));
            if(read(socket, subfilename, MAX_INFORMATION_LENGTH) <= 0)
                return NULL;
            findSpecificName(socket, subfilename);
        } else if(readStatus > 0 && strcmp("download", action) == 0) {
            char filename[MAX_INFORMATION_LENGTH];
            memset(filename, 0, sizeof(filename));
            if(read(socket, filename, MAX_INFORMATION_LENGTH) <= 0)
                return NULL;
            readFileandSend(socket, filename);
        } else if(readStatus > 0 && strcmp("delete", action) == 0) {
            char filename[MAX_INFORMATION_LENGTH];
            memset(filename, 0, sizeof(filename));
            if(read(socket, filename, MAX_INFORMATION_LENGTH) <= 0)
                return NULL;
            deleteBook(socket, filename, authenticatedUser);
        }
    }
    return NULL;
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
        printf("Koneksi baru diterima.\n");
        send(new_socket, successMsg, FAIL_OR_SUCCESS_LENGTH, 0);
        // app(new_socket);
        pthread_create(&tid, NULL, app, (void *)&new_socket);
        pthread_join(tid, NULL);
        close(new_socket);
        printf("Koneksi selesai.\n");
    }
    return 0;
}