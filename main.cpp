#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
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

struct Keranjang
{
    string namaProduk;
    int jumlah;
    double harga;
};

struct new_produk
{
    string nama;
    string spesifikasi;
    double harga;
    int stok;
} new_produk;

struct Nota
{
    string namaProduk;
    int jumlah;
    double totalHarga;
    string waktu;
    string status;
    Nota *next;
    Nota *prev;
};

struct Queue
{
    string namaProduk;
    int jumlah;
};

Nota *head = NULL;
Nota *tail = NULL;

Queue antrian[100];
int front = 0;
int rear = 0;

Keranjang keranjang[100];
int jml_keranjang = 0;

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
    cout << "|  3. Edit Harga Barang  |" << endl;
    cout << "|  4. Lihat data barang  |" << endl;
    cout << "|  5. Lihat Antrian      |" << endl;
    cout << "|  6. Proses Pengiriman  |" << endl;
    cout << "|  7. Keluar             |" << endl;
    cout << "==========================" << endl;
    cout << "Masukkan pilihan anda : ";
    cin >> pilih;
    cin.ignore();
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

void simpan_nota()
{
    ofstream file("database_nota.txt");
    if(!file)
    {
        cout << "Gagal Membuka file untuk menyimpan!\n";
        return;
    }
    Nota *temp = head;

    while(temp != NULL)
    {
        file << temp->namaProduk << "|";
        file << temp->jumlah << "|";
        file << temp->totalHarga << "|";
        file << temp->waktu << "|";
        file << temp->status << endl;

        temp = temp->next;
    }

    file.close();
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

void muat_nota()
{
    ifstream file("database_nota.txt");

    if (!file)
    {
        return;
    }

    string nama, jumlahStr, totalStr, waktu, status;

    while (
        getline(file, nama, '|') &&
        getline(file, jumlahStr, '|') &&
        getline(file, totalStr, '|') &&
        getline(file, waktu, '|') &&
        getline(file, status)
    )
    {
        Nota *baru = new Nota;

        baru->namaProduk = nama;
        baru->jumlah = stoi(jumlahStr);
        baru->totalHarga = stod(totalStr);
        baru->waktu = waktu;
        baru->status = status;
        baru->next = NULL;
        baru->prev = NULL;

        if (head == NULL)
        {
            head = tail = baru;
        }
        else
        {
            tail->next = baru;
            baru->prev = tail;
            tail = baru;
        }
    }

    file.close();
}

// Function Tambah Produk
void tambah_produk()
{
    int idx = -1;

    if (jml_produk >= MAX_PRODUK)
    {
        cout << "Database penuh tidak dapat menambah produk lagi\n";
        return;
    }

    cin.ignore();
    cout << "Masukan nama produk :";
    getline(cin, new_produk.nama);
    for (int i = 0; i < jml_produk; i++)
    {
        if (daftar_produk[i].nama == new_produk.nama)
        {
            cout << "Masukan spesifikasi produk :";
            getline(cin, new_produk.spesifikasi);
            if (daftar_produk[i].spesifikasi == new_produk.spesifikasi)
            {
                idx = i;
                cout << "Produk Sudah Tersedia!\n";
                daftar_produk[idx].stok++;
                simpan_produk();
                break;
            }
        }
    }
    if (idx == -1)
    {
        if (new_produk.spesifikasi.empty())
        {
            cout << "Masukan spesifikasi produk baru :";
            getline(cin, new_produk.spesifikasi);
        }
        cout << "Masukan harga produk : Rp.";
        cin >> new_produk.harga;
        cout << "Masukan jumlah stok :";
        cin >> new_produk.stok;
        daftar_produk[jml_produk].nama = new_produk.nama;
        daftar_produk[jml_produk].spesifikasi = new_produk.spesifikasi;
        daftar_produk[jml_produk].harga = new_produk.harga;
        daftar_produk[jml_produk].stok = new_produk.stok;
        jml_produk++;
        simpan_produk();
    }
}

//Function Edit harga produk
void edit_harga_produk()
{
    if (jml_produk == 0)
    {
        cout << "Tidak ada produk yang tersedia untuk diedit.\n";
        return;
    }

    cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "================= EDIT HARGA PRODUK =================" << endl;
     cout << left << setw(4) << "| NO" << "|"
          << setw(28) << "    NAMA   " << "|"
          << setw(18) << "  Harga Saat Ini" << "|" << endl;
    cout << "=====================================================" << endl;
    for (int i = 0; i < jml_produk; i++)
    {
        cout << "|" << right << setw(2) << i + 1 << "." << "|"
             << setw(28) << left << daftar_produk[i].nama << "| Rp."
             << setw(14) << fixed << setprecision(0) << daftar_produk[i].harga << "|" << endl;
    }
    cout << "=====================================================" << endl;

    int nomor;
    cout << "Masukkan nomor produk yang ingin diubah harganya: ";
    cin >> nomor;
    cin.ignore();

    if (nomor < 1 || nomor > jml_produk)
    {
        cout << "Nomor tidak valid!\n";
        return;
    }

    double harga_baru;
    cout << "Masukkan harga baru untuk " << daftar_produk[nomor - 1].nama << " : Rp.";
    cin >> harga_baru;
    cin.ignore();

    if (harga_baru < 0)
    {
        cout << "Harga tidak valid (tidak boleh minus)!\n";
        return;
    }

    daftar_produk[nomor - 1].harga = harga_baru;
    simpan_produk();
    cout << "Harga produk berhasil diperbarui!\n";
    cout << "Tekan enter untuk melanjutkan...";
    cin.get();
    system("cls");
}

//Function Hapus Produk
void hapus_produk()
{
    if (jml_produk == 0)
    {
        cout << "Tidak ada produk yang tersedia untuk dihapus.\n";
        return;
    }

    cout << "=== HAPUS PRODUK ===\n";
    for (int i = 0; i < jml_produk; i++)
    {
        cout << i + 1 << ". " << daftar_produk[i].nama << "\n";
    }

    int nomor;
    cout << "Masukkan nomor produk yang ingin dihapus: ";
    cin >> nomor;
    cin.ignore();

    if (nomor < 1 || nomor > jml_produk)
    {
        cout << "Nomor tidak valid!\n";
        return;
    }

    string nama_dihapus = daftar_produk[nomor - 1].nama;

    // Menggeser sisa elemen array ke kiri
    for (int i = nomor - 1; i < jml_produk - 1; i++)
    {
        daftar_produk[i] = daftar_produk[i + 1];
    }
    jml_produk--;

    simpan_produk();
    cout << "Produk '" << nama_dihapus << "' berhasil dihapus!\n";
    cout << "Tekan enter untuk melanjutkan...";
    cin.get();
    system("cls");
}

void sorting()
{
    for (int i = 1; i < jml_produk; i++)
    {
        produk key = daftar_produk[i];
        int j = i - 1;

        // bandingkan harga
        while (j >= 0 && daftar_produk[j].harga > key.harga)
        {
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
    cout << "==================================================== DAFTAR PRODUK ====================================================" << endl;
    cout << left << setw(5) << "| NO" << "|"
         << setw(28) << "    NAMA   " << "|"
         << setw(45) << "   \t\t\tSpesifikasi" << "|"
         << setw(15) << "  Harga" << "|"
         << setw(6)  << " Stok" << "|" << endl;
    cout << "=======================================================================================================================" << endl;

    for (int i = 0; i < jml_produk; i++)
    {
        cout << "| " << right << setw(2) << (i + 1) << "." << "|" 
             << setw(28) << left << daftar_produk[i].nama << "|"
             << setw(60) << daftar_produk[i].spesifikasi << "|" << "Rp."
             << setw(12) << fixed << setprecision(0) << daftar_produk[i].harga << "|"
             << setw(6) << daftar_produk[i].stok << "|" << endl;
    }

    cout << "=======================================================================================================================" << endl;
    char pilih_sorting;
    cout << "urutkan produk dari harga terendah? (y/t): ";
    cin >> pilih_sorting;
    cin.ignore();

    if (pilih_sorting == 'y' || pilih_sorting == 'Y')
    {
        system("cls");
        sorting();
    cout << "==================================================== DAFTAR PRODUK ====================================================" << endl;
    cout << left << setw(5) << "| NO" << "|"
         << setw(28) << "    NAMA   " << "|"
         << setw(45) << "   \t\t\tSpesifikasi" << "|"
         << setw(15) << "  Harga" << "|"
         << setw(6)  << " Stok" << "|" << endl;
    cout << "=======================================================================================================================" << endl;

    for (int i = 0; i < jml_produk; i++)
    {
        cout << "| " << right << setw(2) << (i + 1) << "." << "|" 
             << setw(28) << left << daftar_produk[i].nama << "|"
             << setw(60) << daftar_produk[i].spesifikasi << "|" << "Rp."
             << setw(12) << fixed << setprecision(0) << daftar_produk[i].harga << "|"
             << setw(6) << daftar_produk[i].stok << "|" << endl;
    }

    cout << "=======================================================================================================================" << endl;
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
bool tambah_akun()
{
    string user, pass   ;
    cout << "Masukan username : ";
    getline(cin, user);
    cout << "Masukkka password : ";
    getline(cin, pass);

    for(int i = 0; i < jml_akun; i++){
        if(username[i] == user) {
            cout << "User sudah ada, silahkan coba lagi!";
            cin.get();
            system("cls");
            return false;
        }
    }
    username[jml_akun] = user;
    password[jml_akun] = pass;
    jml_akun++;

    simpan_akun();
    system("cls");
    cout << "Selamat anda berhasil mendaftar, selamat berbelanja" << endl;
    cout << "Tekan enter untuk melanjutkan";
    cin.get();
    system("cls");
    return true;
}

// Function Menu Utama Pembeli
void menu_utama()
{
    cout << "========= MENU PEMBELI =========" << endl;
    cout << "|    1. Tampilkan Produk       |" << endl;
    cout << "|    2. Searching Produk       |" << endl;
    cout << "|    3. Lihat Keranjang        |" << endl;
    cout << "|    4. Riwayat Pembelian      |" << endl;
    cout << "|    5. Barang Sudah Sampai    |" << endl;
    cout << "|    6. Keluar                 |" << endl;
    cout << "===============================" << endl;
    cout << "Masukan pilihan anda : ";
    cin >> pilihan;
    cin.ignore();
    system("cls");
}

string toLower(string str) {
    for(int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

string getWaktu()
{
    time_t now = time(0);
    char *dt = ctime(&now);
    return string(dt);
}

void search_produk()
{
    string cari;
    bool ditemukan = false;
    int nomor = 1;

    cout << "Masukkan nama produk yang dicari : ";
    getline(cin, cari);
    cari = toLower(cari);

    int hasil_index[100];
    int jml_hasil = 0;

    cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "================================================== PRODUK DITEMUKAN ==================================================" << endl;
        cout << left << setw(4) << "| NO" << "|"
         << setw(28) << "    NAMA   " << "|"
         << setw(44) << "   \t\t\tSpesifikasi" << "|"
         << setw(15) << "  Harga" << "|"
         << setw(6)  << " Stok" << "|" << endl;
    cout << "======================================================================================================================" << endl;
    for (int i = 0; i < jml_produk; i++)
    {
        if (toLower(daftar_produk[i].nama).find(cari) != string::npos)
        {
            cout << "|" << right << setw(2) << nomor++ << "." << "|"
                 << setw(28) << left << daftar_produk[i].nama << "|"
                 << setw(60) << daftar_produk[i].spesifikasi << "|" << "Rp."
                 << setw(12) << fixed << setprecision(0) << daftar_produk[i].harga << "|"
                 << setw(6) << daftar_produk[i].stok << "|" << endl;

            hasil_index[jml_hasil++] = i;
            ditemukan = true;
        }
    }
    cout << "======================================================================================================================" << endl;

    if(!ditemukan){
        cout << "Produk tidak tersedia" << endl;
        return;
    }

    char pilihTambah;
    cout << "\nApakah Anda ingin menambahkan produk ke keranjang? (y/t): ";
    cin >> pilihTambah;
    cin.ignore();

    if(pilihTambah == 'y' || pilihTambah == 'Y')
    {
        int pilih_nomor, jumlah;

        cout << "Masukan Nomor Produk: ";
        cin >> pilih_nomor;
        cin.ignore();

        if(pilih_nomor < 1 || pilih_nomor > jml_hasil){
            cout << "Nomor tidak valid!" << endl;
            return;
        }

        cout << "Jumlah: ";
        cin >> jumlah;
        cin.ignore();

        int idx = hasil_index[pilih_nomor -1];
        if(jumlah > daftar_produk[idx].stok) {
            cout << "pembelian melebehi stok" << endl;
            return;
        }

        keranjang[jml_keranjang].namaProduk = daftar_produk[idx].nama;
        keranjang[jml_keranjang].jumlah = jumlah;
        keranjang[jml_keranjang].harga = daftar_produk[idx].harga;

        jml_keranjang++;

        cout << "Produk " << daftar_produk[idx].nama << "dengan jumlah " << jumlah << "sudah ditambahkan ke keranjang." << endl;

    }
    cout << "tekan enter untuk kembali";
    cin.get();
    system("cls");
}

void tambah_nota(string nama, int jumlah, double total)
{
    Nota *baru = new Nota;
    baru->namaProduk = nama;
    baru->jumlah = jumlah;
    baru->totalHarga = total;
    baru->waktu = getWaktu();
    baru->status = "Diproses";
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
    simpan_nota();
}

void enqueue(string nama, int jumlah)
{
    antrian[rear].namaProduk = nama;
    antrian[rear].jumlah = jumlah;
    rear++;

    cout << "Pesanan masuk ke antrian Proses\n";
}

void lihat_antrian()
{
    if (front == rear)
    {
        cout << "Tidak ada antrian pembelian\n";
        return;
    }

    cout << "===== ANTRIAN PEMBELIAN =====" << endl;
    for (int i = front; i < rear; i++)
    {
        cout << i + 1 << ". "
             << antrian[i].namaProduk
             << " x" << antrian[i].jumlah << endl;
    }
}

void dequeue()
{
    if (front == rear)
    {
        cout << "Antrian kosong!\n";
        return;
    }

    cout << "Pesanan" << antrian[front].namaProduk
         << " sedang dikirim\n";

    if (head != NULL)
    {
        tail->status = "Dikirim";
        simpan_nota();
    }

    front++;
}

void barang_sampai()
{
    if (head == NULL)
    {
        cout << "Belum ada pesanan\n";
        return;
    }

    tail->status = "Selesai";
    simpan_nota();
    cout << "Barang telah diterima!\n";
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
        cout << "======== NOTA PEMBELIAN ========" << endl;
        cout << "Produk  : " << temp->namaProduk << endl;
        cout << "Jumlah  : " << temp->jumlah << endl;
        cout << "Total   : Rp. " << temp->totalHarga << endl;
        cout << "Waktu   : " << temp->waktu;
        cout << "Status  : " << temp->status << endl;
        cout << "================================" << endl;

        temp = temp->next;
    }
}

//Function Menu Utama Lihat Keranjang
void menu_keranjang()
{
     cout << "========================== KERANJANG BELANJA ==========================" << endl;
     cout << left << setw(5) << "| NO" << "|"
         << setw(28) << "    NAMA   " << "|"
         << setw(15) << "    Jumlah" << "|"
         << setw(19) << "    Harga" << "|" << endl;
    cout << "=======================================================================" << endl;

    if (jml_keranjang == 0)
    {
        cout << "Maaf, keranjang masih kosong!\n";
    }
    else
    {
        double total = 0;
        for (int i = 0; i < jml_keranjang; i++)
        {
            double subtotal = keranjang[i].harga * keranjang[i].jumlah;

            cout << "| " << right << setw(2) << (i + 1) << "." << "|" 
                 << setw(28) << left << keranjang[i].namaProduk << "|\t " 
                 << setw(9) << keranjang[i].jumlah << "| Rp." 
                 << setw(15) << fixed << subtotal << "|" << endl;
                 
                 total += keranjang[i].harga * keranjang[i].jumlah;
        }
        cout << "=======================================================================" << endl;
        cout << setw(29) << "|\t\t\tTOTAL" << " = Rp." << fixed << setw(15) << total << "|" << endl;
        cout << "=======================================================================" << endl;
    }
    cout << "================================" << endl;
    cout << "|   1. Tambah Barang           |" << endl;
    cout << "|   2. Hapus Barang            |" << endl;
    cout << "|   3. Checkout                |" << endl;
    cout << "|   4. Kembali                 |" << endl;
    cout << "================================" << endl;
    cout << "Masukkan Pilihan : ";
    cin >> pilih;
    cin.ignore();
    system("cls");
}

void tambah_keranjang()
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
            keranjang[jml_keranjang].namaProduk = nama;
            keranjang[jml_keranjang].jumlah = jumlah;
            keranjang[jml_keranjang].harga = daftar_produk[i].harga;

            simpan_produk();

            jml_keranjang++;

            cout << "Produk berhasil ditambahkan ke dalam keranjang\n";
            return;
        }
    }

    cout << "Produk tidak ditemukan\n";
}

void hapus_keranjang()
{
    if (jml_keranjang == 0)
    {
        cout << "Keranjang kosong!\n";
        return;
    }

    int index;
    cout << "Masukkan nomor barang yang ingin dihapus: ";
    cin >> index;
    cin.ignore();

    if (index < 1 || index > jml_keranjang)
    {
        cout << "Nomor tidak valid!\n";
        return;
    }

    for (int i = 0; i < jml_produk; i++)
    {
        if (daftar_produk[i].nama == keranjang[index - 1].namaProduk)
        {
            daftar_produk[i].stok += keranjang[index - 1].jumlah;
            simpan_produk();
            break;
        }
    }

    for (int i = index - 1; i < jml_keranjang - 1; i++)
    {
        keranjang[i] = keranjang[i + 1];
    }

    jml_keranjang--;

    cout << "Barang berhasil dihapus!\n";
}

void Checkout()
{
    if (jml_keranjang == 0)
    {
        cout << "Keranjang kosong!\n";
        return;
    }

    for (int i = 0; i < jml_keranjang; i++)
    {
        for (int j = 0; j < jml_produk; j++)
    {
        if (daftar_produk[j].nama == keranjang[i].namaProduk)
        {
            daftar_produk[j].stok -= keranjang[i].jumlah;
        }
    }
        double total = keranjang[i].harga * keranjang[i].jumlah;

        tambah_nota(
            keranjang[i].namaProduk,
            keranjang[i].jumlah,
            total
        );

        enqueue(
            keranjang[i].namaProduk,
            keranjang[i].jumlah
        );
    }

    jml_keranjang = 0;
    cout << "Checkout berhasil!\n";
}

void tampil_keranjang()
{
    if (jml_keranjang == 0)
    {
        cout << "Keranjang ksong!\n";
        return;
    }

    for (int i = 0; i < jml_keranjang; i++)
    {
        cout << i+1 << ". "
             << keranjang[i].namaProduk
             << "x " << keranjang[i].jumlah
             << endl;
    }
}

int main()
{
    muat_akun();
    muat_produk();
    muat_nota();

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
    } while (pilihan_awal != '3');

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
            hapus_produk();
            break;
        case 3:
            edit_harga_produk();
            break;
        case 4:
            tampilkan_produk();
            break;
        case 5:
            lihat_antrian();
            goto menu_penjual;
            break;
        case 6:
            dequeue();
            break;
        case 7:
            cout << "============================****=============================" << endl;
            cout << "|              Terima Kasih Atas Kerjasamanya                |" << endl;
            cout << "============================****=============================";
            cin.get();
            system("cls");
            goto menu_awal;
        default:
            cout << "Pilihan tidak tersedia!\n";
            cin.get();
            goto menu_penjual;
            break;
        }
    } while (pilih != '7');

    // menu pembeli
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
                cout << "============================****=============================" << endl;
                cout << "|                      LOGIN BERHASIL!                      |" << endl;
                cout << "|                    SELAMAT BERBELANJA                     |" << endl;
                cout << "============================****=============================";
                cin.get();
                system("cls");
                goto menu_utama;
            }
            else
            {
                cout << "============================****=============================" << endl;
                cout << "|                       LOGIN GAGAL!                        |" << endl;
                cout << "|                    SILAHKAN COBA LAGI                     |" << endl;
                cout << "============================****=============================";
                cin.get();
                system("cls");
                goto menu_login;
            }
            break;
        }
        case '2':
            if(tambah_akun())
                goto menu_utama;
            else
                goto menu_login;
            break;
        case '3':
            system("cls");
            goto menu_awal;
            break;

        default:
            cout << "============================****=============================" << endl;
            cout << "|      pilihan salah, tekan enter untuk coba lagi           |" << endl;
            cout << "============================****=============================";
            cin.get();
            system("cls");
            goto menu_login;
            break;
        }
    } while (pilihan_login != '3');

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
        {
            int pilihkeranjang;
            do 
            {
                menu_keranjang();
                pilihkeranjang = pilih;

                switch (pilihkeranjang)
                {
                    case 1:
                    tambah_keranjang();
                    break;

                    case 2:
                    hapus_keranjang();
                    break;

                    case 3:
                    Checkout();
                    break;
                }
            }while (pilihkeranjang !=4);

            break;
        }
        case 4:
            tampil_riwayat();
            break;
        case 5:
            barang_sampai();
            break;
        case 6:
            cout << "============================****=============================" << endl;
            cout << "|               TERIMA KASIH SUDAH BERBELANJA               |" << endl;
            cout << "============================****=============================";
            cin.get();
            system("cls");
            goto menu_awal;
            break;

        default:
            return 0;
            break;
        }

    } while (pilihan != 6);
}
