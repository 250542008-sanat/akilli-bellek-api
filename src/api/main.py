import random
import uuid
import psycopg2
from psycopg2.extras import RealDictCursor
from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ⚠️ DİKKAT: BURAYA KENDİ KOPYALADIĞIN DATABASE_PUBLIC_URL LİNKİNİ YAPIŞTIR
DB_URL = "postgresql://postgres:vocfxumGYzTwPVaUKvfaLrnxgYVanzUe@switchback.proxy.rlwy.net:21126/railway"

# 1. Veritabanı ve tablo kurulumu (Uygulama açılırken otomatik çalışır)
def veritabani_tablosu_olustur():
    try:
        conn = psycopg2.connect(DB_URL)
        cursor = conn.cursor()
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS analiz_gecmisi (
                id SERIAL PRIMARY KEY,
                uygulama_adi VARCHAR(100),
                pid INT,
                sizan_bellek FLOAT,
                tarih TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        """)
        conn.commit()
        cursor.close()
        conn.close()
        print("✅ Veritabanı tablosu başarıyla kuruldu veya zaten mevcut!")
    except Exception as e:
        print("❌ Veritabanı bağlantı hatası:", e)

veritabani_tablosu_olustur()


# 2. Analizi Başlat ve Veritabanına Kaydet
@app.post("/analiz/baslat")
async def analiz_baslat(request: Request):
    data = await request.json()
    uygulama = data.get("uygulama_adi", "Bilinmeyen")
    pid = data.get("pid", 0) # Frontend'den PID gelmezse 0 olarak kaydet
    
    oturum_id = str(uuid.uuid4())
    yeni_sizinti = round(random.uniform(10.0, 50.0), 1)

    # ---> VERİTABANINA YAZMA İŞLEMİ <---
    try:
        conn = psycopg2.connect(DB_URL)
        cursor = conn.cursor()
        cursor.execute(
            "INSERT INTO analiz_gecmisi (uygulama_adi, pid, sizan_bellek) VALUES (%s, %s, %s)",
            (uygulama, int(pid), yeni_sizinti)
        )
        conn.commit()
        cursor.close()
        conn.close()
        print(f"💾 {yeni_sizinti} KB veritabanına kaydedildi!")
    except Exception as e:
        print("❌ Veri kaydedilemedi:", e)

    return {
        "durum": "baslatildi",
        "oturum_id": oturum_id,
        "sizinti_degeri": yeni_sizinti,
        "mesaj": f"{uygulama} icin analiz baslatildi"
    }


# 3. YENİ ÖZELLİK: Geçmiş Kayıtları Getir
@app.get("/gecmis")
async def gecmis_analizler():
    try:
        conn = psycopg2.connect(DB_URL)
        cursor = conn.cursor(cursor_factory=RealDictCursor)
        # Son 20 analizi yeninden eskiye doğru getir
        cursor.execute("SELECT * FROM analiz_gecmisi ORDER BY id DESC LIMIT 20")
        kayitlar = cursor.fetchall()
        cursor.close()
        conn.close()
        return {"durum": "basarili", "kayitlar": kayitlar}
    except Exception as e:
        return {"durum": "hata", "mesaj": str(e)}


@app.get("/")
async def root():
    return {"mesaj": "API Calisiyor ve Veritabanina Bagli! 🚀"}