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
}   new_produk;

struct Nota
{
    string username;
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

int indexLogin = -1;
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

string format_Tengah_teks(string pesan, int lebar_total) {
    int sisa_spasi = lebar_total - pesan.length();
    if (sisa_spasi < 0) sisa_spasi = 0;

    int spasi_kiri = sisa_spasi / 2;
    int spasi_kanan = sisa_spasi - spasi_kiri;

    string kiri = string(spasi_kiri, ' ');
    string kanan = string(spasi_kanan, ' ');

    return kiri + pesan + kanan;
}

void sorting()
{
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("DAFTAR PRODUK SUDAH DIURUTKAN DARI HARGA TERRENDAH!", 117) << "|" << endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
    produk temp[MAX_PRODUK];
    for(int i = 0; i < jml_produk; i++) temp[i] = daftar_produk[i];

    for(int i = 1; i < jml_produk; i++) {
        produk key = temp[i];
        int j = i - 1;
        while(j >= 0 && temp[j].harga > key.harga) {
            temp[j + 1] = temp[j];
            j--;
        }
        temp[j + 1] = key;
    }
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("D A F T A R  P R O D U K (TERURUT)",117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    for (int i = 0; i < jml_produk; i++) {
        cout << "| " << right << setw(2) << (i + 1) << "." << "|"
             << setw(28) << left << temp[i].nama << "|"
             << setw(60) << temp[i].spesifikasi << "|" << "Rp."
             << setw(12) << fixed << setprecision(0) << temp[i].harga << "|"
             << setw(6) << temp[i].stok << "|" << endl;
    }
    cout << "=======================================================================================================================" << endl;
}

// ==========================
//   DATABASE 
// ==========================
void simpan_produk()
{
    ofstream file("database_produk.txt");
    if (!file)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("GAGAL MEMBUKA FILE UNTUK MENYIMPAN!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
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
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("DATABASE BELUM ADA, MULAI DENGAN DATA DEFAULT", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
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
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("GAGAL MEMBUKA FILE UNTUK MENYIMPAN!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }
    Nota *temp = head;

    while(temp != NULL)
    {
        file << temp->username << "|";
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

    string usernameFile, nama, jumlahStr, totalStr, waktu, status;

    while (
        getline(file, usernameFile, '|') &&
        getline(file, nama, '|') &&
        getline(file, jumlahStr, '|') &&
        getline(file, totalStr, '|') &&
        getline(file, waktu, '|') &&
        getline(file, status)
    )
    {
        Nota *baru = new Nota;

        baru->username = usernameFile;
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
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("GAGAL MEMBUKA FILE UNTUK MENYIMPAN!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }
    for (int i = 0; i < jml_akun; i++)
    {
        file << username[i] << "|" << password[i] << endl;
    }
    file.close();
}

int muat_akun()
{
    ifstream file("database_akun.txt");
    if (!file)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("DATABASE BELUM ADA, MULAI DENGAN DATA DEFAULT", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
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
//        CRUD  DAN  MENU PENJUAL
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
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("DATABASE PENUH TIDAK DAPAT MENAMBAHKAN PRODUK LAGI", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    // input sekali di awal
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("T A M B A H  P R O D U K", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\tMasukan nama produk        : ";
    getline(cin, new_produk.nama);

    cout << "\t\t\t\t\tMasukan spesifikasi produk : ";
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
            string teks;

            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("P R O D U K  S U D A H  T E R S E D I A !", 117) << "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("APAKAH INGIN MENAMBAHKAN STOK PRODUK?  (Y/T)", 117)<<"|" << endl;
            cout << "=======================================================================================================================" << endl;
            cout << "\t\t\t\t\t\tMasukan Jawaban : ";
            cin >> pilih_stok;
            cin.ignore();

            if (pilih_stok == 'y' || pilih_stok == 'Y')
            {
                cout << "\t\t\t\tMasukkan jumlah stok yang ingin ditambahkan: ";
                cin >> tambah_stok;
                cin.ignore();

                daftar_produk[idx].stok += tambah_stok;
                simpan_produk();
                teks = "TOTAL STOK SEKARANG : " + daftar_produk[idx].stok; 
                cout << "=======================================================================================================================" << endl;
                cout << "|" << format_Tengah_teks("STOK BERHASIL DITAMBAHKAN!", 117) << "|" << endl;
                cout << "|" << format_Tengah_teks(teks, 117) << "|" << endl;
                cout << "=======================================================================================================================" << endl;
            }
            else
            {
                cout << "=======================================================================================================================" << endl;
                cout << "|" << format_Tengah_teks("PENAMBAHAN STOK DIBATALKAN!",117) << "|" << endl;
                cout << "=======================================================================================================================" << endl;
            }

            cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cin.get();
            system("cls");
            return;
        }
    }

    // kalau belum ada, lanjut tambah produk baru
    
    cout << "\t\t\t\t\tMasukan harga produk       : Rp.";
    cin >> new_produk.harga;
    cin.ignore();

    cout << "\t\t\t\t\tMasukan jumlah stok        : ";
    cin >> new_produk.stok;
    cin.ignore();

    daftar_produk[jml_produk].nama = new_produk.nama;
    daftar_produk[jml_produk].spesifikasi = new_produk.spesifikasi;
    daftar_produk[jml_produk].harga = new_produk.harga;
    daftar_produk[jml_produk].stok = new_produk.stok;

    jml_produk++;
    simpan_produk();

    cout << "=======================================================================================================================" << endl;
    cout << "|" <<format_Tengah_teks("P R O D U K  B E R H A S I L   D I T A M B A H K A N !", 117) << "|" << endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
}

void hapus_produk()
{
    if (jml_produk == 0)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("TIDAK ADA PRODUK YANG TERSEDIA UNTUK DIHAPUS.", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "==================================================== HAPUS PRODUK =====================================================" << endl;
    cout << "|" << format_Tengah_teks("NO", 4) << "|"
         << format_Tengah_teks("NAMA", 28) << "|"
         << format_Tengah_teks("SPESIFIKASI", 60) << "|"
         << format_Tengah_teks("HARGA", 15) << "|"
         << format_Tengah_teks("STOK", 6) << "|" << endl;
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
    cout << "\t\t\t\t\tMasukkan nomor produk yang ingin dihapus: ";
    cin >> nomor;
    cin.ignore();

    if (nomor < 1 || nomor > jml_produk)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("N O M O R  T I D A K  V A L I D !", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
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
    system("cls");
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("Produk " + nama_dihapus + " berhasil dihapus!", 117) <<"|"<<endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl; 
    cin.get();
    system("cls");
}

void edit_harga_produk()
{
    if (jml_produk == 0)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("TIDAK ADA PRODUK YANG TERSEDIA UNTUK DIEDIT.", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("E D I T  H A R G A  P R O D U K", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
     cout << "|" << format_Tengah_teks("NO", 4) << "|"
          << format_Tengah_teks("NAMA",70) << "|"
          << format_Tengah_teks("HARGA SAAT INI", 41) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    for (int i = 0; i < jml_produk; i++)
    {
        cout << "| " << right << setw(2) << i + 1 << "." << "|"
             << setw(70) << left << daftar_produk[i].nama << "| Rp."
             << setw(37) << fixed << setprecision(0) << daftar_produk[i].harga << "|" << endl;
    }
    cout << "=======================================================================================================================" << endl;

    int nomor;
    cout << "\t\t\t\tMasukkan nomor produk yang ingin diubah harganya: ";
    cin >> nomor;
    cin.ignore();

    if (nomor < 1 || nomor > jml_produk)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("N O M O R  T I D A K  V A L I D !", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    double harga_baru;
    cout << "\t\t\t\tMasukkan harga baru untuk " << daftar_produk[nomor - 1].nama << " : Rp.";
    cin >> harga_baru;
    cin.ignore();
    system("cls");

    if (harga_baru < 0)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("HARGA TIDAK VALID (HARGA TIDAK BOLEH MINUS)!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    daftar_produk[nomor - 1].harga = harga_baru;
    simpan_produk();
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("HARGA PRODUK BERHASIL DIPERBARUI!", 117) << "|" << endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
}

void tampilkan_produk(bool pembeli)
{
    if (jml_produk == 0)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("T I D A K   A D A  P R O D U K  Y A N G  T E R S E D I A .", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    cout.imbue(locale(cout.getloc(), new format_rupiah));

    cout << "=======================================================================================================================" << endl;
    cout << "|"<< format_Tengah_teks("D A F T A R  P R O D U K",117)<<"|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("NO", 4) << "|"
         << format_Tengah_teks("NAMA", 28) << "|"
         << format_Tengah_teks("SPESIFIKASI", 60) << "|"
         << format_Tengah_teks("HARGA", 15) << "|"
         << format_Tengah_teks("STOK", 6) << "|" << endl;
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

    produk temp[MAX_PRODUK];
    for(int i = 0; i < jml_produk; i++) temp[i] = daftar_produk[i];

    char pilih_sorting;
    cout << "|" << format_Tengah_teks("URUTKAN PRODUK DARI HARGA TERRENDAH?  (Y/T)", 117)<<"|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan Jawaban : ";
    cin >> pilih_sorting;
    cin.ignore();

    if (pilih_sorting == 'y' || pilih_sorting == 'Y')
    {
        system("cls");
        sorting();

        for(int i = 1; i < jml_produk; i++){
            produk key = temp[i];
            int j = i - 1;
            while(j >= 0 && temp[j].harga > key.harga) {
                temp[j + 1] = temp[j];
                j--;
            }
            temp[j + 1] = key;
        }
    }

    // kalau penjual, selesai sampai sini
    if (!pembeli)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    // kalau pembeli, bisa tambah ke keranjang
    char pilihTambah;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("APAKAH INGIN MENAMBAHKAN PRODUK KE KERANJANG?  (Y/T)", 117)<<"|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan Jawaban : ";
    cin >> pilihTambah;
    cin.ignore();

    if (pilihTambah == 'y' || pilihTambah == 'Y')
    {
        int pilih_nomor, jumlah;

        cout << "\t\t\t\t\t\tMasukkan nomor produk: ";
        cin >> pilih_nomor;
        cin.ignore();

        if (pilih_nomor < 1 || pilih_nomor > jml_produk)
        {
            cout << "=======================================================================================================================" << endl;
            cout << "|"<< format_Tengah_teks("N O M O R   T I D A K  V A L I D !", 117) <<"|" << endl;
            cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cin.get();
            system("cls");
            return;
        }

        cout << "\t\t\t\t\t\tMasukkan jumlah: ";
        cin >> jumlah;
        cin.ignore();
        system("cls");

        if (jumlah > daftar_produk[pilih_nomor - 1].stok)
        {
            cout << "=======================================================================================================================" << endl;
            cout << "|"<< format_Tengah_teks("S T O K   T I D A K   M E N C U K U P I !", 117)<<"|" << endl;
            cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cin.get();
            system("cls");
            return;
        }

        keranjang[jml_keranjang].namaProduk = temp[pilih_nomor - 1].nama;
        keranjang[jml_keranjang].jumlah = jumlah;
        keranjang[jml_keranjang].harga = temp[pilih_nomor - 1].harga;

        jml_keranjang++;

        cout << "=======================================================================================================================" << endl;
        cout << "|" <<format_Tengah_teks("P R O D U K  B E R H A S I L   D I T A M B A H K A N  K E  K E R A N J A N G !", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
    }
    else
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|"<< format_Tengah_teks("T I D A K   J A D I  M E N A M B A H K A N  P R O D U K", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
    }

    cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
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
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("T I D A K   A D A  A N T R I A N !", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("A N T R I A N", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("NO", 4) << "|"
         << format_Tengah_teks("NAMA", 80) << "|"
         << format_Tengah_teks("JUMLAH", 31) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    for (int i = 0; i < jumlah; i++)
    {
        cout << "| " << right << setw(3) << (to_string(i + 1) + ".") << "|" 
                 << setw(80) << left << dataDiproses[i]->namaProduk << "|" 
                 << setw(31) << dataDiproses[i]->jumlah << "|" <<endl;
    }

    char pilih;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("APAKAH ADA PRODUK YANG INGIN DIPROSES?  (Y/T)", 117)<<"|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan Jawaban : ";
    cin >> pilih;
    cin.ignore();

    if (pilih == 'y' || pilih == 'Y')
    {
        int nomor;
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("PRODUK NOMER BERAPA YANG INGIN DI PROSES?", 117)<<"|" << endl;
        cout << "=======================================================================================================================" << endl;
        cout << "\t\t\t\t\t\tMasukan Jawaban : ";;
        cin >> nomor;
        cin.ignore();

        if (nomor < 1 || nomor > jumlah)
        {
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("N O M O R  T I D A K  V A L I D", 117)<<"|" << endl;
            cout << "|" << format_Tengah_teks("Tekan Enter Untuk Kembali...", 117)<<"|" << endl;
            cout << "=======================================================================================================================" << endl;
            cin.get();
            system("cls");
            return;
        }

        dataDiproses[nomor - 1]->status = "Dikirim";
        simpan_nota();

        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("PRODUK SEDANG DALAM PROSES PENGIRIMAN", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");

        lihat_antrian();
    }
    else
    {
        system("cls");
    }
}

void dequeue()
{
    Nota *temp = head;
    bool ada = false;
    int no = 1;
    bool header = false;

    while (temp != NULL)
    {
        if (temp->status == "Dikirim")
        {
            if(!header){
                cout << "=======================================================================================================================" << endl;
                cout << "|" << format_Tengah_teks("P R O S E S  P E N G I R I M A N", 117) << "|" << endl;
                cout << "=======================================================================================================================" << endl;
                cout << "|" << format_Tengah_teks("NO", 4) << "|"
                << format_Tengah_teks("NAMA", 80) << "|"
                << format_Tengah_teks("JUMLAH", 31) << "|" << endl;
                cout << "=======================================================================================================================" << endl;
                header = true;
            }
            cout << "| " << right << setw(3) << (to_string(no) + ".") << "|" 
                 << setw(80) << left << temp->namaProduk << "|" 
                 << setw(31) << temp->jumlah << "|" <<endl;

            ada = true;
            no++;
        }
        temp = temp->next;
    }

    cout << "=======================================================================================================================" << endl;

    if (!ada)
    {
        cout << "|" << format_Tengah_teks("TIDAK ADA PRODUK YANG SEDANG DALAM PENGIRIMAN", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
    }

    cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
}

void edit_stok() {
    if(jml_produk == 0) {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("TIDAK ADA PRODUK YANG TERSEDIA.", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return; 
    }
    cout.imbue(locale(cout.getloc(), new format_rupiah));
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("D A F T A R  P R O D U K", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("NO", 4) << "|"
         << format_Tengah_teks("NAMA", 28) << "|"
         << format_Tengah_teks("SPESIFIKASI", 60) << "|"
         << format_Tengah_teks("HARGA", 15) << "|"
         << format_Tengah_teks("STOK", 6) << "|" << endl;
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
    cout << "\t\t\t\tMasukan nomor produk yang ingin diubah stoknya : ";
    cin >> nomor_produk;
    cin.ignore();
        system("cls");

    if(nomor_produk < 1 || nomor_produk > jml_produk) {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("N O M O R  T I D A K  V A L I D !", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    int stok_baru;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("U B A H  S T O K", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\tMasukan Stok baru untuk " << daftar_produk[nomor_produk - 1].nama << " : ";
    cin >> stok_baru;
    cin.ignore();
    system("cls");

    if(stok_baru < 0) {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("S T O K  T I D A K  V A L I D !", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("ANGKA TIDAK BOLEH MINUS!", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    daftar_produk[nomor_produk - 1].stok = stok_baru;
    simpan_produk();

    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("S T O K  B E R H A S I L  D I P E R B A R U I", 117) << "|" << endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
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
        if (username[i] == user && password[i] == pass){
            indexLogin = i;
            return true;
        }
    }
    return false;
}

bool tambah_akun()
{
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("R E G I S T E R", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    string user, pass   ;
    cout << "\t\t\t\t\t\tMasukan username : ";
    getline(cin, user);
    cout << "\t\t\t\t\t\tMasukkka password : ";
    getline(cin, pass);

    system("cls");

    for(int i = 0; i < jml_akun; i++){
        if(username[i] == user) {
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("USER SUDAH ADA, SILAHKAN COBA LAGI!", 117) << "|" << endl;
            cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
            cout << "=======================================================================================================================" << endl;
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
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("SELAMAT ANDA BERHASIL MENDAFTAR, SELAMAT BERBELANJA!", 117) << "|" << endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk Melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
    return true;
}

// ===========================
//    MENU
// ===========================
void menu_awal()
{
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("M E N U  A W A L", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "|                                                  1.Sebagai Penjual                                                  |" << endl;
    cout << "|                                                  2.Sebagai Pembeli                                                  |" << endl;
    cout << "|                                                  3.keluar                                                           |" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan pilihan anda : ";
    cin >> pilihan_awal;
    cin.ignore();
    system("cls");
}

void menu_penjual()
{
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("M E N U  A D M I N", 117) << "|" << endl; 
    cout << "=======================================================================================================================" << endl;
    cout << "|                                                  1. Tambah Barang                                                   |" << endl;
    cout << "|                                                  2. Hapus Barang                                                    |" << endl;
    cout << "|                                                  3. Edit Harga Barang                                               |" << endl;
    cout << "|                                                  4. Lihat data barang                                               |" << endl;
    cout << "|                                                  5. Lihat Antrian                                                   |" << endl;
    cout << "|                                                  6. Proses Pengiriman                                               |" << endl;
    cout << "|                                                  7. Edit Stok                                                       |" << endl;
    cout << "|                                                  8. Keluar                                                          |" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukkan pilihan anda : ";
    cin >> pilih;
    cin.ignore();
    system("cls");
}

void menu_login_buyer()
{
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("S I L A H K A N  L O G I N", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl; 
    cout << "|                                                     1.Login                                                         |" << endl;
    cout << "|                                                     2.Register                                                      |" << endl;
    cout << "|                                                     3.keluar                                                        |" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan pilihan anda : ";
    cin >> pilihan_login;
    cin.ignore();
    system("cls");
}

void menu_utama()
{
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("S E L A M A T  D A T A N G  D I  T O K O  K A M I", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl; 
    cout << "|                                                1. Tampilkan Produk                                                  |" << endl;
    cout << "|                                                2. Searching Produk                                                  |" << endl;
    cout << "|                                                3. Lihat Keranjang                                                   |" << endl;
    cout << "|                                                4. Riwayat Pembelian                                                 |" << endl;
    cout << "|                                                5. Barang Sudah Sampai                                               |" << endl;
    cout << "|                                                6. Keluar                                                            |" << endl;
    cout << "=======================================================================================================================" << endl; 
    cout << "\t\t\t\t\t\tMasukan pilihan anda : ";
    cin >> pilihan;
    cin.ignore();
    system("cls");
}


// ===========================
//   PEMBELI FUNCTION
// ===========================
void search_produk()
{
    string text;
    string cari;
    bool ditemukan = false;
    int nomor = 1;

    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("H A P P Y  S E A R C H I N G", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\tMasukkan nama produk yang dicari : ";
    getline(cin, cari);
    cari = toLower(cari);

    int hasil_index[100];
    int jml_hasil = 0;
    bool header = false;

    cout.imbue(locale(cout.getloc(), new format_rupiah));

    for (int i = 0; i < jml_produk; i++)
    {
        if (toLower(daftar_produk[i].nama).find(cari) != string::npos)
        {
        if (!header){
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("P R O D U K  D I T E M U K A N", 117) << "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("NO", 4) << "|"
                 << format_Tengah_teks("NAMA", 28) << "|"
                 << format_Tengah_teks("SPESIFIKASI", 60) << "|"
                 << format_Tengah_teks("HARGA", 15) << "|"
                 << format_Tengah_teks("STOK", 6) << "|" << endl;
            cout << "=======================================================================================================================" << endl;

            header = true;
        }   
            cout << "| " << right << setw(2) << nomor++ << "." << "|"
                 << setw(28) << left << daftar_produk[i].nama << "|"
                 << setw(60) << daftar_produk[i].spesifikasi << "|" << "Rp."
                 << setw(12) << fixed << setprecision(0) << daftar_produk[i].harga << "|"
                 << setw(6) << daftar_produk[i].stok << "|" << endl;
            hasil_index[jml_hasil++] = i;
            ditemukan = true;
        }
    }
    if(!ditemukan){
        cout << "======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("P R O D U K  T I D A K  T E R S E D I A!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    char pilihTambah;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("APAKAH INGIN MENAMBAHKAN PRODUK KE KERANJANG?  (Y/T)", 117)<<"|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan Jawaban : ";
    cin >> pilihTambah;
    cin.ignore();

    if(pilihTambah == 't' || pilihTambah == 'T'){
        system("cls");
        return;
    }

    if(pilihTambah == 'y' || pilihTambah == 'Y')
    {
        int pilih_nomor, jumlah;

        cout << "\t\t\t\t\t\tMasukan Nomor Produk : ";
        cin >> pilih_nomor;
        cin.ignore();

        if(pilih_nomor < 1 || pilih_nomor > jml_hasil){
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("N O M O R   T I D A K  V A L I D !", 117) <<"|" << endl;
            cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cin.get();
            system("cls");
            return;
        }

        cout << "\t\t\t\t\t\tJumlah               : ";
        cin >> jumlah;
        cin.ignore();

        int idx = hasil_index[pilih_nomor -1];
        if(jumlah > daftar_produk[idx].stok) {
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("S T O K   T I D A K   T E R S E D I A !", 117)<<"|" << endl;
            cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cin.get();
            system("cls");
            return;
        }

        keranjang[jml_keranjang].namaProduk = daftar_produk[idx].nama;
        keranjang[jml_keranjang].jumlah = jumlah;
        keranjang[jml_keranjang].harga = daftar_produk[idx].harga;

        jml_keranjang++;

        system("cls");
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("PRODUK", 55) 
             << "|" << format_Tengah_teks("JUMLAH", 15) 
             << "|" << format_Tengah_teks("STATUS", 45) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cout << "|" << setw(55) << (" " + daftar_produk[idx].nama)
             << "|" << setw(15) << (" " + to_string(jumlah))
             << "|" << setw(45) << " Sudah Ditambahkan ke keranjang" << "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
    }
    char lanjut;
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("APAKAH INGIN LANJUT SEARCHING PRODUK?  (Y/T)", 117)<<"|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan Jawaban : ";
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
    baru->username = username[indexLogin];
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

void barang_sampai()
{
    Nota *temp = head;
    Nota *dataDikirim[100];
    int jumlah = 0;
    bool header = false;

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
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("TIDAK ADA BARANG YANG SEDANG DALAM PENGIRIMAN", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    
    for (int i = 0; i < jumlah; i++)
    {
        if(!header){
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("B A R A N G  D I K I R I M", 117) << "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cout << "|" << format_Tengah_teks("NO", 4) << "|"
                << format_Tengah_teks("NAMA", 80) << "|"
                << format_Tengah_teks("JUMLAH", 31) << "|" << endl;
            cout << "=======================================================================================================================" << endl;    

                header = true;
        }
        cout << "| " << right << setw(3) << (to_string(i + 1) + ".") << "|" 
                 << setw(80) << left << dataDikirim[i]->namaProduk << "|" 
                 << setw(31) << dataDikirim[i]->jumlah << "|" <<endl;
    }

    int nomor;
    char konfirmasi;

    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\tPilih nomor barang yang sudah diterima: ";
    cin >> nomor;
    cin.ignore();

    if (nomor < 1 || nomor > jumlah)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|"<< format_Tengah_teks("N O M O R   T I D A K  V A L I D !", 117) <<"|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }


    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("APAKAH ADA PRODUK SUDAH DITERIMA?  (Y/T)", 117)<<"|" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukan Jawaban : ";
    cin >> konfirmasi;
    cin.ignore();
    system("Cls");

    if (konfirmasi == 'y' || konfirmasi == 'Y')
    {
        dataDikirim[nomor - 1]->status = "Selesai";
        simpan_nota();

        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("BARANG TERKONFIRMASI SUDAH DITERIMA PEMBELI!", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
    }
    else
    {
        cout << "=======================================================================================================================" << endl;;
        cout << "|" << format_Tengah_teks("KONFIRMASI PENERIMAAN BARANG DIBATALKAN!", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
    }

    cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
}

void tampil_riwayat()
{
    Nota *temp = head;
    bool ditemukan = false;

    if (temp == NULL)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("BELUM ADA RIWAYAT PEMBELIAN!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    cout.imbue(locale(cout.getloc(), new format_rupiah));
    while (temp !=NULL)
    {
        if(temp->username == username[indexLogin]){
            ditemukan = true;
            cout << "==================================" << endl;
            cout << "|   N O T A  P E M B E L I A N   |" << endl;
            cout << "==================================" << endl;
            cout << "Produk  : " << temp->namaProduk << endl;
            cout << "Jumlah  : " << temp->jumlah << endl;
            cout << "Total   : Rp. " << fixed << setprecision(0) << temp->totalHarga << endl;
            cout << "Waktu   : " << temp->waktu << endl;
            cout << "Status  : " << temp->status << endl;
            cout << "==================================" << endl;
        }

        temp = temp->next;
    }

    if(!ditemukan){
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("BELUM ADA RIWAYAT PEMBELIAN!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }
    cout << "Tekan enter untuk kembali..." <<endl;
    cin.get();
    system("cls");
    return;
}

void tampil_keranjang()
{
    if (jml_keranjang == 0)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("MAAF, KERANJANG MASIH KOSONG!", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }
    else
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("K E R A N J A N G  B E L A N J A", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("NO", 4) << "|"
             << format_Tengah_teks("NAMA", 60) << "|"
             << format_Tengah_teks("JUMLAH", 10) << "|"
             << format_Tengah_teks("HARGA", 40) << "|" << endl;
         cout << "=======================================================================================================================" << endl;
        double total = 0;
        int total_jumlah = 0;
        for (int i = 0; i < jml_keranjang; i++)
        {
            double subtotal = keranjang[i].harga * keranjang[i].jumlah;

            cout << "| " << right << setw(3) << (to_string(i + 1) + ".") << "|" 
                 << setw(60) << left << keranjang[i].namaProduk << "|" 
                 << format_Tengah_teks(to_string(keranjang[i].jumlah), 10)<< "| Rp." 
                 << setw(36) << fixed << subtotal << "|" << endl;
                 
                 total += keranjang[i].harga * keranjang[i].jumlah;
                 total_jumlah += keranjang[i].jumlah;
        }
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("TOTAL", 65) <<"|"<< format_Tengah_teks(to_string(total_jumlah), 10) << "| Rp." << fixed << setw(36) << total << "|" << endl;
        cout << "=======================================================================================================================" << endl;
    }
}

void menu_keranjang()
{
    tampil_keranjang();
    cout << "=======================================================================================================================" << endl;
    cout << "|                                               1. Tambah Jumlah Barang                                               |" << endl;
    cout << "|                                               2. Hapus Barang                                                       |" << endl;
    cout << "|                                               3. Checkout                                                           |" << endl;
    cout << "|                                               4. Undo Barang dihapus                                                |" << endl;
    cout << "|                                               5. Kembali                                                            |" << endl;
    cout << "=======================================================================================================================" << endl;
    cout << "\t\t\t\t\t\tMasukkan Pilihan : ";
    cin >> pilih;
    cin.ignore();
    system("cls");
}

void hapus_keranjang()
{
    if (jml_keranjang == 0)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("K E R A N J A N G  K O S O N G !", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    tampil_keranjang();
    int index;
    cout << "\t\t\t\t Masukkan nomor barang yang ingin dihapus: ";
    cin >> index;
    cin.ignore();
    system("cls");

    if (index < 1 || index > jml_keranjang)
    {
        cout << "=======================================================================================================================" << endl;
        cout << "|"<< format_Tengah_teks("N O M O R   T I D A K  V A L I D !", 117) <<"|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
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
    cout << "=======================================================================================================================" << endl;
    cout << "|" << format_Tengah_teks("BARANG BERHASIL DIHAPUS!", 117) << "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
}

void undo_hapus_keranjang() {
    if(sKeranjang.top < 0) {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("TIDAK ADA BARANG YANG BISA DI-UNDO", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("B A R A N G  D I H A P U S", 117) << "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("NO", 4) << "|"
             << format_Tengah_teks("NAMA", 59) << "|"
             << format_Tengah_teks("JUMLAH", 11) << "|"
             << format_Tengah_teks("HARGA", 40) << "|" << endl;
         cout << "=======================================================================================================================" << endl;
         for(int i = 0; i <= sKeranjang.top; i++) {
                cout << "| " << right << setw(2) << (i + 1) << "." << "|" 
                 << setw(59) << left << sKeranjang.data[i].namaProduk << "|" 
                 << setw(11) << sKeranjang.data[i].jumlah << "| Rp." 
                 << setw(36) << fixed << setprecision(0) << sKeranjang.data[i].harga << "|" << endl;    
         }
         cout << "=======================================================================================================================" << endl;
         
         int pilih_undo;
         cout << "\t\t\t\t\tPilih nomor barang untuk dikembalikan : ";
         cin >> pilih_undo;
         cin.ignore();
         system("cls");

         if (pilih_undo < 1 || pilih_undo > sKeranjang.top+1) {
            cout << "=======================================================================================================================" << endl;
            cout << "|"<< format_Tengah_teks("N O M O R   T I D A K  V A L I D !", 117) <<"|" << endl;
            cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cin.get();
            system("cls");
            return;
         }

         Keranjang kembali = pop_keranjang(pilih_undo - 1);
         keranjang[jml_keranjang++] = kembali;
         cout << "=======================================================================================================================" << endl;
         cout << "|" << format_Tengah_teks("BARANG SUDAH DIKEMBALIKAN!", 117) << "|" << endl;
         cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
         cout << "=======================================================================================================================" << endl;;
         cin.get();
         system("cls");
}

void tambah_keranjang()
{
    if (jml_keranjang == 0) {
        cout << "=======================================================================================================================" << endl;
        cout << "|" << format_Tengah_teks("K E R A N J A N G   K O S O N G !", 117) << "|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    tampil_keranjang();
    int index , jumlah;
    cout << "\t\t\t\t\tMasukan nomor produk di keranjang : ";
    cin >> index;
    cin.ignore();

    if(index < 1 || index > jml_keranjang) {
        cout << "=======================================================================================================================" << endl;
        cout << "|"<< format_Tengah_teks("N O M O R   T I D A K  V A L I D !", 117) <<"|" << endl;
        cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
        cout << "=======================================================================================================================" << endl;
        cin.get();
        system("cls");
        return;
    }

    cout << "\t\t\t\t\tMasukan Jumlah tambahan           : ";
    cin >> jumlah;
    cin.ignore();
    system("cls");

    keranjang[index - 1].jumlah += jumlah;
    cout << "=======================================================================================================================" << endl;
    cout << "|" <<format_Tengah_teks("P R O D U K  B E R H A S I L   D I T A M B A H K A N !", 117) << "|" << endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
    cin.get();
    system("cls");
}

void enqueue(string nama, int jumlah)
{
    antrian[rear].namaProduk = nama;
    antrian[rear].jumlah = jumlah;
    rear++;

}

void Checkout()
{
    bool berhasil = false;
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
    cout << "=======================================================================================================================" << endl;
    cout << "|" <<format_Tengah_teks("C H E C K O U T  B E R H A S I L !", 117) << "|" << endl;
    cout << "|" << format_Tengah_teks("Tekan enter untuk kembali...", 117)<< "|" << endl;
    cout << "=======================================================================================================================" << endl;
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
            cout << "=======================================================================================================================" << endl;
            cout << "|" <<format_Tengah_teks("V E R I F I K A S I  K O D E", 117) << "|" << endl;
            cout << "=======================================================================================================================" << endl;
            cout << "\t\t\t\t\t\tMasukkan Kode : ";
            getline(cin, kode_kode);
            system("cls");

            // CEK KODE BERHASIL
            if (kode_kode == Codes[0].kode_penjual)
            {
                login_kode = true;
                cout << "=================================================== Login Berhasil! ===================================================" << endl;
                cout << "|" <<format_Tengah_teks("S E L A M A T  D A T A N G  A D M I N", 117) << "|" << endl;
                cout << "|" << format_Tengah_teks("Tekan enter untuk melanjutkan...", 117)<< "|" << endl;
                cout << "=======================================================================================================================" << endl;
                cin.get();
                system("cls");
                goto menu_penjual;
            }
            else
            {
                cout << "===================================================== Kode Gagal! =====================================================" << endl;
                cout << "|" <<format_Tengah_teks("KODE YANG ANDA MASUKKAN SALAH!", 117) << "|" << endl;
                cout << "|" <<format_Tengah_teks("C O B A  L A G I !", 117) << "|" << endl;
                cout << "=======================================================================================================================" << endl;
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
            cout << "======================================================**********=======================================================" << endl;
            cout << "|" <<format_Tengah_teks("PILIHAN SALAH, TEKAN ENTER UNTUK COBA LAGI!", 117) << "|" << endl;
            cout << "======================================================**********=======================================================" << endl;
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
            cout << "======================================================**********=======================================================" << endl;
            cout << "|" <<format_Tengah_teks("TERIMA KASIH ATAS KERJA SAMANYA!", 117) << "|" << endl;
            cout << "======================================================**********=======================================================" << endl;
            cin.get();
            system("cls");
            goto menu_awal;
        default:
            cout << "======================================================**********=======================================================" << endl;
            cout << "|" <<format_Tengah_teks("P I L I H A N  T I D A K  V A L I D !", 117) << "|" << endl;
            cout << "|" <<format_Tengah_teks("Harap Coba Lagi....", 117) << "|" << endl;
            cout << "======================================================**********=======================================================" << endl;
            cin.get();
            system("cls");
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
            cout << "=======================================================================================================================" << endl;
            cout << "|" <<format_Tengah_teks("S I L A H K A N  L O G I N", 117) << "|" << endl;
            cout << "=======================================================================================================================" << endl;
            string user, pass;
            cout << "\t\t\t\t\t\tMasukkan username : ";
            getline(cin, user);
            cout << "\t\t\t\t\t\tMasukkan Password : ";
            getline(cin, pass);

            system("cls");

            if (login(user, pass))
            {
                cout << "======================================================**********=======================================================" << endl;
                cout << "|" <<format_Tengah_teks("L O G I N  B E R H A S I L !", 117) << "|" << endl;
                cout << "|" <<format_Tengah_teks("S E L A M A T  B E R B E L A N J A", 117) << "|" << endl;
                cout << "======================================================**********=======================================================" << endl;
                cin.get();
                system("cls");
                goto menu_utama;
            }
            else
            {
                cout << "======================================================**********=======================================================" << endl;
                cout << "|" <<format_Tengah_teks("L O G I N  G A G A L !", 117) << "|" << endl;
                cout << "|" <<format_Tengah_teks("S I L A H K A N  C O B A  L A G I", 117) << "|" << endl;
                cout << "======================================================**********=======================================================" << endl;
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
            cout << "======================================================**********=======================================================" << endl;
            cout << "|" <<format_Tengah_teks("PILIHAN SALAH, TEKAN ENTER UNTUK COBA LAGI!", 117) << "|" << endl;
            cout << "======================================================**********=======================================================" << endl;
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
                if (jml_keranjang == 0)
                {
                    tampil_keranjang(); 
                    break;
                }

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
                    goto menu_utama;
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
            cout << "======================================================**********=======================================================" << endl;
            cout << "|" <<format_Tengah_teks("TERIMA KASIH SUDAH BERBELANJA!", 117) << "|" << endl;
            cout << "======================================================**********=======================================================" << endl;
            cin.get();
            system("cls");
            goto menu_awal;
            break;

        default:
            cout << "======================================================**********=======================================================" << endl;
            cout << "|" <<format_Tengah_teks("P I L I H A N  T I D A K  V A L I D !", 117) << "|" << endl;
            cout << "|" <<format_Tengah_teks("Harap Coba Lagi....", 117) << "|" << endl;
            cout << "======================================================**********=======================================================" << endl;
            cin.get();
            system("cls");
            break;
        }

    } while (pilihan != 6);
}
