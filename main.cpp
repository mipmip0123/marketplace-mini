#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;

//login
const int MAX_AKUN =100;
int jml_akun = 0;

string username[MAX_AKUN];
string password[MAX_AKUN];

char pilihan_login;

void simpan_akun() {
    ofstream file ("database_akun.txt");
    if (!file) {
        cout << "Gagal Membuka file untuk menyimpan!\n";
        return;
    }
    for (int i = 0; i < jml_akun; i++) {
        file << username[i] << "|" << password[i] << endl;
    }
    file.close();
    cout << "Data akun berhasil disimpan" << endl;
}

int muat_akun() {
    ifstream file("database_akun.txt");
    if (!file) {
        cout << ">> Database belum ada, mulai dengan data default.\n";
        return 0;
    }
    jml_akun = 0;
    while (getline(file, username[jml_akun], '|') && 
    getline(file, password[jml_akun])) {
        jml_akun++;
    }
    file.close();
    return jml_akun;
}

void tambah_akun() {
    cout << "Masukan username : ";
    getline(cin, username[jml_akun]);
    cout << "Masukkka password : ";
    getline(cin, password[jml_akun]);
    jml_akun++;
    simpan_akun();
    system("cls");
    cout << "Selamat anda berhasil mendaftar, selamat berbelanja" << endl;
}

bool login(string user, string pass){
    for(int i = 0; i < jml_akun; i++){
        if(username[i] == user && password[i] == pass) return true;
    }
    return false;
}

void menu_login(){
    cout << "======= MENU LOGIN =======" << endl;
    cout << "1.Login akun" << endl;
    cout << "2.Buat akun" << endl;
    cout << "3.keluar" << endl;
    cout << "==========================" << endl;
    cout << "Masukan pilihan anda : ";
    cin >> pilihan_login;
    cin.ignore();
    system("cls");
}


int main() {
    int pilihan;

    //login
    muat_akun();
    menu_login :
    do {
    menu_login();
    switch (pilihan_login)
    {
    case '1': {
        string user, pass;
        cout << "Masukkan username : ";
        getline(cin, user);
        cout << "Masukkan Password :";
        getline(cin, pass);
        
        if(login(user, pass)){
            cout << "Login berhasil" << endl;
            cout << "Selamat berbelanja" << endl;
        } else {
            cout << "Login Gagal" << endl;
            cout << "Silahkan coba lagi" << endl;
        }
        system("cls");
        cout << "Selamat datang di marketplace mini"<< endl;
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
        cout << "\t============================****=============================";
        cout << "\n\t\t  pilihan salah, tekan enter untuk coba lagi";
        cout << "\n\t============================****=============================";
        cin.get ();
        system ("cls");
        goto menu_login;
        break;
    }
    }while( pilihan_login != 3);

    menu_utama :
    //menu utama
    do {
        cout << "=========== MARKETPLACE HANDPHONE ==========" << endl;
        cout << "\t1.Tambah data" << endl;
        cout << "\t2.Tampilkan data" << endl;
        cout << "\t3.Updtate data" << endl;
        cout << "\t4.Hapus data" << endl;
        cout << "\t5.Cari data" << endl;
        cout << "\t6.Urutkan data" << endl;
        cout << "\t7.Simulasi stack" << endl;
        cout << "\t8.Simulasi queue" << endl;
        cout << "\t9.Keluar" << endl;
        cout << "============================================" << endl;
        cout << "Masukan pilihan anda : ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            
            break;
        case 2:
            
            break;
        case 3:
            
            break;
        case 4:
            
            break;
        case 5:
            
            break;
        case 6:
            
            break;
        case 7:
            
            break;
        case 8:
            
            break;
        case 9:
            cout << "\nTerimakasih" << endl;
            break;
        
        default:
            break;
        }

    } while(pilihan != 9);
    
}