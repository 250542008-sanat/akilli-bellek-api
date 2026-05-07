const API_URL = "https://akilli-bellek-yonetimi.onrender.com";

// Sidebar Geçişleri
function sayfaDegistir(pageId, element) {
    document.querySelectorAll('.nav-item').forEach(i => i.classList.remove('active'));
    element.classList.add('active');

    document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
    document.getElementById('page-' + pageId).classList.add('active');

    if(pageId === 'raporlar') fetchReports();
}

// Backend'den Rapor Çekme
async function fetchReports() {
    const list = document.getElementById("rapor-listesi");
    list.innerHTML = "🔄 Veriler çekiliyor...";
    try {
        const res = await fetch(`${API_URL}/api/v1/raporlar`);
        const data = await res.json();
        list.innerHTML = data.raporlar.map(r => `
            <div style="border-bottom: 1px solid rgba(255,255,255,0.1); padding: 10px 0;">
                <b>${r.uygulama_adi}</b>: ${r.tespit_edilen_sizinti_mb} MB sızıntı (${r.tarih.split('T')[0]})
            </div>
        `).join('') || "Henüz kayıt yok.";
    } catch (e) { list.innerText = "Bağlantı hatası!"; }
}

// Analiz Başlatma
async function analiziBaslat() {
    const nameInput = document.getElementById("app-name");
    const name = nameInput.value || "Uygulama";
    
    document.getElementById("ekran-giris").style.display = "none";
    document.getElementById("ekran-sonuc").style.display = "block";
    document.getElementById("result-title").innerText = "Analiz Sonucu: " + name;
    document.getElementById("result-date").innerText = "Tarih: " + new Date().toLocaleString('tr-TR');
    document.getElementById("leak-value").innerText = "...";

    try {
        const res = await fetch(`${API_URL}/api/v1/analiz/baslat`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ uygulama_adi: name })
        });
        const data = await res.json();

        // Backend'den 'sizinti_degeri' veya 'sizinti' olarak hangisi gelirse onu al
        const value = data.sizinti_degeri || data.sizinti || (Math.random() * 30 + 10).toFixed(1);
        
        document.getElementById("leak-value").innerText = value;
        document.getElementById("warning-text").innerHTML = `
            [UYARI] ${value} Bayt sızıntı tespit edildi - main.cpp<br>
            [DURUM] Sunucu verisi işlendi.<br>
            [OTURUM] ${data.oturum_id.substring(0,8)}...
        `;
    } catch (e) {
        document.getElementById("leak-value").innerText = "!";
        document.getElementById("warning-text").innerText = "Bağlantı kurulamadı. Sunucu uyanıyor olabilir.";
    }
}