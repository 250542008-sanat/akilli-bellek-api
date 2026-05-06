#include "VeriHavuzu.h"

// --- Durum yardimcisi ---
// string_view dondurur: heap tahsisi yok, kopyalama yok
std::string_view durumStr(Durum d)
{
    switch (d)
    {
    case Durum::Bos:
        return "Bos";
    case Durum::Calisiyor:
        return "Calisiyor";
    case Durum::Tamamlandi:
        return "Tamamlandi";
    case Durum::Hata:
        return "Hata";
    }
    return "?";
}

// --- AnalizVerisi Metotlari ---
AnalizVerisi::AnalizVerisi()
    : pid(0), ramKullanimi(0.0), durum(Durum::Bos)
{
    // Eski: durum("") → std::string varsayilan kurucusu cagiriliyordu
    // Yeni: durum(Durum::Bos) → enum atamasi, heap tahsisi yok
}

void AnalizVerisi::sifirla()
{
    pid = 0;
    ramKullanimi = 0.0;
    durum = Durum::Bos;
    // Eski: durum.clear() → std::string bellek islemine girebilirdi
    // Yeni: enum atamasi, O(1) islem, sifir bellek maliyeti
}

// Degisiklik: const std::string& → std::string_view
//   Ornek fayda: yazdir("Iterasyon_0") cagrisi eskiden
//   gecici bir std::string olusturabilirdi; artik olusturmuyor.
void AnalizVerisi::yazdir(std::string_view etiket) const
{
    if (!etiket.empty())
        std::cout << "[" << etiket << "] ";
    std::cout << "PID=" << pid
              << " | RAM=" << ramKullanimi << " MB"
              << " | Durum=\"" << durumStr(durum) << "\"\n";
}

// --- VeriHavuzu Metotlari ---
VeriHavuzu::VeriHavuzu(std::size_t maksKapasite)
    : maksKapasite_(maksKapasite)
{
    havuz_.reserve(maksKapasite_);
    // ist_ uyeleri HavuzIstatistik tarafindan sifirlanir (= 0 varsayilan)
}

// Degisiklik: AnalizVerisi* donduruyordu → unique_ptr dondurur
//   Sahiplik cagirana devredilir; cagiran "birak"i cagirmazsa
//   unique_ptr yine de bellegi serbest birakirsizinti imkansiz.
std::unique_ptr<AnalizVerisi> VeriHavuzu::nesneAl()
{
    if (!havuz_.empty())
    {
        // std::move ile sahipligi vektorden cagirana tar - kopyalama yok
        auto nesne = std::move(havuz_.back());
        havuz_.pop_back();
        ++ist_.havuzHiti;
        return nesne;
    }
    ++ist_.toplamUretilen;
    ++ist_.havuzKacirmasi;
    return std::make_unique<AnalizVerisi>();
    // Eski: return new AnalizVerisi();
    // Yeni: make_unique — tek tahsis, exception-safe, sahiplik net
}

// Degisiklik: raw pointer → unique_ptr (move ile sahiplik devralınır)
void VeriHavuzu::nesneBirak(std::unique_ptr<AnalizVerisi> nesne)
{
    if (!nesne)
        return;
    nesne->sifirla();
    if (havuz_.size() < maksKapasite_)
    {
        havuz_.push_back(std::move(nesne)); // sahiplik havuza geciyor
    }
    // else: unique_ptr scope'tan cikinca otomatik delete — eski kodda
    //       "delete nesne;" satirinin unutulmasi leak'e yol acabilirdi.
}

void VeriHavuzu::istatistikYazdir() const
{
    std::cout << "\n===== HAVUZ ISTATISTIKLERI =====\n"
              << "  Toplam uretilen: " << ist_.toplamUretilen << "\n"
              << "  Havuz isabeti  : " << ist_.havuzHiti << "\n"
              << "  Havuz kacirmasi: " << ist_.havuzKacirmasi << "\n"
              << "================================\n";
}

// Yikici kaldirildi: = default yeterli.
// unique_ptr'lar destructor'da otomatik silinir; eski kod icin
// "for (AnalizVerisi* p : havuz_) delete p;" satirini yazmak zorunluydu.

// --- HavuzKiralama Metotlari ---
HavuzKiralama::HavuzKiralama(VeriHavuzu &havuz)
    : havuz_(havuz), nesne_(havuz.nesneAl()) {}

HavuzKiralama::~HavuzKiralama()
{
    if (nesne_)
        havuz_.nesneBirak(std::move(nesne_));
}

AnalizVerisi *HavuzKiralama::operator->() const { return nesne_.get(); }
AnalizVerisi &HavuzKiralama::operator*() const { return *nesne_; }