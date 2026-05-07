#ifndef BELLEK_TAKIP_H
#define BELLEK_TAKIP_H

#include <iostream>
#include <unordered_map>

// Hangi dosyada ve kaçıncı satırda sızıntı olduğunu tespit etmek için makrolar
#define TESPIT_ET(tip)      BellekKontrol::ornek().kayitEkle(new tip, sizeof(tip), __FILE__, __LINE__)
#define SERBEST_BIRAK(ptr)  BellekKontrol::ornek().kayitSil(ptr); delete ptr

// Her bellek tahsisinin kaydını tutan yapı
struct BellekKaydi {
    size_t      boyut;
    const char* dosya;
    int         satir;
};

// Singleton bellek takip sınıfı
class BellekKontrol {
public:
    // Tek örnek döndür (Singleton)
    static BellekKontrol& ornek() {
        static BellekKontrol instance;
        return instance;
    }

    // Yeni tahsis edilen belleği kayıt altına al
    void* kayitEkle(void* ptr, size_t boyut, const char* dosya, int satir) {
        havuz[ptr] = { boyut, dosya, satir };
        return ptr;
    }

    // Serbest bırakılan belleği kayıttan sil
    void kayitSil(void* ptr) {
        havuz.erase(ptr);
    }

    // Analiz sonucu raporu yazdır
    void raporla() const {
        std::cout << "\n========================================\n";
        std::cout << "       Bellek Analiz Raporu\n";
        std::cout << "========================================\n";

        if (havuz.empty()) {
            std::cout << "[OK] Sizinti bulunmadi. Tebrikler!\n";
        } else {
            std::cout << "[!!] Toplam " << havuz.size() << " adet sizinti tespit edildi:\n\n";
            for (const auto& [adres, kayit] : havuz) {
                std::cout
                    << "  SIZINTI  : " << kayit.boyut << " byte\n"
                    << "  Adres    : " << adres << "\n"
                    << "  Konum    : " << kayit.dosya << ":" << kayit.satir << "\n"
                    << "  ----------------------------------------\n";
            }
        }

        std::cout << "========================================\n\n";
    }

private:
    // Adres → kayıt eşlemesi
    std::unordered_map<void*, BellekKaydi> havuz;

    // Dışarıdan örnekleme engellenir (Singleton)
    BellekKontrol()  = default;
    ~BellekKontrol() = default;
    BellekKontrol(const BellekKontrol&)            = delete;
    BellekKontrol& operator=(const BellekKontrol&) = delete;
};

#endif // BELLEK_TAKIP_H
