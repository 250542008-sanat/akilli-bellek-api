/**
 * SENARYO A - Sağlıklı Bellek Simülasyonu
 * RAM Kurtarıcıları | Akıllı Bellek Yönetimi Projesi
 *
 * Bu program bellek tahsislerini (new/delete) DOĞRU şekilde yönetir.
 * Valgrind analizi sonucunda bellek sızıntısı BULUNMAMALIDIR.
 *
 * Derleme : g++ -o senaryo_a senaryo_a.cpp
 * Çalıştırma : ./senaryo_a
 * Valgrind ile: valgrind --leak-check=full ./senaryo_a
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

// ─── Dinamik dizi simülasyonu ─────────────────────────────────────────────────
void dizi_simulasyonu() {
    std::cout << "[SENARYO-A] Dizi simulasyonu basliyor..." << std::endl;

    int boyut = 100;
    // Bellek tahsisi
    int* sayilar = new int[boyut];

    for (int i = 0; i < boyut; i++) {
        sayilar[i] = i * 2;
    }

    int toplam = 0;
    for (int i = 0; i < boyut; i++) {
        toplam += sayilar[i];
    }

    std::cout << "[SENARYO-A] Dizi toplami: " << toplam << std::endl;

    // Bellek iadesi - DOGRU
    delete[] sayilar;
    std::cout << "[SENARYO-A] Dizi bellegi serbest birakildi." << std::endl;
}

// ─── Nesne simülasyonu ────────────────────────────────────────────────────────
void nesne_simulasyonu() {
    std::cout << "[SENARYO-A] Nesne simulasyonu basliyor..." << std::endl;

    // Bellek tahsisi
    AnalizKaydi* kayit = new AnalizKaydi();
    kayit->id            = 1;
    kayit->uygulama_adi  = "chrome.exe";
    kayit->ram_mb        = 245.6;
    kayit->uyari_sayisi  = 0;

    std::cout << "[SENARYO-A] Kayit olusturuldu: "
              << kayit->uygulama_adi
              << " | RAM: " << kayit->ram_mb << " MB"
              << " | Uyari: " << kayit->uyari_sayisi
              << std::endl;

    // Bellek iadesi - DOGRU
    delete kayit;
    std::cout << "[SENARYO-A] Nesne bellegi serbest birakildi." << std::endl;
}

// ─── Çoklu nesne simülasyonu ──────────────────────────────────────────────────
void coklu_nesne_simulasyonu() {
    std::cout << "[SENARYO-A] Coklu nesne simulasyonu basliyor..." << std::endl;

    int adet = 5;
    AnalizKaydi** kayitlar = new AnalizKaydi*[adet];

    for (int i = 0; i < adet; i++) {
        kayitlar[i] = new AnalizKaydi();
        kayitlar[i]->id           = i + 1;
        kayitlar[i]->uygulama_adi = "uygulama_" + std::to_string(i + 1);
        kayitlar[i]->ram_mb       = 100.0 + (i * 10.5);
        kayitlar[i]->uyari_sayisi = 0;
    }

    std::cout << "[SENARYO-A] " << adet << " kayit olusturuldu." << std::endl;

    // Tüm nesnelerin belleğini iade et - DOGRU
    for (int i = 0; i < adet; i++) {
        delete kayitlar[i];
    }
    delete[] kayitlar;

    std::cout << "[SENARYO-A] Tum nesnelerin bellegi serbest birakildi." << std::endl;
}

// ─── Ana fonksiyon ────────────────────────────────────────────────────────────
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SENARYO A - Saglikli Bellek Yonetimi  " << std::endl;
    std::cout << "  RAM Kurtaricilari Projesi              " << std::endl;
    std::cout << "========================================" << std::endl;

    dizi_simulasyonu();
    std::cout << std::endl;

    nesne_simulasyonu();
    std::cout << std::endl;

    coklu_nesne_simulasyonu();
    std::cout << std::endl;

    std::cout << "========================================" << std::endl;
    std::cout << "  SENARYO A tamamlandi.                 " << std::endl;
    std::cout << "  Beklenen Valgrind sonucu:              " << std::endl;
    std::cout << "  'All heap blocks were freed'           " << std::endl;
    std::cout << "  'ERROR SUMMARY: 0 errors'              " << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
