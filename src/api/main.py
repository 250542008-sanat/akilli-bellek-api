import random
# ... (diğer importlar aynı)

@app.post("/api/v1/analiz/baslat")
def analiz_baslat(request: AnalyzeRequest):
    oturum_id = str(uuid4())
    # Her seferinde farklı gelsin diye 10.0 ile 50.0 arası sayı
    yeni_sizinti = round(random.uniform(10.0, 50.0), 1) 
    
    return {
        "durum": "baslatildi",
        "oturum_id": oturum_id,
        "sizinti_degeri": yeni_sizinti, # İsmi 'sizinti_degeri' yaptık (net olsun)
        "mesaj": f"{request.uygulama_adi} icin analiz baslatildi"
    }