#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

// mengatur rupiah
struct format_rupiah : numpunct<char>
{
    char do_thousands_sep() const { return '.'; }
    string do_grouping() const { return "\3"; }
};

// database produk
struct produk
{
    string nama;
    string spesifikasi;
    double harga;
    int stok;
};

struct Nota
{
    string namaProduk;
    int jumlah;
    double totalHarga;
    Nota *next;
    Nota *prev;
};

Nota *head = NULL;
Nota *tail = NULL;

// database akun
//==============================
const int MAX_AKUN = 100;
int jml_akun = 0;

string username[MAX_AKUN];
string password[MAX_AKUN];

char pilihan_login;

// database produk
//==============================
const int MAX_PRODUK = 100;
produk daftar_produk[MAX_PRODUK];
int jml_produk = 0;

// type data untuk menu
int pilihan = 0, pilihan_awal = 0, pilih = 0;

// type data untuk kode penjual
struct kode
{
    string kode_penjual;
};

bool login_kode;
kode Codes[1] = {"11223344"};
string kode_kode;

// fungsi untuk login pembeli
bool login(string user, string pass)
{
    for (int i = 0; i < jml_akun; i++)
    {
        if (username[i] == user && password[i] == pass)
            return true;
    }
    return false;
}
// Fungction Menu Utama

void menu_awal()
{
    cout << "======= MENU AWAL =======" << endl;
    cout << "|  1.Sebagai Penjual    |" << endl;
    cout << "|  2.Sebagai Pembeli    |" << endl;
    cout << "|  3.keluar             |" << endl;
    cout << "==========================" << endl;
    cout << "Masukan pilihan anda : ";
    cin >> pilihan_awal;
    cin.ignore();
    system("cls");
}

// Function Login Penjual
void menu_penjual()
{
    cout << "======= MENU ADMIN =======" << endl;
    cout << "|  1. Tambah Barang      |" << endl;
    cout << "|  2. Hapus Barang       |" << endl;
    cout << "|  3. Lihat data barang  |" << endl;
    cout << "|  4. Keluar             |" << endl;
    cout << "==========================" << endl;
    cout << "Masukkan pilihan anda : ";
    cin >> pilih;
    system("cls");
}

// Function Produk Atau Menu penjual

// Function Simpan Produk Ke database
void simpan_produk()
{
    ofstream file("database_produk.txt");
    if (!file)
    {
        cout << "Gagal Membuka file untuk menyimpan!\n";
        return;
    }
    for (int i = 0; i < jml_produk; i++)
    {
        file << daftar_produk[i].nama << "|";
        file << daftar_produk[i].spesifikasi << "|";
        file << fixed << setprecision(0) << daftar_produk[i].harga << "|"; // membuat angka desimal dibulatkan dan menghapus semua angka di belakang koma
        file << daftar_produk[i].stok << "|" << endl;
    }
    file.close();
    cout << "Data produk berhasil disimpan\n";
}

string ambil_kolom(const string &line, int index)
{
    size_t awal = 0;
    for (int i = 0; i < index; i++)
    {
        size_t pos = line.find("|", awal);
        if (pos == string::npos)
            return "";
        awal = pos + 1;
    }

    size_t akhir = line.find("|", awal);
    if (akhir == string::npos)
        akhir = line.length();

    return line.substr(awal, akhir - awal);
}

// Function Muat Produk Dari database
int muat_produk()
{
    ifstream file("database_produk.txt");
    if (!file)
    {
        cout << ">> Database belum ada, mulai dengan data default.\n";
        return 0;
    }
    jml_produk = 0;
    string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|", pos1 + 1);
        size_t pos3 = line.find("|", pos2 + 1);
        size_t pos4 = line.find("|", pos3 + 1);

        daftar_produk[jml_produk].nama = line.substr(0, pos1);
        daftar_produk[jml_produk].spesifikasi = line.substr(pos1 + 1, pos2 - pos1 - 1);
        daftar_produk[jml_produk].harga = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
        daftar_produk[jml_produk].stok = stoi(line.substr(pos3 + 1, pos4 - pos3 - 1));
        jml_produk++;
    }
    file.close();
    return jml_produk;
}

// Function Tambah Produk
void tambah_produk()
{
    if (jml_produk >= MAX_PRODUK)
    {
        cout << "Database penuh tidak dapat menambah produk lagi\n";
        return;
    }

    cin.ignore();
    cout << "Masukan nama produk :";
    getline(cin, daftar_produk[jml_produk].nama);
    cout << "Masukan spesifikasi produk :";
    getline(cin, daftar_produk[jml_produk].spesifikasi);
    cout << "Masukan harga produk : Rp.";
    cin >> daftar_produk[jml_produk].harga;
    cout << "Masukan jumlah stok :";
    cin >> daftar_produk[jml_produk].stok;
    jml_produk++;
    simpan_produk();
}

void sorting(){
    for(int i = 1; i < jml_produk; i++){
        produk key = daftar_produk[i];
        int j = i - 1;

        //bandingkan harga
        while(j >= 0 && daftar_produk[j].harga > key.harga) {
            daftar_produk[j + 1] = daftar_produk[j];
            j = j - 1;
        }
        daftar_produk[j + 1] = key;
    }
    cout << "Daftar produk sudah diurutkan dari harga terendah" << endl;
}

// Function Tampilkan Produk
void tampilkan_produk()
{
    if (jml_produk == 0)
    {
        cout << "Tidak ada produk yang tersedia.\n";
        return;
    }
    cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "========== DAFTAR PRODUK ==========" << endl;
    for (int i = 0; i < jml_produk; i++)
    {
        cout << i + 1 << "."
             << daftar_produk[i].nama
             << "|" << daftar_produk[i].spesifikasi
             << "|" << fixed << setprecision(0) << daftar_produk[i].harga
             << "|" << daftar_produk[i].stok << endl;
    }
    cout << "===================================" << endl;
    char pilih_sorting;
    cout << "urutkan produk dari harga terendah? (y/t): ";
    cin >> pilih_sorting;
    cin.ignore();

    if(pilih_sorting == 'y' || pilih_sorting == 'Y'){
        system("cls");
        sorting();
        cout << "========== DAFTAR PRODUK ==========" << endl;
        for (int i = 0; i < jml_produk; i++)
    {
        cout << i + 1 << "."
             << daftar_produk[i].nama
             << "|" << daftar_produk[i].spesifikasi
             << "|" << fixed << setprecision(0) << daftar_produk[i].harga
             << "|" << daftar_produk[i].stok << endl;
    }
    cout << "===================================" << endl;
    }
    cout << "Tekan enter untuk kembali ke menu";
    cin.get();
    system("cls");
}

// Function Pembeli Atau Menu Pembeli

// Function Menu Login Pembeli
void menu_login_buyer()
{
    cout << "======= MENU LOGIN =======" << endl;
    cout << "|       1.Login          |" << endl;
    cout << "|       2.Register       |" << endl;
    cout << "|       3.keluar         |" << endl;
    cout << "==========================" << endl;
    cout << "Masukan pilihan anda : ";
    cin >> pilihan_login;
    cin.ignore();
    system("cls");
}

// Function Simpan Akun Ke database
void simpan_akun()
{
    ofstream file("database_akun.txt");
    if (!file)
    {
        cout << "Gagal Membuka file untuk menyimpan!\n";
        return;
    }
    for (int i = 0; i < jml_akun; i++)
    {
        file << username[i] << "|" << password[i] << endl;
    }
    file.close();
    cout << "Data akun berhasil disimpan" << endl;
}

// Function Muat Akun Dari database
int muat_akun()
{
    ifstream file("database_akun.txt");
    if (!file)
    {
        cout << ">> Database belum ada, mulai dengan data default.\n";
        return 0;
    }
    jml_akun = 0;
    while (getline(file, username[jml_akun], '|') &&
           getline(file, password[jml_akun]))
    {
        jml_akun++;
    }
    file.close();
    return jml_akun;
}

// Function Tambah Akun
void tambah_akun()
{
    cout << "Masukan username : ";
    getline(cin, username[jml_akun]);
    cout << "Masukkka password : ";
    getline(cin, password[jml_akun]);
    jml_akun++;
    simpan_akun();
    system("cls");
    cout << "Selamat anda berhasil mendaftar, selamat berbelanja" << endl;
}

// Function Menu Utama Pembeli
void menu_utama()
{
    cout << "========= MENU UTAMA =========" << endl;
    cout << "|    1. Lihat Produk         |" << endl;
    cout << "|    2. Cari Produk          |" << endl;
    cout << "|    3. Tambah ke Keranjang  |" << endl;
    cout << "|    4. Lihat Keranjang      |" << endl;
    cout << "|    5. Hapus dari Keranjang |" << endl;
    cout << "|    6. Checkout             |" << endl;
    cout << "|    7. Riwayat Pembelian    |" << endl;
    cout << "|    8. Logout               |" << endl;
    cout << "|    9. Keluar               |" << endl;
    cout << "==============================" << endl;
    cout << "Masukan pilihan anda : ";
    cin >> pilihan;
    cin.ignore();
    system("cls");
}

void search_produk() {
    string cari;
    bool ditemukan = false;

    cout << "Masukkan nama produk yang dicari : ";
    getline(cin, cari);

    cout.imbue(locale(cout.getloc(), new format_rupiah));
    for (int i = 0; i < jml_produk; i++) {
        if (daftar_produk[i].nama == cari) {
            cout << "\n=== PRODUK DITEMUKAN ===" << endl;
            cout << "Nama         : " << daftar_produk[i].nama << endl;
            cout << "Spesifikasi  : " << daftar_produk[i].spesifikasi << endl;
            cout << "Harga        : Rp." << fixed << setprecision(0) << daftar_produk[i].harga << endl;
            cout << "Stok         : " << daftar_produk[i].stok << endl;
            ditemukan = true;
            break;
        }
    }
}

void tambah_nota(string nama, int jumlah, double total)
{
    Nota *baru = new Nota;
    baru->namaProduk = nama;
    baru->jumlah = jumlah;
    baru->totalHarga = total;
    baru->next = NULL;
    baru->prev = NULL;

    if (head == NULL)
    {
        head = tail = baru;
    }else
    {
        tail->next = baru;
        baru->prev = tail;
        tail = baru;
    }
}

void tampil_riwayat()
{
    Nota *temp = head;

    if (temp == NULL)
    {
        cout << "Belum ada riwayat pembelian\n";
        return;
    }

    while (temp !=NULL)
    {
        cout << "Produk  : " << temp->namaProduk << endl;
        cout << "Jumlah  : " << temp->jumlah << endl;
        cout << "Total   : " << temp->totalHarga << endl;
        cout << "___________________" << endl;

        temp = temp->next;
    }
}

void Checkout()
{
    string nama;
    int jumlah;

    cout << "Masukkan nama produk: ";
    getline(cin, nama);

    cout << "Masukkan jumlah: ";
    cin >> jumlah;
    cin.ignore();

    for (int i = 0; i < jml_produk; i++)
    {
        if (daftar_produk[i].nama == nama)
        {
            double total = daftar_produk[i].harga * jumlah;
            tambah_nota(nama, jumlah, total);
            daftar_produk[i].stok -= jumlah;
            if (jumlah > daftar_produk[i].stok)
            {
                cout << "Stok tidak cukup!\n";
                return;
            }

            cout << "Checkout Berhasil!\n";
            return;
        }
    }
}

int main()
{
muat_akun();
muat_produk();

// menu awal
menu_awal:
    do
    {
        menu_awal();
        switch (pilihan_awal)
        {
        case 1:
        menu_login_penjual:
            cout << "======= MENU PENJUAL =======" << endl;
            cout << "Masukkan Kode : ";
            getline(cin, kode_kode);
            system("cls");

            // CEK KODE BERHASIL
            if (kode_kode == Codes[0].kode_penjual)
            {
                login_kode = true;
                cout << "================ Login Berhasil ================" << endl;
                cout << "|            Selamat datang Admin!             |" << endl;
                cout << "================================================";
                cin.get();
                system("cls");
                goto menu_penjual;
            }
            else
            {
                cout << "================= Kode Gagal =================" << endl;
                cout << "|       Kode yang anda masukan salah!        |" << endl;
                cout << "|                 Coba lagi!                 |" << endl;
                cout << "==============================================";
                cin.get();
                system("cls");
                goto menu_login_penjual;
            }
            break;
        case 2:
            goto menu_login;
            break;

        case 3:
            return 0;
            break;

        default:
            cout << "============================****=============================" << endl;
            cout << "|      pilihan salah, tekan enter untuk coba lagi           |" << endl;
            cout << "============================****=============================";
            cin.get();
            system("cls");
            goto menu_awal;
            break;
            break;
        }
    } while (pilihan_awal != 3);

    // menu penjual
menu_penjual:
    do
    {
        menu_penjual();
        switch (pilih)
        {
        case 1:
            tambah_produk();
            break;
        case 2:
            break;
        case 3:
            tampilkan_produk();
            break;
        case 4:
            system("cls");
            break;

        default:
            break;
        }
    } while (pilih != 4);

    // menu pembeli
    muat_akun();
menu_login:
    do
    {
        menu_login_buyer();
        switch (pilihan_login)
        {
        case '1':
        {
            string user, pass;
            cout << "Masukkan username : ";
            getline(cin, user);
            cout << "Masukkan Password :";
            getline(cin, pass);

            if (login(user, pass))
            {
                cout << "Login berhasil" << endl;
                cout << "Selamat berbelanja" << endl;
            }
            else
            {
                cout << "Login Gagal" << endl;
                cout << "Silahkan coba lagi" << endl;
            }
            system("cls");
            cout << "Selamat datang di marketplace mini" << endl;
            cout << "Tekan enter untuk melanjutkan";
            cin.get();
            goto menu_utama;
            system("cls");
            break;
        }
        case '2':
            tambah_akun();
            break;
        case '3':
            return 0;

        default:
            cout << "============================****=============================" << endl;
            cout << "|      pilihan salah, tekan enter untuk coba lagi           |" << endl;
            cout << "============================****=============================";
            cin.get();
            system("cls");
            goto menu_login;
            break;
        }
    } while (pilihan_login != 3);

// menu utama
menu_utama:
    do
    {
        menu_utama();
        switch (pilihan)
        {
        case 1:
            tampilkan_produk();
            break;
        case 2:
            search_produk();
            break;
        case 3:

            break;
        case 4:

            break;
        case 5:

            break;
        case 6:
            Checkout();
            break;
        case 7:
            tampil_riwayat();
            break;
        case 8:

            break;
        case 9:
            cout << "\nTerimakasih" << endl;
            break;

        default:
            break;
        }

    } while (pilihan != 9);
}