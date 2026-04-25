/*
 * ============================================================
 *  RAM Kurtarıcıları – Hafta 4
 *  Büyük Boyutlu Verilerin İşlenmesinde Bellek Optimizasyonu
 *  Sorumlu: Aylin Akagündüz
 * ============================================================
 *
 *  Derleme:
 *    g++ -O2 -o buyuk_veri_optimizasyon buyuk_veri_optimizasyon.cpp -lz
 *
 *  Gereksinim:
 *    sudo apt install zlib1g-dev
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <zlib.h>

// ─── 1. zlib ile Sıkıştırma ───────────────────────────────
// Büyük veri bloklarını (resim/video buffer) bellekte
// sıkıştırarak RAM kullanımını azaltır.
std::vector<uint8_t> sikistir(const std::vector<uint8_t>& veri) {
    uLongf sikistirilmis_boyut = compressBound(veri.size());
    std::vector<uint8_t> cikti(sikistirilmis_boyut);

    compress2(cikti.data(), &sikistirilmis_boyut,
              veri.data(), veri.size(), Z_BEST_SPEED);

    cikti.resize(sikistirilmis_boyut);
    return cikti;
}

// ─── 2. Stream (Parçalı) İşleme ──────────────────────────
// Büyük dosyayı tek seferde RAM'e almak yerine
// parça parça okuyup işler; bellek kullanımı sabit kalır.
void stream_isle(const std::string& dosya_adi, size_t parca_boyutu = 4096) {
    std::ifstream dosya(dosya_adi, std::ios::binary);
    if (!dosya.is_open()) {
        std::cerr << "[HATA] Dosya açılamadı: " << dosya_adi << "\n";
        return;
    }

    std::vector<uint8_t> parca(parca_boyutu);
    size_t toplam_okunan = 0;
    int parca_sayisi = 0;

    while (dosya.read(reinterpret_cast<char*>(parca.data()), parca_boyutu)
           || dosya.gcount() > 0) {
        size_t okunan = dosya.gcount();
        toplam_okunan += okunan;
        parca_sayisi++;

        // Her parça işlenir ve scope'tan çıkınca RAII ile serbest bırakılır
        auto sikistirilmis = sikistir({parca.begin(), parca.begin() + okunan});
        // Gerçek projede: sikistirilmis veri API'ye veya DB'ye gönderilir
    }

    std::cout << "Toplam okunan : " << toplam_okunan << " byte\n";
    std::cout << "Parça sayısı  : " << parca_sayisi << "\n";
}

// ─── Ana Program ──────────────────────────────────────────
int main() {
    std::cout << "=== Büyük Veri Bellek Optimizasyonu ===\n\n";

    // Simüle edilmiş 1 MB'lık resim buffer'ı
    std::vector<uint8_t> resim_buffer(1024 * 1024);
    for (size_t i = 0; i < resim_buffer.size(); ++i)
        resim_buffer[i] = static_cast<uint8_t>(i % 256);

    // Sıkıştırma testi
    size_t ham_boyut = resim_buffer.size();
    auto sikistirilmis = sikistir(resim_buffer);
    size_t yeni_boyut = sikistirilmis.size();

    std::cout << "Ham boyut         : " << ham_boyut / 1024 << " KB\n";
    std::cout << "Sıkıştırılmış     : " << yeni_boyut / 1024 << " KB\n";
    std::cout << "Kazanım           : "
              << 100.0 * (1.0 - (double)yeni_boyut / ham_boyut)
              << " %\n\n";

    std::cout << "Stream işleme /proc/self/status ile test edilebilir.\n";
    return 0;
}
