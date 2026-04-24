#include "VeriHavuzu.h"

// --- AnalizVerisi Metotları ---
AnalizVerisi::AnalizVerisi() : pid(0), ramKullanimi(0.0), durum("") {
    std::cout << "[SISTEM] Yeni AnalizVerisi nesnesi olusturuldu.\n";
}

void AnalizVerisi::sifirla() {
    pid = 0; ramKullanimi = 0.0; durum.clear();
}

void AnalizVerisi::yazdir(const std::string& etiket) const {
    if (!etiket.empty()) std::cout << "[" << etiket << "] ";
    std::cout << "PID=" << pid << " | RAM=" << ramKullanimi << " MB | Durum=\"" << durum << "\"\n";
}

// --- VeriHavuzu Metotları ---
VeriHavuzu::VeriHavuzu(std::size_t maksKapasite) 
    : maksKapasite_(maksKapasite), toplamUretilen_(0), havuzHiti_(0), havuzKacirmasi_(0) {
    havuz_.reserve(maksKapasite_);
}

AnalizVerisi* VeriHavuzu::nesneAl() {
    if (!havuz_.empty()) {
        AnalizVerisi* nesne = havuz_.back();
        havuz_.pop_back();
        ++havuzHiti_;
        return nesne;
    }
    ++toplamUretilen_; ++havuzKacirmasi_;
    return new AnalizVerisi();
}

void VeriHavuzu::nesneBirak(AnalizVerisi* nesne) {
    if (!nesne) return;
    nesne->sifirla();
    if (havuz_.size() < maksKapasite_) {
        havuz_.push_back(nesne);
    } else {
        delete nesne;
    }
}

void VeriHavuzu::istatistikYazdir() const {
    std::cout << "\n===== HAVUZ ISTATISTIKLERI =====\n"
              << "  Toplam uretilen: " << toplamUretilen_ << "\n"
              << "  Havuz isabeti  : " << havuzHiti_ << "\n"
              << "================================\n";
}

VeriHavuzu::~VeriHavuzu() {
    for (AnalizVerisi* p : havuz_) delete p;
    havuz_.clear();
}

// --- HavuzKiralama Metotları ---
HavuzKiralama::HavuzKiralama(VeriHavuzu& havuz) : havuz_(havuz), nesne_(havuz.nesneAl()) {}
HavuzKiralama::~HavuzKiralama() { if (nesne_) havuz_.nesneBirak(nesne_); }
AnalizVerisi* HavuzKiralama::operator->() { return nesne_; }
AnalizVerisi& HavuzKiralama::operator*() { return *nesne_; }
