#!/bin/bash

# RAM Kurtarıcıları - Otomatik Valgrind Analiz Motoru
# Sorumlu: Semanur Buhan

GIRDI_DOSYASI=$1
CIKTI_YURUTULEBILIR="analiz_edilecek_uygulama"
RAPOR_DOSYASI="valgrind_raporu.log"

if [ -z "$GIRDI_DOSYASI" ]; then
    echo "Hata: Lütfen analiz edilecek C++ dosyasını belirtin."
    echo "Kullanım: ./valgrind_analiz.sh <dosya_adi.cpp>"
    exit 1
fi

echo "=========================================="
echo "🚀 RAM Kurtarıcıları Valgrind Entegrasyonu Başlatılıyor..."
echo "Derlenen Dosya: $GIRDI_DOSYASI"
echo "=========================================="

# C++ kodunu derle
g++ -o $CIKTI_YURUTULEBILIR $GIRDI_DOSYASI backend/VeriHavuzu.cpp

if [ $? -ne 0 ]; then
    echo "❌ Derleme Hatası! Lütfen kontrol edin."
    exit 1
fi

echo "✅ Derleme Başarılı. Bellek Analizi Yapılıyor..."

# Valgrind'i çalıştır ve log dosyasına yazdır
valgrind --leak-check=full --show-leak-kinds=all --log-file=$RAPOR_DOSYASI ./$CIKTI_YURUTULEBILIR > /dev/null

echo "=========================================="
echo "✅ Analiz Tamamlandı!"
echo "Detaylı rapor '$RAPOR_DOSYASI' dosyasına kaydedildi."
echo "=========================================="