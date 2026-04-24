#ifndef VERI_HAVUZU_H
#define VERI_HAVUZU_H

#include <iostream>
#include <vector>
#include <string>

// 1. Analiz Verisi Sınıfı
class AnalizVerisi {
public:
    int pid;
    double ramKullanimi;
    std::string durum;

    AnalizVerisi();
    void sifirla();
    void yazdir(const std::string& etiket = "") const;

    // Kopyalamayı engelleme
    AnalizVerisi(const AnalizVerisi&) = delete;
    AnalizVerisi& operator=(const AnalizVerisi&) = delete;
};

// 2. Nesne Havuzu Sınıfı
class VeriHavuzu {
public:
    explicit VeriHavuzu(std::size_t maksKapasite = 8);
    ~VeriHavuzu();

    AnalizVerisi* nesneAl();
    void nesneBirak(AnalizVerisi* nesne);
    void istatistikYazdir() const;

    // Kopyalamayı engelleme
    VeriHavuzu(const VeriHavuzu&) = delete;
    VeriHavuzu& operator=(const VeriHavuzu&) = delete;

private:
    std::vector<AnalizVerisi*> havuz_;
    std::size_t maksKapasite_;
    std::size_t toplamUretilen_;
    std::size_t havuzHiti_;
    std::size_t havuzKacirmasi_;
};

// 3. RAII Yardımcı Sınıfı
class HavuzKiralama {
public:
    explicit HavuzKiralama(VeriHavuzu& havuz);
    ~HavuzKiralama();
    AnalizVerisi* operator->();
    AnalizVerisi& operator*();

private:
    VeriHavuzu& havuz_;
    AnalizVerisi* nesne_;
};

#endif
