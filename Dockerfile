# 1. Temel İşletim Sistemi (İçinde Python yüklü hafif bir Linux)
FROM python:3.10-slim

# 2. C++ derleyicisi (g++) ve Valgrind gibi kritik Linux araçlarını kur
RUN apt-get update && apt-get install -y \
    g++ \
    valgrind \
    make \
    && rm -rf /var/lib/apt/lists/*

# 3. Çalışma klasörümüzü /app olarak belirle
WORKDIR /app

# 4. Projedeki tüm dosyaları buluttaki bu klasörün içine kopyala
COPY . /app

# 5. Python kütüphanelerini (FastAPI vb.) kur
RUN pip install --no-cache-dir -r requirements.txt

# 6. Dışarıya açılacak port (Render genelde 8000 veya kendi portunu kullanır)
EXPOSE 8000

# 7. Motoru (Backend) Çalıştır!
CMD ["uvicorn", "src.api.main:app", "--host", "0.0.0.0", "--port", "8000"]