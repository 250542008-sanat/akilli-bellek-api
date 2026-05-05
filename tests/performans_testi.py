"""
Performans Testi - Akıllı Bellek Yönetimi API
RAM Kurtarıcıları | Hafta 6
Sorumlu: Sümeyra Adıyaman

Çalıştırma:
1. Önce API'yi başlat: uvicorn main:app --reload
2. Sonra bu scripti çalıştır: python tests/performans_testi.py
"""

import urllib.request
import urllib.error
import json
import time
import statistics

BASE_URL = "http://127.0.0.1:8000"

def istek_at(url, method="GET", data=None):
    """Tek bir HTTP isteği atar ve süreyi ölçer."""
    baslangic = time.time()
    try:
        if data:
            body = json.dumps(data).encode("utf-8")
            req = urllib.request.Request(
                url, data=body,
                headers={"Content-Type": "application/json"},
                method=method
            )
        else:
            req = urllib.request.Request(url, method=method)

        with urllib.request.urlopen(req, timeout=5) as yanit:
            yanit.read()
            sure = (time.time() - baslangic) * 1000
            return sure, yanit.status
    except urllib.error.URLError:
        return None, 0

def test_calistir(test_adi, url, tekrar=50, method="GET", data=None):
    """Bir endpoint'i birden fazla kez test eder."""
    print(f"\n{'='*50}")
    print(f"TEST: {test_adi}")
    print(f"URL : {url}")
    print(f"{'='*50}")

    sureler = []
    basarili = 0
    basarisiz = 0

    for i in range(tekrar):
        sure, status = istek_at(url, method, data)
        if sure is not None and status == 200:
            sureler.append(sure)
            basarili += 1
        else:
            basarisiz += 1

    if sureler:
        print(f"Toplam İstek  : {tekrar}")
        print(f"Başarılı      : {basarili}")
        print(f"Başarısız     : {basarisiz}")
        print(f"En Az Süre    : {min(sureler):.2f} ms")
        print(f"En Fazla Süre : {max(sureler):.2f} ms")
        print(f"Ortalama Süre : {statistics.mean(sureler):.2f} ms")
        print(f"Medyan Süre   : {statistics.median(sureler):.2f} ms")
        return statistics.mean(sureler)
    else:
        print("Tüm istekler başarısız! API çalışıyor mu?")
        return None

def main():
    print("\n" + "="*50)
    print("  PERFORMANS TESTİ BAŞLIYOR")
    print("  RAM Kurtarıcıları - Hafta 6")
    print("="*50)

    # Test 1: Sağlık kontrolü
    test_calistir(
        "Sistem Sağlık Kontrolü",
        f"{BASE_URL}/api/v1/sistem/saglik"
    )

    # Test 2: Bellek durumu
    test_calistir(
        "Bellek Durumu",
        f"{BASE_URL}/api/v1/sistem/bellek"
    )

    # Test 3: Profil endpoint
    test_calistir(
        "Profilleme Aracı",
        f"{BASE_URL}/api/v1/sistem/profil"
    )

    # Test 4: Analiz başlatma
    test_calistir(
        "Analiz Başlatma (POST)",
        f"{BASE_URL}/api/v1/analiz/baslat",
        tekrar=20,
        method="POST",
        data={"uygulama_adi": "test_app", "mod": "valgrind", "ornekleme_ms": 500}
    )

    # Test 5: Raporlar listesi
    test_calistir(
        "Raporlar Listesi",
        f"{BASE_URL}/api/v1/raporlar"
    )

    print("\n" + "="*50)
    print("  PERFORMANS TESTİ TAMAMLANDI")
    print("="*50)

if __name__ == "__main__":
    main()