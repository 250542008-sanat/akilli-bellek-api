from fastapi import FastAPI
from pydantic import BaseModel
from uuid import uuid4
from datetime import datetime

app = FastAPI(
    title="Akıllı Bellek Yönetimi API",
    description="RAM Kurtarıcıları - Mock Prototip v1.0",
    version="1.0"
)

sessions = {}

class AnalyzeRequest(BaseModel):
    uygulama_adi: str
    pid: int | None = None
    mod: str = "valgrind"
    ornekleme_ms: int = 500

@app.get("/")
def root():
    return {"mesaj": "API calisiyor", "dokumantasyon": "/docs"}

@app.post("/api/v1/analiz/baslat")
def analiz_baslat(request: AnalyzeRequest):
    oturum_id = str(uuid4())
    sessions[oturum_id] = {
        "uygulama_adi": request.uygulama_adi,
        "durum": "devam_ediyor",
        "anlik_ram_mb": 120.5,
        "toplam_sizinti_mb": 8.2,
        "baslangic_zamani": datetime.now().isoformat()
    }
    return {
        "durum": "baslatildi",
        "oturum_id": oturum_id,
        "mesaj": f"{request.uygulama_adi} icin analiz baslatildi"
    }

@app.get("/api/v1/analiz/durum/{oturum_id}")
def analiz_durum(oturum_id: str):
    if oturum_id not in sessions:
        return {"hata": "oturum_bulunamadi"}
    return {
        "oturum_id": oturum_id,
        **sessions[oturum_id]
    }

@app.get("/api/v1/raporlar")
def raporlar():
    return {
        "toplam": 2,
        "raporlar": [
            {
                "id": 1,
                "uygulama_adi": "test_app_a",
                "tarih": "2026-03-20T10:00:00",
                "tespit_edilen_sizinti_mb": 0.0,
                "uyari_sayisi": 0
            },
            {
                "id": 2,
                "uygulama_adi": "test_app_b",
                "tarih": "2026-03-21T14:30:00",
                "tespit_edilen_sizinti_mb": 22.1,
                "uyari_sayisi": 7
            }
        ]
    }

@app.get("/api/v1/sistem/saglik")
def sistem_saglik():
    return {
        "api": "calisiyor",
        "valgrind": "mock",
        "sqlite": "mock",
        "proc_fs": "mock",
        "surum": "1.0.0",
        "zaman": datetime.now().isoformat()
    }