import random
from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
import uuid

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.post("/analiz/baslat")
async def analiz_baslat(request: Request):
    # Gelen veriyi ham olarak alıyoruz, böylece 422 hatası vermeyecek
    data = await request.json()
    uygulama = data.get("uygulama_adi", "Bilinmeyen")
    
    oturum_id = str(uuid.uuid4())
    yeni_sizinti = round(random.uniform(10.0, 50.0), 1)

    return {
        "durum": "baslatildi",
        "oturum_id": oturum_id,
        "sizinti_degeri": yeni_sizinti,
        "mesaj": f"{uygulama} icin analiz baslatildi"
    }

@app.get("/")
async def root():
    return {"mesaj": "API Calisiyor!"}