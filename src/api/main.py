import random
from fastapi import FastAPI
from pydantic import BaseModel
import uuid
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

# Vercel'den ve arayüzden gelen isteklere kapıyı açıyoruz (CORS Ayarı)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Güvenlik kapısını tamamen açar
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class AnalyzeRequest(BaseModel):
    uygulama_adi: str
    pid: int
    profilleme_modu: str
    sizinti_esigi: int

@app.post("/api/v1/analiz/baslat")
async def analiz_baslat(request: AnalyzeRequest):
    oturum_id = str(uuid.uuid4())
    # Her seferinde farklı gelsin diye 10.0 ile 50.0 arası sayı
    yeni_sizinti = round(random.uniform(10.0, 50.0), 1)

    return {
        "durum": "baslatildi",
        "oturum_id": oturum_id,
        "sizinti_degeri": yeni_sizinti,
        "mesaj": f"{request.uygulama_adi} icin analiz baslatildi"
    }

@app.get("/")
async def root():
    return {"mesaj": "Akilli Bellek Yonetimi API Calisiyor!"}