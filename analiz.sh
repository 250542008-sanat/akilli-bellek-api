#!/bin/bash

# ========================================
#   Bellek Sızıntısı Analiz Betiği
#   Valgrind ile derinlemesine tarama
# ========================================

KAYNAK="main.cpp"
CIKTI="bellek_testi"
LOG="valgrind_raporu.txt"

echo ""
echo "========================================"
echo "  Bellek Sizintisi Analiz Araci"
echo "========================================"
echo ""

# Derleme
echo "[1/3] Derleniyor: $KAYNAK"
g++ -std=c++17 -g -Wall -Wextra -o "$CIKTI" "$KAYNAK"

if [ $? -ne 0 ]; then
    echo "[HATA] Derleme basarisiz. Cikiliyor."
    exit 1
fi

echo "[OK]  Derleme tamamlandi: ./$CIKTI"
echo ""

# Valgrind taraması
echo "[2/3] Valgrind ile tarama baslatiliyor..."
echo ""

valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file="$LOG" \
    ./"$CIKTI"

echo ""
echo "[3/3] Tarama tamamlandi."
echo ""

# Raporu ekrana yazdır
if [ -f "$LOG" ]; then
    echo "========================================"
    echo "  Valgrind Raporu: $LOG"
    echo "========================================"
    cat "$LOG"
else
    echo "[UYARI] Log dosyasi olusturulamadi."
fi

echo ""
echo "========================================"
echo "  Analiz tamamlandi."
echo "========================================"
echo ""
