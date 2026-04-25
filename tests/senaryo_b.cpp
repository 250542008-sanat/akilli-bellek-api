/**
 * SENARYO B - Hatalı Bellek Simülasyonu (Bellek Sızıntısı)
 * RAM Kurtarıcıları | Akıllı Bellek Yönetimi Projesi
 *
 * Bu program bellek tahsislerini (new) YANLIŞ yönetir.
 * delete çağrılmadan program sonlanır → bellek sızıntısı oluşur.
 * Valgrind analizi sonucunda sızıntılar TESPIT EDILMELIDIR.
 *
 * Derleme : g++ -o senaryo_b senaryo_b.cpp
 * Çalıştırma : ./senaryo_b
 * Valgrind ile: valgrind --leak-check=full ./senaryo_b
 */

#include <iostream>
#include <string>

// ─── Veri yapısı: Bellek analiz kaydı ────────────────────────────────────────
struct AnalizKaydi {
    int     id;
    std::string uygulama_adi;
    double  ram_mb;
    int     uyari_sayisi;
};

// ─── Sızıntı 1: Dizi serbest bırakılmıyor ────────────────────────────────────
void sizinti_1_dizi() {
    std::cout << "[SENARYO-B] Sizinti-1: Dizi tahsis ediliyor..." << std::endl;

    int boyut = 100;
    // Bellek tahsisi yapılıyor
    int* sayilar = new int[boyut];

    for (int i = 0; i < boyut; i++) {
        sayilar[i] = i * 3;
    }

    std::cout << "[SENARYO-B] Sizinti-1: Dizi kullanildi." << std::endl;

    // HATA: delete[] sayilar; cagrilmiyor!
    // Bu 400 byte sızıntıya yol açar (100 * 4 byte)
    std::cout << "[SENARYO-B] UYARI: 400 byte bellek serbest birakilmadi!" << std::endl;
}

// ─── Sızıntı 2: Nesne serbest bırakılmıyor ───────────────────────────────────
void sizinti_2_nesne() {
    std::cout << "[SENARYO-B] Sizinti-2: Nesne tahsis ediliyor..." << std::endl;

    // Bellek tahsisi yapılıyor
    AnalizKaydi* kayit = new AnalizKaydi();
    kayit->id            = 99;
    kayit->uygulama_adi  = "hata_uygulama.exe";
    kayit->ram_mb        = 512.0;
    kayit->uyari_sayisi  = 7;

    std::cout << "[SENARYO-B] Sizinti-2: Nesne olusturuldu: "
              << kayit->uygulama_adi << std::endl;

    // HATA: delete kayit; cagrilmiyor!
    // Bu ~40 byte sızıntıya yol açar
    std::cout << "[SENARYO-B] UYARI: Nesne bellegi serbest birakilmadi!" << std::endl;
}

// ─── Sızıntı 3: Döngüde birikim ──────────────────────────────────────────────
void sizinti_3_dongude_birikim() {
    std::cout << "[SENARYO-B] Sizinti-3: Dongude bellek birikimi basliyor..." << std::endl;

    int tekrar = 10;
    for (int i = 0; i < tekrar; i++) {
        // Her döngüde yeni bellek tahsis ediliyor
        AnalizKaydi* kayit = new AnalizKaydi();
        kayit->id           = i;
        kayit->uygulama_adi = "dongü_" + std::to_string(i);
        kayit->ram_mb       = 50.0 + i;
        kayit->uyari_sayisi = i * 2;

        // HATA: delete kayit; cagrilmiyor!
        // Pointer her döngüde eziliyor, önceki bellek kaybolur
    }

    // 10 nesne × ~40 byte = ~400 byte sızıntı
    std::cout << "[SENARYO-B] UYARI: Dongu icinde " << tekrar
              << " nesne serbest birakilmadi!" << std::endl;
}

// ─── Sızıntı 4: Dangling pointer (sarkan işaretçi) ───────────────────────────
void sizinti_4_dangling_pointer() {
    std::cout << "[SENARYO-B] Sizinti-4: Dangling pointer ornegi..." << std::endl;

    int* ptr1 = new int(42);
    int* ptr2 = ptr1;  // İki pointer aynı belleği gösteriyor

    std::cout << "[SENARYO-B] ptr1 degeri: " << *ptr1 << std::endl;

    delete ptr1;       // Bellek serbest bırakıldı
    ptr1 = nullptr;

    // HATA: ptr2 hâlâ eski adresi gösteriyor (dangling pointer)
    // ptr2'yi kullanmak tanımsız davranış yaratır
    std::cout << "[SENARYO-B] UYARI: ptr2 dangling pointer durumunda!" << std::endl;
}

// ─── Ana fonksiyon ────────────────────────────────────────────────────────────
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SENARYO B - Hatali Bellek Yonetimi    " << std::endl;
    std::cout << "  RAM Kurtaricilari Projesi              " << std::endl;
    std::cout << "========================================" << std::endl;

    sizinti_1_dizi();
    std::cout << std::endl;

    sizinti_2_nesne();
    std::cout << std::endl;

    sizinti_3_dongude_birikim();
    std::cout << std::endl;

    sizinti_4_dangling_pointer();
    std::cout << std::endl;

    std::cout << "========================================" << std::endl;
    std::cout << "  SENARYO B tamamlandi.                 " << std::endl;
    std::cout << "  Beklenen Valgrind sonucu:              " << std::endl;
    std::cout << "  'definitely lost: ~840 bytes'          " << std::endl;
    std::cout << "  'ERROR SUMMARY: hatalar tespit edildi' " << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}