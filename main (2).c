#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

/* Kamus Umum */
typedef struct {
    float VolumeTotal;     /* Menyatakan daya tampung maksimum tanki */
    float VolumeCurrent;   /* Menyatakan volume air saat tertentu */
} TankiAir;

enum statustangki {
    KOSONG, TIDAKPENUH, PENUH
};

TankiAir T;                    /* T adalah objek Tanki Air */
int n = 1000;                   /* Kapasitas Air */
char Tombol;                   /* Tombol pemilihan aksi */
int JumlahAir;                 /* Jumlah Air yang diambil */
const char *stringstatustangki[] = {"Kosong", "Tidak Penuh", "Penuh"};
enum statustangki varstatus;

float totalBiaya = 0.0;  // Variabel untuk menghitung total biaya
float totalAirDikeluarkan = 0.0;  // Variabel untuk menghitung total air yang dikeluarkan

/* Deklarasi Fungsi */
void CreateTanki(TankiAir *V) {
    V->VolumeTotal = 1000.0;     /* Daya tampung maksimum */
    V->VolumeCurrent = 0.0;     /* Awal penciptaan, tanki kosong */
}

void Isi_Tanki(TankiAir *V) {
    V->VolumeCurrent = V->VolumeTotal; /* Isi tanki hingga penuh */
}

TankiAir KurangiVolumeTanki(TankiAir V, float JmlAir) {
    if (V.VolumeCurrent >= JmlAir) {
        V.VolumeCurrent -= JmlAir; /* Kurangi volume air sesuai jumlah */
        totalAirDikeluarkan += JmlAir;  // Tambah total air yang dikeluarkan
        totalBiaya += JmlAir * 5000;  // Tambah biaya berdasarkan volume air yang diambil
    } else {
        totalAirDikeluarkan += V.VolumeCurrent;  // Tambah sisa air yang dikeluarkan
        totalBiaya += V.VolumeCurrent * 5000;  // Tambah biaya sesuai volume yang tersisa
        V.VolumeCurrent = 0; /* Jika air kurang, kosongkan tanki */
    }
    return V;
}

bool IsEmptyTanki(TankiAir V) {
    return V.VolumeCurrent <= 0;
}

bool IsFullTanki(TankiAir V) {
    return V.VolumeCurrent >= V.VolumeTotal;
}

float IsSisaVolume(TankiAir V) {
    return V.VolumeCurrent;
}

float IsKapasitas(TankiAir V) {
    return V.VolumeTotal;
}

enum statustangki IsStatusTangki(TankiAir V) {
    if (IsEmptyTanki(V)) {
        return KOSONG;
    } else if (IsFullTanki(V)) {
        return PENUH;
    } else {
        return TIDAKPENUH;
    }
}

void delay(int milliseconds) {
    clock_t start_time = clock();
    while (clock() < start_time + milliseconds * CLOCKS_PER_SEC / 1000);
}

#ifdef __linux__
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

/* Program Utama */
int main() {
    /* ALGORITMA / PROSES PROGRAM UTAMA */
    CreateTanki(&T);
    printf("Awal penciptaan/pendefinisian, Volume Tanki Air sbb:\n");
    printf("Kapasitas Tanki   : %.2f m^3\n", IsKapasitas(T));
    printf("Volume Current    : %.2f m^3\n", IsSisaVolume(T));
    printf("Sedang ngisi...\n");
    delay(5000); /* Delay 5 detik */
    Isi_Tanki(&T);
    printf("Tanki sudah diisi ---> Tanki Penuh\n");

    // Menanyakan apakah ingin mengambil air
    printf("Apakah anda ingin mengambil air? (y/t): ");
    char pilihan;
    scanf(" %c", &pilihan);

    if (pilihan == 't' || pilihan == 'T') {
        // Menyimpan data ke file CSV
        FILE *file = fopen("output.csv", "w");
        if (file) {
            fprintf(file, "Total Air Dikeluarkan (m^3): %.2f\n", totalAirDikeluarkan);
            fprintf(file, "Total Biaya (Rp): Rp. %.2f\n", totalBiaya);
            fclose(file);
            printf("Data telah disimpan ke output.csv\n");
        } else {
            printf("Gagal menyimpan data ke file.\n");
        }
        printf("Program selesai.\n");
        return 0;
    }

    /* Loop utama: Ambil air terus-menerus sampai tombol M ditekan atau tangki kosong */
    do {
        printf("Sistem Hidup --> Volume Tangki Air: %.2f m^3\n", IsSisaVolume(T));
        printf("Status Tangki Air: %s\n", stringstatustangki[IsStatusTangki(T)]);
        printf("Ketik \"M\" untuk berhenti\n");

        if (!IsEmptyTanki(T)) {
            int r = rand() % 20;  // Menentukan jumlah air yang diambil (random)
            T = KurangiVolumeTanki(T, r);  // Kurangi volume air sesuai jumlah yang diambil
        }

        delay(2500);  // Menunggu selama 2,5 detik sebelum mengambil air lagi

        if (kbhit()) {  // Cek apakah tombol telah ditekan
            Tombol = getchar();  // Mendapatkan input tombol
            if (Tombol == 'M' || Tombol == 'm') {
                printf("Sistem dimatikan...\n");
                break;  // Keluar dari loop jika tombol 'M' ditekan
            }
        }

    } while (!IsEmptyTanki(T));  // Program terus berjalan sampai tangki kosong atau tombol 'M' ditekan

    // Menyimpan data ke file CSV
    FILE *file = fopen("output.csv", "w");
    if (file) {
        fprintf(file, "Total Air Dikeluarkan (m^3): %.2f\n", totalAirDikeluarkan);
        fprintf(file, "Total Biaya (Rp): Rp. %.2f\n", totalBiaya);
        fclose(file);
        printf("Data telah disimpan ke output.csv\n");
    } else {
        printf("Gagal menyimpan data ke file.\n");
    }

    // Menampilkan total biaya dan air yang dikeluarkan
    printf("Total air yang dikeluarkan: %.2f m^3\n", totalAirDikeluarkan);
    printf("Total biaya yang diperlukan untuk air yang diambil: Rp.%.2f\n", totalBiaya);

    return 0;
}
