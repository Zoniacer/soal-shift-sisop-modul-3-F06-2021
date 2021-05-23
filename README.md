# soal-shift-sisop-modul-3-F06-2021

## Soal Shift Modul 3

### **Soal No. 1**

Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna
aplikasi ini.

Di dalam proyek itu, Keverk diminta:

a. Pada saat client tersambung dengan server, terdapat dua pilihan pertama, yaitu register dan login. Jika memilih register, client akan diminta input id dan passwordnya untuk dikirimkan ke server. User juga dapat melakukan login. Login berhasil jika id dan password yang dikirim dari aplikasi client sesuai dengan list
akun yang ada didalam aplikasi server. Sistem ini juga dapat menerima multi-connections. Koneksi terhitung ketika aplikasi client tersambung dengan server. Jika terdapat 2 koneksi atau lebih maka harus menunggu sampai client pertama keluar untuk bisa melakukan login dan mengakses aplikasinya. Keverk menginginkan lokasi penyimpanan id dan password pada file bernama akun.txt dengan format :

#### akun.txt

```example
id:password
id2:password2
```

b. Sistem memiliki sebuah database yang bernama files.tsv. Isi dari files.tsv ini adalah path file saat berada di server, publisher, dan tahun publikasi. Setiap penambahan dan penghapusan file pada folder file yang bernama FILES pada server akan memengaruhi isi dari files.tsv. Folder FILES otomatis dibuat saat server dijalankan.

c. Tidak hanya itu, Keverk juga diminta membuat fitur agar client dapat menambah file baru ke dalam server. Direktori FILES memiliki struktur direktori di bawah ini :

Direktori **FILES**

```example
File1.ekstensi
File2.ekstensi
```

Pertama client mengirimkan input ke server dengan struktur sebagai berikut :
Contoh Command Client :

```example
add
```

Output Client Console:

```example
Publisher:
Tahun Publikasi:
Filepath:
```

Kemudian, dari aplikasi client akan dimasukan data buku tersebut (perlu diingat bahwa Filepath ini merupakan path file yang akan dikirim ke server). Lalu client nanti akan melakukan pengiriman file ke aplikasi server dengan menggunakan socket. Ketika file diterima di server, maka row dari files.tsv akan bertambah sesuai dengan data terbaru yang ditambahkan.

d. Dan client dapat mendownload file yang telah ada dalam folder FILES di server, sehingga sistem harus dapat mengirim file ke client. Server harus melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di folder client tersebut.

```example
download TEMPfile.pdf
```

e. Setelah itu, client juga dapat menghapus file yang tersimpan di server. Akan tetapi, Keverk takut file yang dibuang adalah file yang penting, maka file hanya akan diganti namanya menjadi ‘old-NamaFile.ekstensi’. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus.

Contoh Command Client:

```example
delete TEMPfile.pdf
```

f. Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang
bernama see. Output dari perintah tersebut keluar dengan format.

Contoh Command Client:

```example
see
```

Contoh Format Output pada Client:

```example
Nama:
Publisher:
Tahun publishing:
Ekstensi File :
Filepath :

Nama:
Publisher:
Tahun publishing:
Ekstensi File :
Filepath :
```

g. Aplikasi client juga dapat melakukan pencarian dengan memberikan suatu string. Hasilnya adalah semua nama file yang mengandung string tersebut. Format output seperti format output f.

Contoh Client Command:

```example
find TEMP
```

h. Dikarenakan Keverk waspada dengan pertambahan dan penghapusan file di server, maka Keverk membuat suatu log untuk server yang bernama running.log. Contoh isi dari log ini adalah

#### running.log

```example
Tambah : File1.ektensi (id:pass)
Hapus : File2.ektensi (id:pass)
```

Akan tetapi, kevek saat itu masih belum paham dengan sistem server dan client. Bantulah Keverk untuk menyelesaikan proyek ini!

Note:

- Dilarang menggunakan system() dan execv(). Silahkan dikerjakan sepenuhnya dengan thread dan socket programming.
- Untuk download dan upload silahkan menggunakan file teks dengan ekstensi dan isi bebas (yang ada isinya bukan touch saja dan tidak kosong) dan requirement untuk benar adalah percobaan dengan minimum 5 data.

Struktur Direktori:

![Struktur direktori no. 1](https://media.discordapp.net/attachments/798177440425181256/845864050990907402/unknown.png)

### Source code lengkap no. 1

#### Server

Pada fungsi main, dipanggil fungsi `setupServer()` untuk melakukan binding port dan listen ke port. Lalu dilakukan pemeriksaan pada file akun.txt, files.tsv, dan folder files. Apabila salah satunya tidak ada, maka akan langsung dibuat. Setelah itu dilakukan looping yang infinite dengan tujuan agar server always on. Di setiap loop, dilakukan penerimaan koneksi dan dilakukan pemanggilan fungsi `app()` pada thread baru. Ketika koneksi tadi selesai / terputus, baru akan dilakukan join thread dan menerima koneksi lain pada antrian. Saat mulai, server akan melakukan autentikasi user terlebih dahulu, apabila sudah, fitur-fitur baru dapat diakses.

```c
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
    for(; i<size; i++) {
        if(chunk[i] == -1) break;
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
    for(i=0; i<size; i++) {
        charFromFile = fgetc(file);
        if(charFromFile == EOF) {
            chunk[i] = -1;
            break;
        }
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
        pthread_create(&tid, NULL, app, (void *)&new_socket);
        pthread_join(tid, NULL);
        close(new_socket);
        printf("Koneksi selesai.\n");
    }
    return 0;
}
```

#### Client

Pertama, pada fungsi main dilakukan pemanggilan fungsi `setupClient()` untuk menginisiasi socket dan menyambungkan ke server. Setelah itu, socket akan menunggu hingga koneksi diterima oleh server. Setelah diterima, maka aplikasi akan dijalankan. Aplikasi akan melakukan autentikasi user terlebih dahulu, apabila sudah, fitur-fitur baru dapat diakses.

```c
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
#define invalidCmd "\nMaaf, command yang anda masukkan tidak valid\n"

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
    for(i=0; i<size; i++) {
        charFromFile = fgetc(file);
        if(charFromFile == EOF) {
            chunk[i] = -1;
            break;
        }
        chunk[i] = charFromFile;
    }
    if(charFromFile == EOF) return false;
    else return true;
}

bool readFileandSend(int socket, char filename[]) {
    int chunk[MAX_FILE_CHUNK];
    char message[FAIL_OR_SUCCESS_LENGTH];
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
    for(; i<size; i++) {
        if(chunk[i] == -1) break;
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
        return;
    }
    printf("Buku gagal ditambahkan.\n");
}

bool authentication(int sock) {
    const char loginPrompt[] = "Silahkan daftar atau login terlebih dahulu.";
    puts(loginPrompt);
    char action[10];
    printf("(login / register / exit) : ");
    getlineRemoveNewline(action);
    if(strcmp(action, "register") == 0) {
        send(sock , action, strlen(action) , 0);
        daftar(sock);
    } else if(strcmp(action, "login") == 0) {
        send(sock , action, strlen(action) , 0);
        return login(sock);
    } else if(strcmp(action, "exit") == 0) {
        close(sock);
        exit(EXIT_SUCCESS);
    } else puts(invalidCmd);
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
    while(lineCount--) {
        char information[MAX_INFORMATION_LENGTH];
        memset(information, 0, MAX_INFORMATION_LENGTH);
        read(socket, information, MAX_INFORMATION_LENGTH);
        printBookInfo(information);
    }
}

void findSpecificName(int socket) {
    char filename[MAX_INFORMATION_LENGTH];
    getlineRemoveNewline(filename);
    send(socket, filename, sizeof(filename), 0);
    char strLineCount[LINE_COUNT_STR_LENGTH];
    read(socket, strLineCount, LINE_COUNT_STR_LENGTH);
    int lineCount = atoi(strLineCount);
    while(lineCount--) {
        char information[MAX_INFORMATION_LENGTH];
        memset(information, 0, MAX_INFORMATION_LENGTH);
        read(socket, information, MAX_INFORMATION_LENGTH);
        if(strcmp(information, failMsg) == 0) break;
        printBookInfo(information);
    }
}

void deleteBook(int sock) {
    char filename[MAX_INFORMATION_LENGTH];
    getlineRemoveNewline(filename);
    send(sock, filename, sizeof(filename), 0);
    char message[FAIL_OR_SUCCESS_LENGTH];
    memset(message, 0, FAIL_OR_SUCCESS_LENGTH);
    read(sock, message, FAIL_OR_SUCCESS_LENGTH);
    if(strcmp(message, failMsg) == 0) printf("Gagal menghapus file.\n");
    else printf("Sukses menghapus file.\n");
}

void printPrompt() {
    printf("\nList perintah :\n");
    printf("1. add\n");
    printf("2. download x (x: nama file yang ingin didownload)\n");
    printf("3. delete x (x: nama file yang ingin dihapus)\n");
    printf("4. see\n");
    printf("5. find x (x: nama file yang ingin dicari)\n");
    printf("6. exit\n");
    printf("Masukkan perintah : ");
}

void executePrompt(int sock, char action[]) {
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
        close(sock);
        exit(EXIT_SUCCESS);
    } else if(strcmp("delete", action) == 0) {
        send(sock, action, MAX_INFORMATION_LENGTH, 0);
        deleteBook(sock);
    } else if(strcmp("find", action) == 0) {
        send(sock, action, MAX_INFORMATION_LENGTH, 0);
        findSpecificName(sock);
    } else puts(invalidCmd);
}

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    if(!setupClient(&sock)) {
        printf("Gagal inisiasi client\n");
        exit(EXIT_FAILURE);
    }
    
    puts("Menunggu hingga koneksi diterima.");
    char acceptedConnection[FAIL_OR_SUCCESS_LENGTH];
    memset(acceptedConnection, 0, FAIL_OR_SUCCESS_LENGTH);
    read(sock, acceptedConnection, FAIL_OR_SUCCESS_LENGTH);
    if(strcmp(acceptedConnection, successMsg) == 0) {
        puts("Koneksi diterima.");
    } else {
        puts("Koneksi gagal.");
        exit(EXIT_FAILURE);
    }

    while(!authentication(sock));

    printf("User authenticated.\n");
    while(true) {
        char action[MAX_INFORMATION_LENGTH];
        memset(action, 0, MAX_INFORMATION_LENGTH);
        printPrompt();
        scanf("%s", action);
        getchar();
        executePrompt(sock, action);
    }
    return 0;
}
```

### **Jawaban Soal 1a**

#### 1a pada server.c

Pada fungsi `app()` dilakukan looping hingga autentikasi selesai, namun apabila koneksi terputus di tengah jalan, maka langsung end aplikasi.

```c
while(!authStatus) {
    authStatus = authentication(socket, authenticatedUser);
    if(authStatus == -1) { // Connection closed
        return NULL;
    }
}
```

Fungsi `authentication()` akan handle autentikasi dengan menyediakan dua pilihan, login / register. Apabila user melakukan login dan sukses, akan direturn true, sebaliknya selalu false kecuali kalau koneksi terputus akan return -1.

```c
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
```

Fungsi `daftar()` akan menerima request dari client berupa id dan password lalu menyimpannya ke `akun.txt`

```c
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
```

Fungsi `login()` akan menerima request dari client berupa id dan password lalu membandingkan dengan record pada `akun.txt`

```c
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
```

#### 1a pada client.c

Secara garis besar, strukturnya mirip seperti pada `server.c`, namun disini yang dilakukan adalah handle input dari user dan mengirimnya ke server.

##### Autentikasi pada main

```c
while(!authentication(sock));
```

##### Fungsi autentikasi

Mengambil input dari user dan handle permintaan user.

```c
bool authentication(int sock) {
    const char loginPrompt[] = "Silahkan daftar atau login terlebih dahulu.";
    puts(loginPrompt);
    char action[10];
    printf("(login / register / exit) : ");
    getlineRemoveNewline(action);
    if(strcmp(action, "register") == 0) {
        send(sock , action, strlen(action) , 0);
        daftar(sock);
    } else if(strcmp(action, "login") == 0) {
        send(sock , action, strlen(action) , 0);
        return login(sock);
    } else if(strcmp(action, "exit") == 0) {
        close(sock);
        exit(EXIT_SUCCESS);
    } else puts(invalidCmd);
    return false;
}
```

##### Fungsi login

Mengambil input dari user dan mengirim ke server setelah dilakukan pemformatan `id:password`.

```c
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
```

##### Fungsi daftar

Mengambil input dari user dan mengirim ke server setelah dilakukan pemformatan `id:password`

```c
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
```

### **Jawaban Soal 1b**

#### 1b pada server.c

Folder akan langsung dibuat ketika server dijalankan. Berikut potongan kodenya pada fungsi `main()`

```c
if(!isFileExists("akun.txt")) createFile("akun.txt");
if(!isFileExists("files.tsv")) createFile("files.tsv");
if(!isFolderExists("FILES")) mkdir("FILES", S_IRWXU);
```

### **Jawaban Soal 1c**

#### 1c pada server.c

Pertama, apabila request dari client berupa command `add`, maka akan dilakukan pemanggilan fungsi `addBuku()`.

```c
if(readStatus > 0 && strcmp("add", action) == 0) {
    addBuku(socket, authenticatedUser);
}
```

Berikutnya, pada fungsi `addBuku()` dilakukan pemanggilan fungsi `addDataBuku()` yang akan dijelaskan di bawah. Kemudian diambil path dengan `strtok()` karena client mengirim dengan format `Publisher|Tahun|Path`. Setelah itu dilakukan penerimaan file dengan fungsi `readandSavefile()` yang juga akan dijelaskan di bawah. Setelah itu client akan dikonfirmasi terkait status sukses atau gagalnya penyimpanan file. Yang terakhir, dipanggil fungsi `logging()` dengan event `Tambah` yang akan dijelaskan pada [Jawaban Soal 1h](Jawaban-Soal-1h)

Fungsi `addDataBuku()` akan menyimpan data buku ke dalam `files.tsv`

```c
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
```

Fungsi `readandSavefile()` akan menerima file berupa chunk 4 KB beberapa kali sesuai request client, lalu menyimpannya menggunakan fungsi `writeToBinaryFile()` yang akan dijelaskan di bawah.

```c
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
```

Fungsi `writeToBinaryFile()` akan menyimpan setiap isi dari array chunk berupa integer ke dalam file menggunakan `fputc()`. Alasan penggunaan integer dibandingkan char adalah agar special character yang tidak masuk dalam ASCII dapat dikirimkan (agar file selain .txt bisa dikirim)

```c
void writeToBinaryFile(FILE * file, int chunk[], int size) {
    int i=0;
    for(; i<size; i++) {
        if(chunk[i] == -1) break;
        fputc(chunk[i], file);
    }
}
```

#### 1c pada client.c

Berikut adalah pemanggilan fungsi `addBuku()` apabila user memberi input `add`

```c
if(strcmp("add", action) == 0) {
    send(sock, action, MAX_INFORMATION_LENGTH, 0);
    addBuku(sock);
}
```

`addBuku()` akan menghandle input dari user, memformat dengan separator `|` dan mengirimnya ke server dengan fungsi `readFileandSend()` yang akan dijelaskan di bawah.

```c
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
        return;
    }
    printf("Buku gagal ditambahkan.\n");
}
```

Fungsi `readFileandSend()` akan membaca file dengan fungsi `readBinaryFile()` mengirimkan file chunk dari file dan menyelesaikannya apabila mencapai `EOF`.

```c
bool readFileandSend(int socket, char filename[]) {
    int chunk[MAX_FILE_CHUNK];
    char message[FAIL_OR_SUCCESS_LENGTH];
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
```

Fungsi `readBinaryFile()` akan membaca file menggunakan `fgetc()` hingga batas file chunk atau jika mencapai `EOF`.

```c
bool readBinaryFile(FILE * file, int chunk[], int size) {
    int i=0, charFromFile;
    for(i=0; i<size; i++) {
        charFromFile = fgetc(file);
        if(charFromFile == EOF) {
            chunk[i] = -1;
            break;
        }
        chunk[i] = charFromFile;
    }
    if(charFromFile == EOF) return false;
    else return true;
}
```

### **Jawaban Soal 1d**

#### 1d pada server.c

Fungsi `readFileandSend()` akan dipanggil dari server apabila user menginput download, fungsi ini cara kerjanya mirip dengan yang ada pada client seperti yang sudah dijelaskan di atas.

```c
if(readStatus > 0 && strcmp("download", action) == 0) {
    char filename[MAX_INFORMATION_LENGTH];
    memset(filename, 0, sizeof(filename));
    if(read(socket, filename, MAX_INFORMATION_LENGTH) <= 0)
        return NULL;
    readFileandSend(socket, filename);
}
```

#### 1d pada client.c

Fungsi `readandSavefile()` akan dipanggil dari client apabila user menginput download, fungsi ini cara kerjanya mirip dengan yang ada pada server seperti yang sudah dijelaskan di atas.

```c
if(strcmp("download", action) == 0) {
    send(sock, action, MAX_INFORMATION_LENGTH, 0);
    char filename[MAX_INFORMATION_LENGTH];
    getlineRemoveNewline(filename);
    send(sock, filename, sizeof(filename), 0);
    FILE * buku = readandSavefile(sock, filename);
    if(buku) fclose(buku);
}
```

### **Jawaban Soal 1e**

#### 1e pada server.c

Fungsi `deleteBook()` akan dipanggil apabila client memintanya.

```c
if(readStatus > 0 && strcmp("delete", action) == 0) {
    char filename[MAX_INFORMATION_LENGTH];
    memset(filename, 0, sizeof(filename));
    if(read(socket, filename, MAX_INFORMATION_LENGTH) <= 0)
        return NULL;
    deleteBook(socket, filename, authenticatedUser);
}
```

Fungsi tersebut akan memanggil `isBookExistInTsv()`, jika ada tidak ada maka akan mengirim pesan error ke client. Jika tidak, maka dilakukan rename sesuai soal dan menghapus rownya dari `files.tsv`

```c
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
```

Fungsi `isBookExistInTsv()` akan scan record pada `files.tsv` satu persatu dan return `true` apabila ditemukan.

```c
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
```

#### 1e pada client.c

Fungsi `deleteBook()` akan dipanggil ketika user memintanya.

```c
if(strcmp("delete", action) == 0) {
    send(sock, action, MAX_INFORMATION_LENGTH, 0);
    deleteBook(sock);
}
```

Fungsi tersebut akan mengirimkan detil nama file dan mengirimkannya ke server. Setelah itu mengolah status respon dari server (gagal / berhasil).

```c
void deleteBook(int sock) {
    char filename[MAX_INFORMATION_LENGTH];
    getlineRemoveNewline(filename);
    send(sock, filename, sizeof(filename), 0);
    char message[FAIL_OR_SUCCESS_LENGTH];
    memset(message, 0, FAIL_OR_SUCCESS_LENGTH);
    read(sock, message, FAIL_OR_SUCCESS_LENGTH);
    if(strcmp(message, failMsg) == 0) printf("Gagal menghapus file.\n");
    else printf("Sukses menghapus file.\n");
}
```

### **Jawaban Soal 1f**

#### 1f pada server.c

Fungsi `seeFilesTsv()` akan dipanggil apabila client memintanya.

```c
if(readStatus > 0 && strcmp("see", action) == 0) {
    seeFilesTsv(socket);
}
```

Fungsi `seeFilesTsv()` akan menghitung memanggil `getLineCount()` untuk mengetahui terdapat berapa baris pada `files.tsv`, setelah itu akan mengirimkan respon ke client berupa jumlah row dan detil setiap row.

```c
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
```

`getLineCount()` hanya perlu melakukan `getc()` dan menghitung jumlah line berdasarkan jumlah `\n` yang ada.

```c
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
```

#### 1f pada client.c

Fungsi `receiveFilesTsv()` dipanggil.

```c
if(strcmp("see", action) == 0) {
    send(sock, action, MAX_INFORMATION_LENGTH, 0);
    receiveFilesTsv(sock);
}
```

Fungsi `receiveFilesTsv()` akan menerima respon dari server berupa jumlah buku dan detilnya. Untuk setiap buku akan dipanggil fungsi `printBookInfo()` yang akan dijelaskan di bawah.

```c
void receiveFilesTsv(int socket) {
    char strLineCount[LINE_COUNT_STR_LENGTH];
    read(socket, strLineCount, LINE_COUNT_STR_LENGTH);
    int lineCount = atoi(strLineCount);
    while(lineCount--) {
        char information[MAX_INFORMATION_LENGTH];
        memset(information, 0, MAX_INFORMATION_LENGTH);
        read(socket, information, MAX_INFORMATION_LENGTH);
        printBookInfo(information);
    }
}
```

Fungsi `printBookInfo()` akan mengekstrak publisher, tahun, dan filepath menggunakan `strtok()` lalu dari filepath akan diambil nama filenya menggunakan `basename()` dan lebih spesifik lagi akan disaring extension dan nama filenya saja. Setelah itu diprint sesuai permintaan soal.

```c
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
```

### **Jawaban Soal 1g**

#### 1g pada server.c

Fungsi `findSpecificName()` akan dipanggil.

```c
if(readStatus > 0 && strcmp("find", action) == 0) {
    char subfilename[MAX_INFORMATION_LENGTH];
    memset(subfilename, 0, sizeof(subfilename));
    if(read(socket, subfilename, MAX_INFORMATION_LENGTH) <= 0)
        return NULL;
    findSpecificName(socket, subfilename);
}
```

Fungsi `findSpecificName()` akan melihat isi `files.tsv` dan mengekstrak nama filenya menggunakan cara seperti di atas. Setelah itu, yang memiliki substring sesuai akan dikirimkan ke client.

```c
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
```

#### 1g pada client.c

Fungsi `findSpecificName()` akan dipanggil.

```c
if(strcmp("find", action) == 0) {
    send(sock, action, MAX_INFORMATION_LENGTH, 0);
    findSpecificName(sock);
}
```

Fungsi `findSpecificName()` menerima respon dari server dan untuk setiap row-nya akan dipanggil fungsi `printBookInfo()` yang telah dijelaskan.

```c
void findSpecificName(int socket) {
    char filename[MAX_INFORMATION_LENGTH];
    getlineRemoveNewline(filename);
    send(socket, filename, sizeof(filename), 0);
    char strLineCount[LINE_COUNT_STR_LENGTH];
    read(socket, strLineCount, LINE_COUNT_STR_LENGTH);
    int lineCount = atoi(strLineCount);
    while(lineCount--) {
        char information[MAX_INFORMATION_LENGTH];
        memset(information, 0, MAX_INFORMATION_LENGTH);
        read(socket, information, MAX_INFORMATION_LENGTH);
        if(strcmp(information, failMsg) == 0) break;
        printBookInfo(information);
    }
}
```

### **Jawaban Soal 1h**

#### 1h pada server.c

Fungsi `logging()` akan menerima event berupa `Tambah` atau `Hapus` setelah itu akan memasukkan lognya ke folder `running.log` sesuai permintaan soal.

```c
void logging(const char event[], const char filepath[], char authenticatedUser[]) {
    FILE * logFile = fopen("running.log", "a");
    char copy_of_filepath[strlen(filepath) + 1];
    strcpy(copy_of_filepath, filepath);
    if(logFile) {
        fprintf(logFile, "%s : %s (%s)\n", event, basename(copy_of_filepath), authenticatedUser);
        fclose(logFile);
    }
}
```

### **Kendala pengerjaan No.1 :**

- Ketika demo, tiba-tiba fungsi untuk command `see` dan `find` tidak bekerja dan program harus diexit lalu dijalankan kembali baru bisa bekerja.
- Soal terlalu padat sehingga sangat menyita waktu.

### **Soal No. 2**

Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:

**a.** Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).

**b.** Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (**Catatan!**: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang ada. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya matri(dari paling besar ke paling kecil) (**Catatan!**: gunakan thread untuk perhitungan di setiap cel).

**Ketentuan:**

```a
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```

**Contoh :**

| A   | B   | Angka-Angka Faktorial |
| --- | --- | --------------------- |
| 4   | 4   | 4 3 2 1               |
| 4   | 5   | 4 3 2 1               |
| 4   | 3   | 4 3 2                 |
| 4   | 0   | 0                     |
| 0   | 4   | 0                     |
| 4   | 6   | 4 3 2 1               |

**Contoh :**

| Matriks A |     | Matriks B |     | Matriks Hasil |            |
| --------- | --- | --------- | --- | ------------- | ---------- |
| 0         | 4   | 0         | 4   | 0             | 4\*3\*2\*1 |
| 4         | 5   | 6         | 2   | 4\*3\*2\*1    | 5\*4       |
| 5         | 6   | 6         | 0   | 5\*4\*3\*2\*1 | 0          |

**c.** Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)

**Note:**

- Semua matriks berasal dari input ke program.
- Dilarang menggunakan system()

### **Jawaban Soal 2a**

Pada soal 2a kita diminta untuk membuat sebuah program yang digunakan untuk melakukan perkalian matriks 4x3 dan 3x6 yang dimasukkan ke program oleh user. Untuk kode yang berfungsi untuk menghitung perkalian matriks adalah sebagai berikut:

```C
//Fungsi hitung perkalian matriks
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
. 
. 
. 
. 
//Untuk membuat thread dan memanggil fungsi hitung matriks
for (i = 0; i < ROW; i++) {
     int err = pthread_create(&(tid[i]), NULL, &hitungmatriks, NULL);
     if(err!=0){
        printf("\n can't create thread : [%s]",strerror(err));
     }
}
```

Kemudian setelah melakukan operasi pada matriks dan menghasilkan yang merupakan hasil perkalian, hasil tersebut akan diberikan ke soal2b melalui *shared memory*, berikut kodenya:

```C
printf("\nMATRIKS HASIL\n");
int *share_matriks;

key_t key = 1234;
int shmid = shmget(key, sizeof(int[ROW][COL]), IPC_CREAT | 0666);
share_matriks = (int*)shmat(shmid, NULL, 0);

//Memasukkan matriks ke shared memory
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
```

Berikut ini sedikit penjelasan mengenai kode di atas.

- `shmid` disini mengembalikan nilai dari pemanggilan sistem `shmget(key, sizeof(int[ROW][COL]), IPC_CREAT | 0666)` yang digunakan untuk membuat suatu segmen dalam memori.
- `share_matriks = (int *)shmat(shmid, NULL, 0)` mendaftarkan atau mengattach segmen ke data space dari proses.
- `shmdt(share_matriks)` untuk mendetach segmen dari data space dari proses.
- `shmctl(shmid, IPC_RMID, NULL)` untuk mengetahui atau mengubah informasi yang berkaitan dengan suatu shared memory.

Berikut ini merupakan **source code** lengkap dari soal 2a:

```C
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

```

### **Jawaban Soal 2b**

Untuk soal2b kita diminta untuk mengambil matriks hasil operasi dari soal2a sebelumnya dari shared memory dan menampilkannya.

```C
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
```

Lalu user akan memasukkan matriks yang akan menjadi batas matriks batas, kemudian dengan matriks tersebut akan dilakukan operasi faktorial dengan ketentuan berikut:

- If a >= b  -> a!/(a-b)!
- If b > a -> a!
- If 0 -> 0

Pada soal ini diminta untuk membuat thread untuk menghitung setiap cell pada matriks nya, berikut untuk kodenya:

```C
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
. 
. 
. 
. 

//Untuk membuat thread dan menanggil fungsi mtx_thread
for (i = 0; i < ROW; i++) {
    for(j = 0; j < COL; j++){
        int err = pthread_create(&(tid[i*COL+j]), NULL, &mtx_thread, NULL);
        if(err!=0){
            printf("\n can't create thread : [%s]",strerror(err));
        }
    }
}

```

Kemudian berikut adalah **Source Code** lengkap dari soal2b:

```C
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
```

Lalu berikut untuk tampilan saat kode dari soal2a dan soal2b dijalankan:

![image](https://user-images.githubusercontent.com/40484843/119246083-a6aa2380-bba8-11eb-809f-223d9e3046c3.png)

### **Jawaban Soal 2c**

Pada soal2c kita diminta untuk membuat program yang akan menampilkan 5 program teratas yang memakan resource komputer sekarang, disini diminta untuk menggunakan pipe. Caranya adalah dengan membuat 2 buah pipe yang menghubungkan 3 command, berikut kode nya:

```C
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
```

Pertama adalah membuat pipe pertama kemudian menjalankan fork untuk mengeksekusi perintah `ps aux`. Kedua yaitu membuat pipe kedua dan menjalankan fork untuk mengeksekusi perintah `sort -nrk 3,3`. Lalu terakhir yaitu menjalankan fork untuk mengeksekusi perintah `head -5`.

berikut merupakan tampilan saat menjalankan program soal2c:

![image](https://user-images.githubusercontent.com/40484843/119246524-82e8dc80-bbac-11eb-9a00-a54d9a712486.png)

### **Kendala pengerjaan No.2:**

1. Kendala untuk soal no 2a adalah kesulitan untuk menyimpan matriks pada shared memory karena berbentuk matriks 2 dimensi
2. Kendala untuk soal no 2b adalah kesulitan untuk membuat thread untuk tiap cell pada matriks, karena itu membutuhkan banyak thread.
3. Kendala untuk soal no 2c adalah saat program dijalankan, program tersebut tidak langsung menutup dan harus di terminate secara manual. Untuk masalah ini sudah terselesaikan dengan melakukan terminate dalam program(revisi)

### **Soal No. 3**

Seorang mahasiswa bernama Alex sedang mengalami masa gabut. Di saat masa gabutnya, ia memikirkan untuk merapikan sejumlah file yang ada di laptopnya. Karena jumlah filenya terlalu banyak, Alex meminta saran ke Ayub. Ayub menyarankan untuk membuat sebuah program C agar file-file dapat dikategorikan. Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.

Contoh apabila program dijalankan:

```text
# Program soal3 terletak di /home/izone/soal3
$ ./soal3 -f path/to/file1.jpg path/to/file2.c path/to/file3.zip
#Hasilnya adalah sebagai berikut
/home/izone
|-jpg
    |--file1.jpg
|-c
    |--file2.c
|-zip
    |--file3.zip
```

**(a)** Program menerima opsi -f seperti contoh di atas, jadi pengguna bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan oleh pengguna.

Output yang dikeluarkan adalah seperti ini :

```text
File 1 : Berhasil Dikategorikan (jika berhasil)
File 2 : Sad, gagal :( (jika gagal)
File 3 : Berhasil Dikategorikan
```

**(b)** Program juga dapat menerima opsi -d untuk melakukan pengkategorian pada suatu directory. Namun pada opsi -d ini, user hanya bisa memasukkan input 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin.

Contohnya adalah seperti ini:

```bash
./soal3 -d /path/to/directory/
```

Perintah di atas akan mengkategorikan file di /path/to/directory, lalu hasilnya akan disimpan di working directory dimana program C tersebut berjalan (hasil kategori filenya bukan di /path/to/directory).

Output yang dikeluarkan adalah seperti ini :

```text
Jika berhasil, print “Direktori sukses disimpan!”
Jika gagal, print “Yah, gagal disimpan :(“
```

**(c)** Selain menerima opsi-opsi di atas, program ini menerima opsi \*, contohnya ada di bawah ini:

```bash
./soal3 \*
```

Opsi ini akan mengkategorikan seluruh file yang ada di working directory ketika menjalankan program C tersebut.

**(d)** Semua file harus berada di dalam folder, jika terdapat file yang tidak memiliki ekstensi, file disimpan dalam folder “Unknown”. Jika file hidden, masuk folder “Hidden”.

**(e)** Setiap 1 file yang dikategorikan dioperasikan oleh 1 thread agar bisa berjalan secara paralel sehingga proses kategori bisa berjalan lebih cepat.

Namun karena Ayub adalah orang yang hanya bisa memberi ide saja, tidak mau bantuin buat bikin programnya, Ayub meminta bantuanmu untuk membuatkan programnya. Bantulah agar program dapat berjalan!

Catatan:

- Kategori folder tidak dibuat secara manual, harus melalui program C
- Program ini tidak case sensitive. Contoh: JPG dan jpg adalah sama
- Jika ekstensi lebih dari satu (contoh “.tar.gz”) maka akan masuk ke folder dengan titik terdepan (contoh “tar.gz”)
- Dilarang juga menggunakan fork-exec dan system()
- Bagian b dan c berlaku rekursif

### **Jawaban Soal 3a,d,e**

```c
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
	...
```
Pertama menerima input dalam main dengan `int main(int argc, char *argv[])` dan jika menerima perintah `-f` dalam potongan kodingan berikut `if(strcmp(argv[1], "-f") == 0)` kemudian dibuat thread untuk ke dalam fungsi `categorize(void *arg)` sebanyak argc-2 dalam potongan kodingan berikut ini. Fungsi ini membuat dir atau memasukin file sesuai extensi jika file tersebut ada dan bukan direktori.
```c
	for(int i=2; i<argc; i++){
            pthread_create(&tid[count], NULL, categorize, (void *)argv[i]);
            count++;
        }
```
Fungsi categorize

```c
void* categorize(void *arg){
    char *src = (char *)arg;
    struct stat path_stat;
    stat(src, &path_stat);
    if(access(src, F_OK)!=0|| S_ISDIR(path_stat.st_mode)){
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
    pthread_mutex_lock(&signal);
    if(rename(srcP, destP) != 0){
        fprintf(stderr,"error: %s\n\n",strerror(errno));
    }
    pthread_mutex_unlock(&signal);
    return (void *) 1;
    }
    pthread_mutex_lock(&signal);
    pthread_mutex_unlock(&signal); 
}
```
Fungsi ini pertama cek apabila path yang diberikan tidak ada dan apabila path tersebut direktori dalam kodingan berikut `if(access(src, F_OK)!=0|| S_ISDIR(path_stat.st_mode))`, jika masuk syarat tersebut maka dilakukan `return (void *) 0;`.
Kemudian path tersebut masuk pada fungsi `checkExt(char *dir)` yang disimpan dalam `char *srcExt`. Fungsi ini mencari extensi dari path yang diberikan.

```c
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
```
Dalam fungsi ini pertama dicek apabila file tersebut file Hidden dengan `if(extdot[1]==dot[0])` dimana `dot[0]` merupakan titik dan `extdot` merupakan nama file yang digunakan `strrchr(dir,'/')`, fungsi ini mencari `/` paling akhir. Jika masuk syarat hidden file maka `return hid`, mengembalikan nama direktori `Hidden` Lalu dicek jika memiliki extension, dimana `if(ext != NULL)` maka dilakukan `return lowercase(ext + 1)` mengembalikan extention untuk nama direktori dalam bentuk lowercase. Jika tidak memiliki extention maka `return unk` mengembalikan nama direktori `Unknown`. Berikut fungsi `lowercase()`
```c
char *lowercase(char *str){
    unsigned char *temp = (unsigned char *)str;

    while(*temp){
        *temp = tolower(*temp);
        temp++;
    }
    return str;
}
```
Lalu sudah dikembalikan nama direktori dibuatlah direktori tersebut jika belum ada dengen kodingan berikut.
```c
    DIR *dir = opendir(srcExt);
    if(dir == NULL){
        mkdir(srcExt, S_IRWXU);
    }
```
`S_IRWXU` ini agar dapat permission read, write, extecute/search.

Setelah dibuat direktori tersebut disimpan dulu nama file tersebut dengan fungsi `chekName()` dan menyimpan path working direktori dengan fungsi `getenv("PWD")`.
```c
    char *srcName = checkName(srcP);
    char *curr = getenv("PWD");
```
Berikut fungsi checkName
```c
char *checkName(char *dir){
    char *name = strrchr(dir, '/');
    if(name == dir)
        return "";
    return name + 1;
}
```
Sesudah menyimpan nama file dan path working direktori dibuatlah string untuk menyimpan path untuk ke direktori baru sebagai berikut.
```c
    char *destP = malloc(strlen(srcExt) + 1 + strlen(srcName) + 1 + strlen(curr) + 1);
    strcpy(destP, curr);
    strcat(destP, "/");
    strcat(destP, srcExt);
    strcat(destP, "/");
    strcat(destP, srcName);
```
Fungsi malloc agar menyediakan tempat untuk variable `destP`, kemudian fungsi `strcpy(destP, curr)` untuk copy path working direktori ke `destP`. Lalu dilakukan concate "/", nama file direktori, "/",dan nama file.

Sesudah path destinasi lalu dipindah menggunakan fungsi rename.
```c
    if(rename(srcP, destP) != 0){
        fprintf(stderr,"error: %s\n\n",strerror(errno));
    }
```
Lalu `return (void *) 1` menandakan berhasil dilakukan categorize.

Kembali pada main,
```c
        ...
        
        count=0;
        for(count=0; count<argc-2; count++){
        
            pthread_join(tid[count], &flag);
            x = (int)flag;
            if(x)printf("File %d : Berhasil Dikategorikan\n", count+1);
            else if(!x)printf("File %d : Sad, gagal :(\n",count+1);
        }
```
Dilakukan pthread_join dan menyimpan nilai return dalam variable flag agar dapat mengtahui jika berhasil atau tidak dilakukannya categorize.

### **Jawaban Soal 3b,d,e**

```c
    ...
    
    }else if(strcmp(argv[1], "-d") == 0){
	void *flag=0;
	int x=0;
	listFilesRecursively(argv[2]);
    
	...
```
Jika menerima perintah -d maka dilakukan fungsi `listFilesRecursively` untuk menyimpan path file - file secara recursive.

```c
char catepath[100][100];
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
```
Jika file tersebut bukan direktori maka akan disimpan path dalam variable catepath dan menggunakan countlistrec sebagai counter berapa banyak file yang dikategorikan.
Kembali ke main,
```c
	...
	
        count=0;
        for(count=0; count<countlistrec; count++){
            pthread_join(tid[count], &flag);
            x = (int)flag;
        }
	if(x)printf("Direktori suskses disimpan!\n");
        else if(!x || countlistrec==0)printf("Yah, gagal disimpan :(\n");
        return 0;
	
	...
```
Dilakukan pthread_join seperti soal 3a tetapi ditambahkan syarat gagal jika `countlistrec==0` maka gagal dilakukan kategori.

### **Jawaban Soal 3c,d,e**
Dalam main
```c
   ...
   
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
```
Dalam soal 3c sama persis dengan no 3d hanya perbedaan path yang digunakan adalah path working direktori yang didapatkan lewat `getenv("PWD")`.


### **Kendala Soal No. 3**

1. Pada saat mengerjakan 3a bermasalah saat file tidak ditemukan tetapi masih melakukan output berhasil.
2. Pada saat mengerjakan 3b bermasalah saat file tidak ditemukan mendapat error segemntation fault berkali - kali

![Screenshot 2021-05-06 210519](https://user-images.githubusercontent.com/63639703/119249949-ff88b480-bbc6-11eb-9a35-86f09485abe5.png)
