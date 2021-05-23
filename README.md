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

### **Jawaban Soal 1a**

### **Jawaban Soal 1b**

### **Jawaban Soal 1c**

### **Jawaban Soal 1d**

### **Jawaban Soal 1e**

### **Jawaban Soal 1f**

### **Jawaban Soal 1g**

### **Jawaban Soal 1h**

### **Kendala pengerjaan No.1 :**

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
