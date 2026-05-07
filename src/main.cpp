#include <iostream>
#include <memory>
#include "BellekTakip.h"

/*
 * Hatalı kullanım örneği:
 * Tahsis edilen bellek hiçbir zaman serbest bırakılmıyor.
 * BellekKontrol bu sızıntıyı tespit edecek.
 */
void hataYapanFonksiyon() {
    int* sızan = TESPIT_ET(int);
    // SERBEST_BIRAK(sızan) çağrılmıyor → sızıntı!
    (void)sızan; // derleyici uyarısını bastır
}

/*
 * Doğru kullanım örneği:
 * std::unique_ptr kapsam dışına çıkınca belleği otomatik serbest bırakır.
 * Sızıntı oluşmaz.
 */
void dogruYapanFonksiyon() {
    std::unique_ptr<int> akilliPtr = std::make_unique<int>(23);
    // Fonksiyon bitince unique_ptr otomatik olarak delete çağırır
}

int main() {
    std::cout << "Bellek analizi baslatiliyor...\n\n";

    std::cout << "[1] Hatali fonksiyon calistirilıyor...\n";
    hataYapanFonksiyon();

    std::cout << "[2] Dogru fonksiyon calistirilıyor...\n";
    dogruYapanFonksiyon();

    // Tüm sızıntıları raporla
    BellekKontrol::ornek().raporla();

    return 0;
}
