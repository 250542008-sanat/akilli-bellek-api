// 1. Backend Adresini Tanımla
const API_URL = "https://akilli-bellek-yonetimi.onrender.com";

// MENÜ EKRANLARI ARASI GEÇİŞ YAPMA FONKSİYONU
function menuyuAc(ekranId) {
    document.getElementById("ekran-1").style.display = "none";
    document.getElementById("ekran-2").style.display = "none";
    document.getElementById("ekran-3").style.display = "none";
    document.getElementById("ekran-4").style.display = "none";

    document.getElementById(ekranId).style.display = "block";
}

// ANALİZİ BAŞLATMA FONKSİYONU (GERÇEK VERSİYON)
async function analiziBaslat() {
    const uygulamaAdi = document.getElementById("uygulama-adi").value || "Uygulama";

    // Analiz ekranına geç
    menuyuAc("ekran-2");

    document.getElementById("sonuc-baslik").innerText = "Analiz Sonucu: " + uygulamaAdi;
    const bugun = new Date();
    document.getElementById("sonuc-tarıh").innerText = "Tarih: " + bugun.toLocaleString('tr-TR');

    // 1. Aşama: Yükleniyor Animasyonu ve API Çağrısı
    document.getElementById("sizinti-miktari").innerText = "...";
    document.getElementById("uyari-1").innerText = "⚙️ Bulut sunucusuna bağlanılıyor...";
    document.getElementById("uyari-2").innerText = "🔍 Valgrind ve C++ motoru hazırlanıyor...";

    try {
        // RENDER'DAKİ BACKEND'E İSTEK AT
        const response = await fetch(`${API_URL}/api/v1/analiz/baslat`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ uygulama_adi: uygulamaAdi })
        });

        const data = await response.json();

        // 2. Aşama: Gelen Verileri Ekrana Yazdır
        if (response.ok) {
            // Backend'den (main.py) gelen verileri kullanıyoruz
            document.getElementById("sizinti-miktari").innerText = "8.2"; // Mock verimiz 8.2 MB
            document.getElementById("uyari-1").innerText = "✅ [BAŞARILI] " + data.mesaj;
            document.getElementById("uyari-2").innerText = "ℹ️ Oturum ID: " + data.oturum_id;
        } else {
            throw new Error("Sunucu hatası");
        }

    } catch (error) {
        // Hata Durumu
        document.getElementById("sizinti-miktari").innerText = "HATA";
        document.getElementById("uyari-1").innerText = "🔴 Sunucuya bağlanılamadı!";
        document.getElementById("uyari-2").innerText = "Lütfen Render sunucusunun 'Live' olduğundan emin olun.";
        console.error("Bağlantı Hatası:", error);
    }
}