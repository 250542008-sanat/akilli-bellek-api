#ifndef VERI_HAVUZU_H
#define VERI_HAVUZU_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>      // std::unique_ptr, std::make_unique
#include <string_view> // std::string_view - kopyasiz parametre gecisi

// 1. Durum enum sinifi: std::string yerine 1 baytlik sabit tip
//    Eski: std::string durum = "Calisiyor"  → ~32 bayt (SSO dahil heap riski)
//    Yeni: Durum durum = Durum::Calisiyor   →   1 bayt (enum class uint8_t)
enum class Durum : uint8_t
{
    Bos,
    Calisiyor,
    Tamamlandi,
    Hata
};

// Durum degerini okunabilir metne donusturucu (heap tahsisi yok)
std::string_view durumStr(Durum d);

// 2. Analiz Verisi Sinifi
//    Degisiklik: std::string durum → Durum durum (struct boyutu ~32 bayt küçüldü)
class AnalizVerisi
{
public:
    int pid;
    double ramKullanimi;
    Durum durum;

    AnalizVerisi();
    void sifirla();

    // Degisiklik: const std::string& → std::string_view
    //   Faydasi: string literal verildiginde gecici string nesnesi olusturmaz,
    //            cagiran tarafta kopyalama maliyeti sifira iner.
    void yazdir(std::string_view etiket = "") const;

    // Kopyalama engelli, tasima serbest (unique_ptr ile uyumlu)
    AnalizVerisi(const AnalizVerisi &) = delete;
    AnalizVerisi &operator=(const AnalizVerisi &) = delete;
    AnalizVerisi(AnalizVerisi &&) noexcept = default;
    AnalizVerisi &operator=(AnalizVerisi &&) noexcept = default;
};

// 3. Istatistik yapisi - veri yerelligi icin tek struct'ta gruplandirma
//    Eski: 3 ayri std::size_t uye (dagik) | Yeni: tek struct (cache-friendly)
struct HavuzIstatistik
{
    std::size_t toplamUretilen = 0;
    std::size_t havuzHiti = 0;
    std::size_t havuzKacirmasi = 0;
};

// 4. Nesne Havuzu Sinifi
//    Degisiklik: vector<AnalizVerisi*> → vector<unique_ptr<AnalizVerisi>>
//      • Manuel delete dongusu kaldirildi (~destructor = default)
//      • nesneAl/nesneBirak sahiplik transferini move ile yapıyor
class VeriHavuzu
{
public:
    explicit VeriHavuzu(std::size_t maksKapasite = 8);
    ~VeriHavuzu() = default; // unique_ptr'lar otomatik temizler

    // Degisiklik: AnalizVerisi* → std::unique_ptr<AnalizVerisi>
    //   Sahiplik cagirana devredilir; cagiran unutsa bile unique_ptr sizer eder.
    std::unique_ptr<AnalizVerisi> nesneAl();

    // Degisiklik: raw pointer → unique_ptr (move ile sahiplik geri alinir)
    void nesneBirak(std::unique_ptr<AnalizVerisi> nesne);

    void istatistikYazdir() const;

    // Kopyalama engelli
    VeriHavuzu(const VeriHavuzu &) = delete;
    VeriHavuzu &operator=(const VeriHavuzu &) = delete;

private:
    std::vector<std::unique_ptr<AnalizVerisi>> havuz_; // ham pointer yok
    std::size_t maksKapasite_;
    HavuzIstatistik ist_; // gruplu istatistik
};

// 5. RAII Yardimci Sinifi
//    Degisiklik: iceride AnalizVerisi* → std::unique_ptr<AnalizVerisi>
//      Sonuc: HavuzKiralama scope'tan cikinca otomatik nesneBirak cagirilir,
//             kullanici unuttugunda dahi sizinti olmaz.
class HavuzKiralama
{
public:
    explicit HavuzKiralama(VeriHavuzu &havuz);
    ~HavuzKiralama();

    AnalizVerisi *operator->() const;
    AnalizVerisi &operator*() const;

    // Kopyalama engelli, tasima serbest
    HavuzKiralama(const HavuzKiralama &) = delete;
    HavuzKiralama &operator=(const HavuzKiralama &) = delete;

private:
    VeriHavuzu &havuz_;
    std::unique_ptr<AnalizVerisi> nesne_;
};

#endif