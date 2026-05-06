/**
 * VeriHavuzu - Optimizasyon Sonrasi Kalici Test
 * Akıllı Bellek Yönetim Sistemi | Semanur Buhan | Hafta 5
 *
 * Derleme: g++ -std=c++17 -o test_veri tests/veri_havuzu_test.cpp backend/VeriHavuzu.cpp
 * Valgrind: valgrind --leak-check=full --show-leak-kinds=all ./test_veri
 */

#include "../backend/VeriHavuzu.h"
#include <iostream>
#include <string>

int main()
{
    std::cout << "=== VeriHavuzu Optimizasyon Sonrasi Test ===\n";
    std::cout << "AnalizVerisi boyutu: " << sizeof(AnalizVerisi) << " bayt\n\n";

    VeriHavuzu havuz(4);

    // Test 1: unique_ptr tabanli al/birak
    for (int i = 0; i < 6; ++i)
    {
        auto v = havuz.nesneAl();
        v->pid = i + 1;
        v->ramKullanimi = 100.0 + i * 15.5;
        v->durum = Durum::Calisiyor;
        v->yazdir("Iterasyon_" + std::to_string(i));
        havuz.nesneBirak(std::move(v));
    }

    // Test 2: RAII kiralama
    {
        HavuzKiralama kiralama(havuz);
        kiralama->pid = 99;
        kiralama->ramKullanimi = 512.0;
        kiralama->durum = Durum::Tamamlandi;
        kiralama->yazdir("RAII-Kiralama");
    }

    // Test 3: Durum donusturucusu
    std::cout << "\nDurum listesi: ";
    for (auto d : {Durum::Bos, Durum::Calisiyor, Durum::Tamamlandi, Durum::Hata})
        std::cout << durumStr(d) << " ";
    std::cout << "\n";

    havuz.istatistikYazdir();
    std::cout << "=== Test Tamamlandi ===\n";
    return 0;
}