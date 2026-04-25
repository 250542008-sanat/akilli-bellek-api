/**
 * Yapan Kişi:Mustafa Şahingöz
 * Bellek sızıntıları bulundu ve düzeltilmiş kod oluşturuldu.
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

// ─── Sızıntı 1: Dizi serbest bırakılmıyor (DÜZELTİLDİ) ───────────────────────
void sizinti_1_dizi() {
    std::cout << "[SENARYO-B] Sizinti-1: Dizi tahsis ediliyor..." << std::endl;

    int boyut = 100;
    int* sayilar = new int[boyut];

    for (int i = 0; i < boyut; i++) {
        sayilar[i] = i * 3;
    }

    std::cout << "[SENARYO-B] Sizinti-1: Dizi kullanildi." << std::endl;

    // TAMİRAT: Dizi için ayrılan bellek serbest bırakıldı
    delete[] sayilar; 
    std::cout << "[SENARYO-B] DUZELTME: 400 byte bellek basariyla temizlendi!" << std::endl;
}

// ─── Sızıntı 2: Nesne serbest bırakılmıyor (DÜZELTİLDİ) ──────────────────────
void sizinti_2_nesne() {
    std::cout << "[SENARYO-B] Sizinti-2: Nesne tahsis ediliyor..." << std::endl;

    AnalizKaydi* kayit = new AnalizKaydi();
    kayit->id            = 99;
    kayit->uygulama_adi  = "hata_uygulama.exe";
    kayit->ram_mb        = 512.0;
    kayit->uyari_sayisi  = 7;

    std::cout << "[SENARYO-B] Sizinti-2: Nesne olusturuldu: "
              << kayit->uygulama_adi << std::endl;

    // TAMİRAT: Nesne için ayrılan bellek serbest bırakıldı
    delete kayit;
    std::cout << "[SENARYO-B] DUZELTME: Nesne bellegi basariyla temizlendi!" << std::endl;
}

// ─── Sızıntı 3: Döngüde birikim (DÜZELTİLDİ) ─────────────────────────────────
void sizinti_3_dongude_birikim() {
    std::cout << "[SENARYO-B] Sizinti-3: Dongude bellek birikimi basliyor..." << std::endl;

    int tekrar = 10;
    for (int i = 0; i < tekrar; i++) {
        AnalizKaydi* kayit = new AnalizKaydi();
        kayit->id           = i;
        kayit->uygulama_adi = "dongü_" + std::to_string(i);
        kayit->ram_mb       = 50.0 + i;
        kayit->uyari_sayisi = i * 2;

        // TAMİRAT: Her döngü turunda nesnenin işi bitince belleği temizliyoruz
        delete kayit; 
    }

    std::cout << "[SENARYO-B] DUZELTME: Dongu icindeki tum nesneler temizlendi!" << std::endl;
}

// ─── Sızıntı 4: Dangling pointer (sarkan işaretçi) (DÜZELTİLDİ) ──────────────
void sizinti_4_dangling_pointer() {
    std::cout << "[SENARYO-B] Sizinti-4: Dangling pointer ornegi..." << std::endl;

    int* ptr1 = new int(42);
    int* ptr2 = ptr1;  

    std::cout << "[SENARYO-B] ptr1 degeri: " << *ptr1 << std::endl;

    delete ptr1;       
    ptr1 = nullptr;

    // TAMİRAT: ptr2'nin silinmiş bir belleği göstermemesi için onu da sıfırlıyoruz
    ptr2 = nullptr;
    std::cout << "[SENARYO-B] DUZELTME: ptr2 guvenli hale getirildi (nullptr)!" << std::endl;
}

// ─── Ana fonksiyon ────────────────────────────────────────────────────────────
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  SENARYO B - Duzeltilmis Surum          " << std::endl;
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
    std::cout << "  'All heap blocks were freed'           " << std::endl;
    std::cout << "  'ERROR SUMMARY: 0 errors'              " << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
