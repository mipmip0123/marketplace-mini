#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
using namespace std;


// ========================
// STRUCT & GLOBAL DATA
// ========================
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

struct kode
{
    string kode_penjual;
};

struct stack_keranjang {
    Keranjang data[100];
    int top;
} sKeranjang;

const int MAX_PRODUK = 100;
const int MAX_AKUN = 100;

produk daftar_produk[MAX_PRODUK];
Keranjang keranjang[100];
Queue antrian[100];
string username[MAX_AKUN], password[MAX_AKUN];
kode Codes[1] = {"11223344"};

int jml_produk = 0, jml_keranjang = 0, jml_akun = 0;
int front = 0, rear = 0;
int pilihan = 0, pilihan_awal = 0, pilih = 0;
char pilihan_login;
bool login_kode;
string kode_kode;

Nota *head = NULL;
Nota *tail = NULL;


// ==========================
//    UTILITY FUNCTION
// ==========================
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
    string waktu(dt);

    //hapus newline bawaan ctime
    if(!waktu.empty() && waktu[waktu.length() - 1] == '\n') {
        waktu.erase(waktu.length() - 1);
    }
    return waktu;
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

void initstack_keranjang() {
    sKeranjang.top = -1;
}

void push_keranjang(Keranjang k) {
    if(sKeranjang.top < 99) {
        sKeranjang.data[++sKeranjang.top] = k;
    }
}

Keranjang pop_keranjang(int index_pop) {
    if(index_pop < 0 || index_pop > sKeranjang.top) {
        cout << "Nomor tidak valid" << endl;
        return Keranjang();
    }
    Keranjang kembali = sKeranjang.data[index_pop];

    for(int i = index_pop; i < sKeranjang.top; i++) {
        sKeranjang.data[i] = sKeranjang.data[i + 1];
    }
    sKeranjang.top--;
    return kembali;
}

// ==========================
//   DATABASE 
// ==========================
void simpan_produk()
{
    ofstream file("database_produk.txt");
    if (!file)
    {
        cout << "=======================================" << endl;
        cout << "| Gagal Membuka file untuk menyimpan! | " << endl;
        cout << "=======================================";
        cin.get();
        system("cls");
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
}

int muat_produk()
{
    ifstream file("database_produk.txt");
    if (!file)
    {
        cout << "==================================================" << endl;
        cout << "| Database belum ada, mulai dengan data default. |" << endl;
        cout << "==================================================";
        cin.get();
        system("cls");
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

void simpan_nota()
{
    ofstream file("database_nota.txt");
    if(!file)
    {
        cout << "=======================================" << endl;
        cout << "| Gagal Membuka file untuk menyimpan! |" << endl;
        cout << "=======================================";
        cin.get();
        system("cls");
        return;
    }
    Nota *temp = head;

    while(temp != NULL)
    {
        file << temp->namaProduk << "|";
        file << temp->jumlah << "|";
        file << fixed << setprecision(0) << temp->totalHarga << "|";
        file << temp->waktu << "|";
        file << temp->status << endl;

        temp = temp->next;
    }

    file.close();
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

void simpan_akun()
{
    ofstream file("database_akun.txt");
    if (!file)
    {
        cout << "=======================================" << endl;
        cout << "| Gagal Membuka file untuk menyimpan! |" << endl;
        cout << "=======================================";
        cin.get();
        system("cls");
        return;
    }
    for (int i = 0; i < jml_akun; i++)
    {
        file << username[i] << "|" << password[i] << endl;
    }
    file.close();
    cout << "===============================" << endl;
    cout << "| Data akun berhasil disimpan |" << endl;
    cout << "===============================";
    cin.get();
    system("cls");
}

int muat_akun()
{
    ifstream file("database_akun.txt");
    if (!file)
    {
        cout << "==================================================" << endl;
        cout << "| Database belum ada, mulai dengan data default. |" << endl;
        cout << "==================================================";
        cin.get();
        system("cls");
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


// ==========================
//        CRUD
// ==========================
void tambah_produk()
{
    int idx = -1;

    new_produk.nama = "";
    new_produk.spesifikasi = "";
    new_produk.harga = 0;
    new_produk.stok = 0;

    if (jml_produk >= MAX_PRODUK)
    {
        cout << "===================================================" << endl;
        cout << "| Database penuh tidak dapat menambah produk lagi |" << endl;
        cout << "===================================================";
        cin.get();
        system("cls");
        return;
    }

    // input sekali di awal
    cout << "Masukan nama produk : ";
    getline(cin, new_produk.nama);

    cout << "Masukan spesifikasi produk : ";
    getline(cin, new_produk.spesifikasi);

    // cek nama + spesifikasi
    for (int i = 0; i < jml_produk; i++)
    {
        if (daftar_produk[i].nama == new_produk.nama &&
            daftar_produk[i].spesifikasi == new_produk.spesifikasi)
        {
            idx = i;

            char pilih_stok;
            int tambah_stok;

            cout << "============================================" << endl;
            cout << "|          Produk Sudah Tersedia!          |" << endl;
            cout << "|  Apakah ingin menambahkan stok?          |" << endl;
            cout << "============================================" << endl;
            cout << "Apakah ingin menambahkan stok produk? (y/t): ";
            cin >> pilih_stok;
            cin.ignore();

            if (pilih_stok == 'y' || pilih_stok == 'Y')
            {
                cout << "Masukkan jumlah stok yang ingin ditambahkan: ";
                cin >> tambah_stok;
                cin.ignore();

                daftar_produk[idx].stok += tambah_stok;
                simpan_produk();

                cout << "========================================" << endl;
                cout << "| Stok berhasil ditambahkan!           |" << endl;
                cout << "| Total stok sekarang: "
                     << daftar_produk[idx].stok << endl;
                cout << "========================================" << endl;
            }
            else
            {
                cout << "===============================" << endl;
                cout << "| Penambahan stok dibatalkan! |" << endl;
                cout << "===============================" << endl;
            }

            cout << "Tekan enter untuk kembali...";
            cin.get();
            system("cls");
            return;
        }
    }

    // kalau belum ada, lanjut tambah produk baru
    cout << "Masukan harga produk : Rp.";
    cin >> new_produk.harga;
    cin.ignore();

    cout << "Masukan jumlah stok : ";
    cin >> new_produk.stok;
    cin.ignore();

    daftar_produk[jml_produk].nama = new_produk.nama;
    daftar_produk[jml_produk].spesifikasi = new_produk.spesifikasi;
    daftar_produk[jml_produk].harga = new_produk.harga;
    daftar_produk[jml_produk].stok = new_produk.stok;

    jml_produk++;
    simpan_produk();

    cout << "===================================" << endl;
    cout << "| Produk berhasil ditambahkan!    |" << endl;
    cout << "===================================" << endl;
    cout << "Tekan enter untuk kembali...";
    cin.get();
    system("cls");
}

void edit_stok() {
    if(jml_produk == 0) {
        cout << "=============================================" << endl;
        cout << "|      T I D A K  A D A  P R O D U K        |" << endl;
        cout << "=============================================";
        cin.get();
        system("cls");
        return; 
    }
    cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "=======================================================================================================================" << endl;
    cout << "|                                              D A F T A R  P R O D U K                                               |" << endl;
    cout << "=======================================================================================================================" << endl;
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

    int nomor_produk;
    cout << "Masukan nomor produk yang ingin diubah stoknya : ";
    cin >> nomor_produk;
    cin.ignore();
        system("cls");

    if(nomor_produk < 1 || nomor_produk > jml_produk) {
        cout << "=====================" << endl; 
        cout << "| Nomor Tidak Valid |" << endl;
        cout << "=====================";
        cin.get();
        system("cls");
        return;
    }

    int stok_baru;
    cout << "===========================================" << endl;
    cout << "|            U B A H  S T O K             |" << endl;
    cout << "===========================================" << endl;
    cout << "Masuka Stok baru untuk  " << daftar_produk[nomor_produk - 1].nama << " : ";
    cin >> stok_baru;
    cin.ignore();
    system("cls");

    if(stok_baru < 0) {
        cout << "==========================================" << endl;
        cout << "|            Stok Tidak Valid            |" << endl;
        cout << "|        Angka Tidak Boleh Minus         |" << endl;
        cout << "==========================================";
        cin.get();
        system("cls");
        return;
    }

    daftar_produk[nomor_produk - 1].stok = stok_baru;
    simpan_produk();

    cout << "===================================================" << endl;
    cout << "|  S T O K  B E R H A S I L  D I P E R B A R U I  |" << endl;
    cout << "|         Tekan Enter Untuk Melanjutkan           |" << endl;
    cout << "===================================================";
    cin.get();
    system("cls");
    
}

void edit_harga_produk()
{
    if (jml_produk == 0)
    {
        cout << "================================================" << endl;
        cout << "| Tidak ada produk yang tersedia untuk diedit. |" << endl;
        cout << "================================================";
        cin.get();
        system("cls");
        return;
    }

    cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "=====================================================" << endl;
    cout << "|                 EDIT HARGA PRODUK                 |" << endl;
    cout << "=====================================================" << endl;
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
        cout << "======================" << endl;
        cout << "| Nomor tidak valid! |" << endl;
        cout << "======================";
        cin.get();
        system("cls");
        return;
    }

    double harga_baru;
    cout << "Masukkan harga baru untuk " << daftar_produk[nomor - 1].nama << " : Rp.";
    cin >> harga_baru;
    cin.ignore();
    system("cls");

    if (harga_baru < 0)
    {
        cout << "==========================================" << endl;
        cout << "| Harga tidak valid (tidak boleh minus)! |" << endl;
        cout << "==========================================";
        cin.get();
        system("cls");
        return;
    }

    daftar_produk[nomor - 1].harga = harga_baru;
    simpan_produk();
    cout << "=====================================" << endl;
    cout << "| Harga produk berhasil diperbarui! |" << endl;
    cout << "|  Tekan enter untuk melanjutkan... |" << endl;
    cout << "=====================================";
    cin.get();
    system("cls");
}

void hapus_produk()
{
    if (jml_produk == 0)
    {
        cout << "=================================================" << endl;
        cout << "| Tidak ada produk yang tersedia untuk dihapus. |" << endl;
        cout << "=================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "==================================================== HAPUS PRODUK =====================================================" << endl;
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

    int nomor;
    cout << "Masukkan nomor produk yang ingin dihapus: ";
    cin >> nomor;
    cin.ignore();

    if (nomor < 1 || nomor > jml_produk)
    {
        cout << "========================" << endl;
        cout << "|  Nomor tidak valid!  |" << endl;
        cout << "========================" << endl;
        cin.get();
        system("cls");
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
    cout << "=====================================================" << endl;
    cout << "| Produk " << nama_dihapus << " berhasil dihapus!   |" << endl;
    cout << "|        Tekan enter untuk melanjutkan...           |" << endl;
    cout << "====================================================="; 
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
    cout << "=====================================================" << endl;
    cout << "| Daftar produk sudah diurutkan dari harga terendah |" << endl;
    cout << "=====================================================";
    cin.get();
    system("cls");
}

void tampilkan_produk(bool pembeli)
{
    if (jml_produk == 0)
    {
        cout << "=====================================" << endl;
        cout << "|  Tidak ada produk yang tersedia.  |" << endl;
        cout << "=====================================";
        cin.get();
        system("cls");
        return;
    }

    cout.imbue(locale(cout.getloc(), new format_rupiah));

    cout << "=======================================================================================================================" << endl;
    cout << "|                                              D A F T A R  P R O D U K                                               |" << endl;
    cout << "=======================================================================================================================" << endl;
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
    cout << "Urutkan produk dari harga terendah? (y/t): ";
    cin >> pilih_sorting;
    cin.ignore();

    if (pilih_sorting == 'y' || pilih_sorting == 'Y')
    {
        system("cls");
        sorting();

        cout << "=======================================================================================================================" << endl;
        cout << "|                                              D A F T A R  P R O D U K                                               |" << endl;
        cout << "=======================================================================================================================" << endl;
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

    // kalau penjual, selesai sampai sini
    if (!pembeli)
    {
        cout << "Tekan enter untuk kembali ke menu...";
        cin.get();
        system("cls");
        return;
    }

    // kalau pembeli, bisa tambah ke keranjang
    char pilihTambah;
    cout << "Apakah anda ingin menambahkan produk ke keranjang? (y/t): ";
    cin >> pilihTambah;
    cin.ignore();

    if (pilihTambah == 'y' || pilihTambah == 'Y')
    {
        int pilih_nomor, jumlah;

        cout << "Masukkan nomor produk: ";
        cin >> pilih_nomor;
        cin.ignore();

        if (pilih_nomor < 1 || pilih_nomor > jml_produk)
        {
            cout << "======================" << endl;
            cout << "| Nomor tidak valid! |" << endl;
            cout << "======================" << endl;
            cout << "Tekan enter untuk kembali...";
            cin.get();
            system("cls");
            return;
        }

        cout << "Masukkan jumlah: ";
        cin >> jumlah;
        cin.ignore();

        if (jumlah > daftar_produk[pilih_nomor - 1].stok)
        {
            cout << "=========================" << endl;
            cout << "| Stok tidak mencukupi! |" << endl;
            cout << "=========================" << endl;
            cout << "Tekan enter untuk kembali...";
            cin.get();
            system("cls");
            return;
        }

        keranjang[jml_keranjang].namaProduk = daftar_produk[pilih_nomor - 1].nama;
        keranjang[jml_keranjang].jumlah = jumlah;
        keranjang[jml_keranjang].harga = daftar_produk[pilih_nomor - 1].harga;

        jml_keranjang++;

        cout << "==============================================" << endl;
        cout << "| Produk berhasil ditambahkan ke keranjang!  |" << endl;
        cout << "==============================================" << endl;
    }
    else
    {
        cout << "==================================" << endl;
        cout << "| Tidak jadi menambahkan produk |" << endl;
        cout << "==================================" << endl;
    }

    cout << "Tekan enter untuk kembali ke menu...";
    cin.get();
    system("cls");
}


// ===========================
//     LOGIN
// ===========================
bool login(string user, string pass)
{
    for (int i = 0; i < jml_akun; i++)
    {
        if (username[i] == user && password[i] == pass)
            return true;
    }
    return false;
}

bool tambah_akun()
{
    cout << "====================================" << endl;
    cout << "|          R E G I S T E R         |" << endl;
    cout << "====================================" << endl;
    string user, pass   ;
    cout << "Masukan username : ";
    getline(cin, user);
    cout << "Masukkka password : ";
    getline(cin, pass);

    system("cls");

    for(int i = 0; i < jml_akun; i++){
        if(username[i] == user) {
            cout << "=======================================" << endl;
            cout << "| User sudah ada, silahkan coba lagi! |" << endl;
            cout << "=======================================";
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
    cout << "=======================================================" << endl;
    cout << "| Selamat anda berhasil mendaftar, selamat berbelanja |" << endl;
    cout << "|            Tekan enter untuk melanjutkan            |" << endl;
    cout << "=======================================================";
    cin.get();
    system("cls");
    return true;
}

// ===========================
//    MENU
// ===========================
void menu_awal()
{
    cout << "=========================" << endl;
    cout << "|       MENU AWAL       |" << endl;
    cout << "=========================" << endl;
    cout << "|  1.Sebagai Penjual    |" << endl;
    cout << "|  2.Sebagai Pembeli    |" << endl;
    cout << "|  3.keluar             |" << endl;
    cout << "==========================" << endl;
    cout << "Masukan pilihan anda : ";
    cin >> pilihan_awal;
    cin.ignore();
    system("cls");
}

void menu_penjual()
{
    cout << "======= MENU ADMIN =======" << endl;
    cout << "|  1. Tambah Barang      |" << endl;
    cout << "|  2. Hapus Barang       |" << endl;
    cout << "|  3. Edit Harga Barang  |" << endl;
    cout << "|  4. Lihat data barang  |" << endl;
    cout << "|  5. Lihat Antrian      |" << endl;
    cout << "|  6. Proses Pengiriman  |" << endl;
    cout << "|  7. Edit Stok          |" << endl;
    cout << "|  8. Keluar             |" << endl;
    cout << "==========================" << endl;
    cout << "Masukkan pilihan anda : ";
    cin >> pilih;
    cin.ignore();
    system("cls");
}

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

void menu_utama()
{
    cout << "================================" << endl;
    cout << "|     M E N U  P E M B E L I   |" << endl;
    cout << "================================" << endl;
    cout << "|    1. Tampilkan Produk       |" << endl;
    cout << "|    2. Searching Produk       |" << endl;
    cout << "|    3. Lihat Keranjang        |" << endl;
    cout << "|    4. Riwayat Pembelian      |" << endl;
    cout << "|    5. Barang Sudah Sampai    |" << endl;
    cout << "|    6. Keluar                 |" << endl;
    cout << "================================" << endl;
    cout << "Masukan pilihan anda : ";
    cin >> pilihan;
    cin.ignore();
    system("cls");
}

// ===========================
//   PEMBELI FUNCTION
// ===========================
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
    cout << "======================================================================================================================" << endl;
    cout << "|                                           P R O D U K  D I T E M U K A N                                           |" << endl;
    cout << "======================================================================================================================" << endl;
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
        cout << "=========================" << endl;
        cout << "| Produk tidak tersedia |" << endl;
        cout << "=========================";
        cin.get();
        system("cls");
        return;
    }

    char pilihTambah;
    cout << "\nApakah Anda ingin menambahkan produk ke keranjang? (y/t): ";
    cin >> pilihTambah;
    cin.ignore();

    if(pilihTambah == 't' || pilihTambah == 'T'){
        system("cls");
        return;
    }

    if(pilihTambah == 'y' || pilihTambah == 'Y')
    {
        int pilih_nomor, jumlah;

        cout << "Masukan Nomor Produk: ";
        cin >> pilih_nomor;
        cin.ignore();

        if(pilih_nomor < 1 || pilih_nomor > jml_hasil){
            cout << "======================" << endl;
            cout << "| Nomor tidak valid! |" << endl;
            cout << "======================";
            cin.get();
            system("cls");
            return;
        }

        cout << "Jumlah: ";
        cin >> jumlah;
        cin.ignore();

        int idx = hasil_index[pilih_nomor -1];
        if(jumlah > daftar_produk[idx].stok) {
            cout << "=========================" << endl;
            cout << "|  Stok tidak tersedia  |" << endl;
            cout << "=========================";
            cin.get();
            system("cls");
            return;
        }

        keranjang[jml_keranjang].namaProduk = daftar_produk[idx].nama;
        keranjang[jml_keranjang].jumlah = jumlah;
        keranjang[jml_keranjang].harga = daftar_produk[idx].harga;

        jml_keranjang++;

        system("cls");
        cout << "=================================================================================" << endl;
        cout << "|" << left << setw(30) << "Produk"
             << "|" << setw(10) << "Jumlah"
             << "|" << setw(37) << "Status" << "|" << endl;
        cout << "=================================================================================" << endl;
        cout << "|" << setw(30) << daftar_produk[idx].nama
             << "|" << setw(10) << jumlah
             << "|" << setw(37) << "Sudah Ditambahkan ke keranjang" << "|" << endl;
        cout << "=================================================================================";
        cin.get();
        system("cls");
    }
    char lanjut;
    cout << "==============================================" << endl;
    cout << "| apakah ingin lanjut searching produk (y/t) |" << endl;
    cout << "==============================================" << endl;
    cout << "               Masukan Jawaban : ";
    cin >> lanjut;
    cin.ignore();
    
    if(lanjut == 'y' || lanjut == 'Y') {
        system("cls");
        search_produk();
    }
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

    cout << "===================================" << endl;
    cout << "| Pesanan masuk ke antrian Proses |" << endl;
    cout << "===================================";
    cin.get();
    system("cls");
}

void dequeue()
{
    Nota *temp = head;
    bool ada = false;

    cout << "==============================================================" << endl;
    cout << "|                   P R O S E S  P E N G I R I M A N        |" << endl;
    cout << "==============================================================" << endl;

    while (temp != NULL)
    {
        if (temp->status == "Dikirim")
        {
            cout << "| Produk : " << left << setw(40) << temp->namaProduk
                 << "| Jumlah : " << setw(5) << temp->jumlah << "|" << endl;
            cout << "==============================================================" << endl;

            ada = true;
        }
        temp = temp->next;
    }

    if (!ada)
    {
        cout << "| Tidak ada produk yang sedang dikirim                      |" << endl;
        cout << "==============================================================" << endl;
    }

    cout << "Tekan enter untuk kembali...";
    cin.get();
    system("cls");
}

void lihat_antrian()
{
    Nota *temp = head;
    Nota *dataDiproses[100];
    int jumlah = 0;

    while (temp != NULL)
    {
        if (temp->status == "Diproses")
        {
            dataDiproses[jumlah] = temp;
            jumlah++;
        }
        temp = temp->next;
    }

    if (jumlah == 0)
    {
        cout << "======================" << endl;
        cout << "| Tidak ada antrian! |" << endl;
        cout << "======================";
        cin.get();
        system("cls");
        return;
    }

    cout << "===================================================" << endl;
    cout << "|                   A N T R I A N                 |" << endl;
    cout << "===================================================" << endl;

    for (int i = 0; i < jumlah; i++)
    {
        cout << i + 1 << ". "
             << dataDiproses[i]->namaProduk
             << " | Jumlah: " << dataDiproses[i]->jumlah << endl;
    }

    char pilih;
    cout << "\nApakah ada produk yang ingin diproses? (y/t): ";
    cin >> pilih;
    cin.ignore();

    if (pilih == 'y' || pilih == 'Y')
    {
        int nomor;
        cout << "Produk nomor berapa yang ingin diproses? ";
        cin >> nomor;
        cin.ignore();

        if (nomor < 1 || nomor > jumlah)
        {
            cout << "Nomor tidak valid!" << endl;
            cin.get();
            system("cls");
            return;
        }

        dataDiproses[nomor - 1]->status = "Dikirim";
        simpan_nota();

        cout << "Produk sedang diproses pengiriman!" << endl;
        cin.get();
        system("cls");

        lihat_antrian();
    }
    else
    {
        system("cls");
    }
}

void barang_sampai()
{
    Nota *temp = head;
    Nota *dataDikirim[100];
    int jumlah = 0;

    while (temp != NULL)
    {
        if (temp->status == "Dikirim")
        {
            dataDikirim[jumlah] = temp;
            jumlah++;
        }
        temp = temp->next;
    }

    if (jumlah == 0)
    {
        cout << "==========================================" << endl;
        cout << "| Tidak ada barang yang sedang dikirim! |" << endl;
        cout << "==========================================";
        cin.get();
        system("cls");
        return;
    }

    cout << "===================================================" << endl;
    cout << "|              B A R A N G  D I K I R I M        |" << endl;
    cout << "===================================================" << endl;

    for (int i = 0; i < jumlah; i++)
    {
        cout << i + 1 << ". "
             << dataDikirim[i]->namaProduk
             << " | Jumlah: " << dataDikirim[i]->jumlah << endl;
    }

    int nomor;
    char konfirmasi;

    cout << "\nPilih nomor barang yang sudah diterima: ";
    cin >> nomor;
    cin.ignore();

    if (nomor < 1 || nomor > jumlah)
    {
        cout << "======================" << endl;
        cout << "| Nomor tidak valid! |" << endl;
        cout << "======================";
        cin.get();
        system("cls");
        return;
    }

    cout << "Apakah produk sudah diterima? (y/t): ";
    cin >> konfirmasi;
    cin.ignore();

    if (konfirmasi == 'y' || konfirmasi == 'Y')
    {
        dataDikirim[nomor - 1]->status = "Selesai";
        simpan_nota();

        cout << "==========================================" << endl;
        cout << "| Barang berhasil dikonfirmasi diterima! |" << endl;
        cout << "==========================================";
    }
    else
    {
        cout << "==============================" << endl;
        cout << "| Konfirmasi dibatalkan!     |" << endl;
        cout << "==============================";
    }

    cin.get();
    system("cls");
}

void tampil_riwayat()
{
    Nota *temp = head;

    if (temp == NULL)
    {
        cout << "===============================" << endl;
        cout << "| Belum ada riwayat pembelian |" << endl;
        cout << "===============================";
        cin.get();
        system("cls");
        return;
    }

    cout.imbue(locale(cout.getloc(), new format_rupiah));
    while (temp !=NULL)
    {
        cout << "==================================" << endl;
        cout << "|   N O T A  P E M B E L I A N   |" << endl;
        cout << "==================================" << endl;
        cout << "Produk  : " << temp->namaProduk << endl;
        cout << "Jumlah  : " << temp->jumlah << endl;
        cout << "Total   : Rp. " << fixed << setprecision(0) << temp->totalHarga << endl;
        cout << "Waktu   : " << temp->waktu << endl;
        cout << "Status  : " << temp->status << endl;
        cout << "==================================" << endl;

        temp = temp->next;
    }
}

void tampil_keranjang()
{
    if (jml_keranjang == 0)
    {
        cout << "=================================" << endl;
        cout << "| Maaf, keranjang masih kosong! |" << endl;
        cout << "=================================";
        cin.get();
        system("cls");
    }
    else
    {
         cout << "=======================================================================" << endl;
         cout << "|                  K E R A N J A N G  B E L A N J A                   |" << endl;
         cout << "=======================================================================" << endl;
         cout << left << setw(5) << "| NO" << "|"
         << setw(28) << "    NAMA   " << "|"
         << setw(15) << "    Jumlah" << "|"
         << setw(19) << "    Harga" << "|" << endl;
         cout << "=======================================================================" << endl;
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
}

void menu_keranjang()
{
    tampil_keranjang();
    cout << "\t\t================================" << endl;
    cout << "\t\t|   1. Tambah Jumlah Barang    |" << endl;
    cout << "\t\t|   2. Hapus Barang            |" << endl;
    cout << "\t\t|   3. Checkout                |" << endl;
    cout << "\t\t|   4. Undo Barang dihapus     |" << endl;
    cout << "\t\t|   5. Kembali                 |" << endl;
    cout << "\t\t================================" << endl;
    cout << "\t\t\tMasukkan Pilihan : ";
    cin >> pilih;
    cin.ignore();
    system("cls");
}

void hapus_keranjang()
{
    if (jml_keranjang == 0)
    {
        cout << "=====================" << endl;
        cout << "| Keranjang kosong! |" << endl;
        cout << "=====================";
        cin.get();
        system("cls");
        return;
    }

    tampil_keranjang();
    int index;
    cout << "Masukkan nomor barang yang ingin dihapus: ";
    cin >> index;
    cin.ignore();
    system("cls");

    if (index < 1 || index > jml_keranjang)
    {
        cout << "======================" << endl;
        cout << "| Nomor tidak valid! |" << endl;
        cout << "======================";
        cin.get();
        system("cls");
        return;
    }

    push_keranjang(keranjang[index - 1]);

    for (int i = 0; i < jml_produk; i++)
    {
        if (daftar_produk[i].nama == keranjang[index - 1].namaProduk)
        {
            daftar_produk[i].stok += keranjang[index - 1].jumlah;
            break;
        }
    }

    for (int i = index - 1; i < jml_keranjang - 1; i++)
    {
        keranjang[i] = keranjang[i + 1];
    }

    jml_keranjang--;
    cout << "============================" << endl;
    cout << "| Barang berhasil dihapus! |" << endl;
    cout << "============================";
    cin.get();
    system("cls");
}

void undo_hapus_keranjang() {
    if(sKeranjang.top < 0) {
        cout << "======================================" << endl;
        cout << "| Tidak ada barang yang bisa di-undo |" << endl;
        cout << "======================================";
        cin.get();
        system("cls");
        return;
    }

         cout << "=======================================================================" << endl;
         cout << "|                      B A R A N G  D I H A P U S                     |" << endl;
         cout << "=======================================================================" << endl;
         cout << left << setw(5) << "| NO" << "|"
         << setw(28) << "    NAMA   " << "|"
         << setw(15) << "    Jumlah" << "|"
         << setw(19) << "    Harga" << "|" << endl;
         cout << "=======================================================================" << endl;
         for(int i = 0; i <= sKeranjang.top; i++) {
                cout << "| " << right << setw(2) << (i + 1) << "." << "|" 
                 << setw(28) << left << sKeranjang.data[i].namaProduk << "|\t " 
                 << setw(9) << sKeranjang.data[i].jumlah << "| Rp." 
                 << setw(15) << fixed << setprecision(0) << sKeranjang.data[i].harga << "|" << endl;    
         }
         cout << "=======================================================================" << endl;
         
         int pilih_undo;
         cout << "Pilih nomor barang untuk dikembalikan : ";
         cin >> pilih_undo;
         cin.ignore();

         if (pilih_undo < 1 || pilih_undo > sKeranjang.top+1) {
            cout << "=====================" << endl;
            cout << "| Nomor tidak valid |" << endl;
            cout << "=====================";
            cin.get();
            system("cls");
            return;
         }

         Keranjang kembali = pop_keranjang(pilih_undo - 1);
         keranjang[jml_keranjang++] = kembali;
         cout << "=============================" << endl;
         cout << "| Barang Sudah Dikembalikan |" << endl;
         cout << "=============================";
         cin.get();
         system("cls");
}

void tambah_keranjang()
{
    if (jml_keranjang == 0) {
        cout << "=====================" << endl;
        cout << "| keranjang kosong! |" << endl;
        cout << "=====================";
        cin.get();
        system("cls");
        return;
    }

    tampil_keranjang();
    int index , jumlah;
    cout << "Masukan nomor produk di keranjang : ";
    cin >> index;
    cin.ignore();

    if(index < 1 || index > jml_keranjang) {
        cout << "======================" << endl;
        cout << "| Nomor tidak Valid! |" << endl;
        cout << "======================";
        cin.get();
        system("cls");
        return;
    }

    cout << "Masukan Jumlah tambahan : ";
    cin >> jumlah;
    cin.ignore();

    keranjang[index - 1].jumlah += jumlah;
    cout << "================================" << endl;
    cout << "| Produk berhasil ditambahkkan |" << endl;
    cout << "================================";
    cin.get();
    system("cls");
}

void Checkout()
{
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
    
    simpan_produk();
    jml_keranjang = 0;
    cout << "======================" << endl;
    cout << "| Checkout berhasil! |" << endl;
    cout << "======================";
    cin.get();
    system("cls");
    return;
}


int main()
{
    muat_akun();
    muat_produk();
    muat_nota();
    initstack_keranjang();

// menu awal
menu_awal:
    do
    {
        menu_awal();
        switch (pilihan_awal)
        {
        case 1:
        menu_login_penjual:
            cout << "================================" << endl;
            cout << "| V E R I F I K A S I  K O D E |" << endl;
            cout << "================================" << endl;
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
            tampilkan_produk(false);
            break;
        case 5:
            lihat_antrian();
            break;
        case 6:
            dequeue();
            break;
        case 7:
            edit_stok();
            break;
        case 8:
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
            cout << "==================================" << endl;
            cout << "|   S I L A H K A N  L O G I N   |" << endl;
            cout << "==================================" << endl;
            string user, pass;
            cout << "Masukkan username : ";
            getline(cin, user);
            cout << "Masukkan Password :";
            getline(cin, pass);

            system("cls");

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
            tampilkan_produk(true);
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

                    case 4:
                    undo_hapus_keranjang();
                    break;
                }
            }while (pilihkeranjang !=5);

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
