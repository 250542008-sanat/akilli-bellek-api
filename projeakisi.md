# 1. HAFTA ÇALIŞMALARI (6 Mart - 12 Mart 2026)
---

## 1.1 Proje Analizi ve Kapsam Tanımlama

**Sorumlu:** Mustafa Şahingöz

**1. Projenin Temel Hedefleri**

* Elde edilen teknik verileri yapılandırılmış bir formatta işleyerek arayüz üzerinden kullanıcıya raporlamak.
* Arka planda çalışan analiz motorunun ürettiği karmaşık terminal çıktılarını, Python tabanlı kullanıcı dostu bir grafik arayüz (GUI) ile görselleştirerek (grafikler, metrikler ve uyarılar halinde) raporlamak.

**2. Proje Kapsamı**

**Kapsam İçi:** Hedef uygulamanın Linux ortamında bellek yönetim araçlarıyla (Valgrind) çalıştırılması, anlık/periyodik bellek tüketim verilerinin toplanması ve bu verilerin Python arayüzünde anlamlı raporlara dönüştürülmesi.
* **Kapsam Dışı:** Tespit edilen bellek sızıntılarının sistem tarafından otomatik olarak düzeltilmesi veya kaynak kodun değiştirilmesi (Uygulama yalnızca tespit ve raporlama yapacaktır). Ayrıca uygulamanın ilk sürümü Linux/WSL ortamına odaklanacak olup, doğrudan Windows (native) desteği kapsam dışıdır.

**3. Potansiyel Zorluklar ve Risk Analizi**

* **Performans Yükü (Overhead):** Dinamik analiz araçlarının (özellikle Valgrind'in) hedef programın çalışma hızını yavaşlatması ve sistem kaynaklarını fazladan tüketmesi.
* **Veri Ayrıştırma (Parsing) Zorluğu:** Valgrind'in ürettiği standart dışı, karmaşık metin çıktıların Python tarafına aktarılırken veri kaybı yaşanmadan hatasız bir şekilde ayrıştırılması.
* **Sistemler Arası İletişim:** Veriyi toplayan arka uç (C++) ile veriyi gösteren ön uç (Python) arasındaki iletişimin sorunsuz, düşük gecikmeli ve kopmadan sağlanması.

**4. Mevcut Bellek Optimizasyonu Yaklaşımları ve Araç Araştırması**

* **Manuel Yaklaşımlar:** Geliştiricilerin bellek tahsislerini (`malloc/free` veya `new/delete`) manuel loglamasıdır. İnsan hatasına açıktır ve büyük projelerde sürdürülebilir değildir.
## 1.2 🛡️ Bellek Optimizasyonu İçin Potansiyel Teknolojiler ve Stratejik Seçim Raporu

**Proje Adı:** Akıllı Bellek Yönetim Uygulaması  
**Ekip:** RAM Kurtarıcıları  
**Hazırlayan:** Yusuf Tuğra Deveci  
**Tarih:** 6 Mart 2026  

---

## 🎯 1. GİRİŞ VE ARAŞTIRMA KAPSAMI
Bu rapor, **"RAM Kurtarıcıları"** ekibinin geliştireceği uygulamanın teknolojik altyapısını belirlemek amacıyla hazırlanmıştır. Projemizin temel odağı; sistem kaynaklarını en az seviyede tüketen (*minimum memory footprint*) ve anlık tepki verebilen yüksek performanslı bir bellek yönetim aracı inşa etmektir.

---

## 🛠️ 2. SEÇİLEN TEKNOLOJİ YIĞINI (TECH STACK) VE ANALİZİ

### 2.1. Sistem Motoru (Backend): C++ ve Rust Karşılaştırması
Araştırmalarımız sonucunda projenin çekirdek motoru için **C++** dili seçilmiştir.

* **Neden C++?:** C++, donanım seviyesinde doğrudan bellek yönetimi (`malloc`/`free`) yapmamıza olanak tanıyan en güçlü dildir.
* **"Milimetrik" Optimizasyon:** Belleğin her bir byte'ını manuel olarak kontrol etme imkanını sadece bu seviyedeki diller sunar.
* **Neden Rust Değil?:** Rust güvenli bir alternatif olsa da, projemizin "bellek üzerinde tam hakimiyet" ve "geleneksel sistem araçlarıyla uyum" hedefi için C++'ın esnekliği daha uygun görülmüştür.

### 2.2. Kullanıcı Paneli (Frontend): Python Hibrit Modeli
Kullanıcının sistemi izleyeceği arayüz için **Python** tercih edilmiştir.
> **Stratejik Sebep:** Bellek yönetimi gibi ağır işleri C++ motoru yaparken, bu verileri görselleştirmek için Python'ın zengin grafik kütüphanelerinden faydalanılacaktır.

### 2.3. Veri Kaynağı: Linux /proc Sanal Dosya Sistemi

Sistem verilerini toplamak için dış kütüphaneler yerine Linux çekirdeğinin (kernel) sunduğu `/proc` sistemi kullanılacaktır.

* **Teknik Üstünlük:** `/proc/meminfo` gibi dosyalar diskte yer kaplamayan, doğrudan RAM'deki canlı veriyi yansıtan sanal dosyalardır. Bu dosyaları okuyarak sistemin o anki "gerçek röntgenini" en hızlı haliyle çekmiş olacağız.
* **Çapraz Platform (Cross-Platform) Uyumluluğu:** `/proc` mimarisi yalnızca Linux çekirdeğine özgü olsa da, projenin Windows ve macOS gibi farklı işletim sistemlerinde de çalışabilmesi güvence altına alınmıştır. Bu uyumluluğu sağlamak için; C++ tabanlı arka uç (backend) kodları Windows ortamında **WSL2**, macOS ortamında ise **sanallaştırma (Docker/VM)** teknolojileri kullanılarak izole bir Linux çekirdeğinde çalıştırılacaktır. Python tabanlı kullanıcı arayüzü (frontend) ise işletim sisteminden bağımsız olarak çalışarak bu verileri ekrana yansıtacaktır.

### 2.4. Denetim Aracı: Valgrind (Memcheck)
Yazılımın kalitesini garanti altına almak için **Valgrind** analiz aracı kullanılacaktır.
* **Kritik Görev:** Manuel bellek yönetiminde oluşabilecek en küçük **bellek sızıntılarını** (*memory leaks*) bile tespit ederek, programın RAM tüketiminin sabit kalmasını sağlayacaktır.

---

## 🚫 3. DİĞER TEKNOLOJİLERİN ELENME GEREKÇELERİ

### 3.1. Java ve .NET Ekosisteminin Riskleri
Java ve C# gibi diller, kendi çalışma ortamları (*Virtual Machine*) için başlangıçta yüksek miktarda RAM tüketirler. Ayrıca **Garbage Collector** (Çöp Toplayıcı) mekanizması, sistemi öngörülemez zamanlarda durdurabilir.

### 3.2. Saf Python Yaklaşımının Verimsizliği
Eğer tüm projeyi Python ile yazsaydık, dilin kendi iç yapısı nedeniyle basit bir tam sayıyı saklamak için bile C++'a göre **5-10 kat daha fazla** yer harcardık. Bu durum, projemizin "bellek kurtarma" misyonuyla çelişmektedir.

---

## 🏁 4. SONUÇ
Seçilen **C++ tabanlı backend** ve **Python tabanlı frontend** mimarisi, projemizin hem profesyonel bir sistem aracı gibi çalışmasını hem de modern bir kullanıcı arayüzüne sahip olmasını sağlayacaktır. 

> *Sistem kaynaklarını sömüren değil, onları özgürleştiren bir yazılımın temeli atılmıştır.*

---

---

## 1.3 👩‍💻 Sümeyra Adıyaman – Gereksinim Toplama ve Paydaş Analizi

### Görev Tanımı
Bu görev kapsamında proje paydaşları belirlenmiş, paydaşların ihtiyaçları analiz edilmiş ve bu ihtiyaçlara göre sistem gereksinimleri oluşturulmuştur. Ayrıca belirlenen gereksinimler önem derecelerine göre önceliklendirilmiştir.

---

## Paydaşlar

Projeden etkilenen veya projeyi kullanacak kişiler aşağıdaki şekilde belirlenmiştir:

- Yazılım geliştiriciler
- Sistem yöneticileri
- Son kullanıcılar
- Proje yöneticileri

---

## Paydaş İhtiyaçları

### Yazılım Geliştiriciler
- Programların bellek kullanımını analiz edebilmek
- Bellek sızıntılarını tespit edebilmek
- Kod performansını artırmak için analiz sonuçlarını inceleyebilmek

### Sistem Yöneticileri
- Sunucularda çalışan uygulamaların RAM kullanımını izlemek
- Gereksiz bellek tüketimini azaltmak
- Sistem performansını artırmak

### Son Kullanıcılar
- Daha hızlı çalışan uygulamalar kullanmak
- Uygulamaların stabil çalışması

### Proje Yöneticileri
- Sistem kaynaklarının daha verimli kullanılmasını sağlamak
- Performans problemlerini azaltmak

---

## Fonksiyonel Gereksinimler

- Sistem çalışan uygulamaların RAM kullanımını analiz edebilmelidir.
- Sistem bellek sızıntılarını tespit edebilmelidir.
- Sistem analiz sonuçlarını kullanıcıya rapor halinde sunabilmelidir.
- Sistem gereksiz bellek kullanımını tespit edebilmelidir.
- Sistem geliştircilere bellek kullanımını iyileştirmeye yönelik öneriler sunulabilir.

---

## Fonksiyonel Olmayan Gereksinimler

- Sistem Linux ve Linux tabanlı ortamlar (WSL vb.) üzerinde çalışacak şekilde tasarlanmalıdır.
- Sistem kullanıcı dostu olmalıdır.
- Analiz işlemleri mümkün olduğunca hızlı gerçekleştirilmelidir.
- Sistem büyük uygulamalarda da analiz yapabilecek kapasitede olmalıdır.

---

## Gereksinim Önceliklendirme

### Must Have (Olmazsa Olmaz)
- RAM kullanım analizi
- Bellek sızıntısı tespiti
- Analiz sonuçlarının raporlanması

### Should Have (Önemli)
- Bellek optimizasyonu önerileri
- Detaylı analiz çıktıları

### Could Have (Ek Özellik)
- Grafiksel raporlama
- Kullanıcı dostu görsel arayüz


## 1.4 Geliştirme Ortamı Yapılandırma Raporu

**Sorumlu:** Semanur Buhan

### 1. Özet ve Amaç
Akıllı Bellek Yönetimi Uygulaması projesinin teknik gereksinimleri doğrultusunda, geliştirme süreçlerinin sağlıklı yürütülebilmesi için gerekli olan araç zinciri (toolchain) ve çalışma ortamı yapılandırılmıştır. Bu rapor, kurulumu yapılan sistem bileşenlerini ve doğrulama adımlarını içermektedir.

### 2. Teknik Altyapı ve Kurulumlar

#### 2.1. İşletim Sistemi Katmanı (Linux/WSL)
Uygulamanın bellek analizi süreçlerinde Linux çekirdeğine ait `/proc` sanal dosya sistemini kullanacak olması nedeniyle, geliştirme ortamı olarak **WSL2 (Windows Subsystem for Linux)** üzerine kurulu **Ubuntu** dağıtımı tercih edilmiş ve kurulumu tamamlanmıştır.

#### 2.2. C++ Geliştirme ve Analiz Araçları
Arka uç (backend) motorunun geliştirilmesi için gerekli araçlar sisteme dahil edilmiştir:
* **Derleyici (GCC/G++):** C++ kodlarının derlenmesi için `build-essential` paketi kuruldu.
* **Dinamik Analiz (Valgrind):** Bellek sızıntılarının tespiti ve analizi için Valgrind araç seti yapılandırıldı.
* **Hata Ayıklayıcı (GDB):** Çalışma zamanı hatalarının tespiti için debugger entegrasyonu sağlandı.

#### 2.3. Python ve Arayüz Çalışma Alanı
Ön yüz (frontend) ve raporlama sistemleri için gerekli Python altyapısı kurulmuştur:
* Python 3.x güncel sürüm kurulumu doğrulandı.
* Modüller arası çakışmaları önlemek adına izole sanal ortam (`venv`) yapısı hazırlandı.

#### 2.4. IDE ve Geliştirici Eklentileri
Geliştirme arayüzü olarak **VS Code** üzerinde aşağıdaki konfigürasyonlar yapılmıştır:
* **WSL Remote Extension:** Linux dosya sistemine doğrudan erişim sağlandı.
* **C/C++ & Python Extension Pack:** Akıllı kod tamamlama ve derleme desteği aktif edildi.

### 3. Doğrulama ve Test
Yapılan kurulumlar terminal üzerinden test edilerek derleyici ve analiz araçlarının (Valgrind, G++ ve Python) sorunsuz çalıştığı teyit edilmiştir. Geliştirme ortamı, projenin tasarım ve kodlama aşamaları için hazır durumdadır.


# Proje Akışı

## Hafta 1

### Mustafa Şahingöz
Porje analizi ve kapsam tanımlama yapıldı.

### Yusuf Tuğra Deveci
Bellek optimizasyonu için teknolojiler araştırıldı.

### Sümeyra Adıyaman
Paydaş analizi ve gereksinimler belirlendi.

### Semanur Buhan
Geliştirme ortamı yapılandırılması ve kurulumu tamamlandı.


# 2. HAFTA ÇALIŞMALARI (12 Mart - 15 Mart 2026)
---
## 2.1 Test Mimarisi, Simülasyon Senaryoları ve Sprint Planlaması

**Sorumlu:** Mustafa Şahingöz 

**1. Simülasyon ve "Uygulama Sahası" Stratejisi**
Geliştirilecek olan "Akıllı Bellek Yönetimi" aracının (Valgrind ve `/proc` motoru) test edilebilmesi ve yeteneklerinin kanıtlanabilmesi için dış bir projeye ihtiyaç duyulmadan kendi **"Simülasyon Ortamımız"** tasarlanmıştır. Bu kapsamda analiz motoruna "kobay" olarak verilmek üzere iki farklı C++ simülasyon uygulaması geliştirilecektir:

* **Senaryo A (Sağlıklı Simülasyon):** Bellek tahsislerinin (`malloc`/`new`) ve iadelerinin (`free`/`delete`) kusursuz yapıldığı, bellek sızıntısı (memory leak) içermeyen ve stabil RAM tüketen referans uygulama.
* **Senaryo B (Hatalı/Kanserli Simülasyon):** Sistemin hata yakalama kapasitesini test etmek amacıyla, bilinçli olarak tahsis edilmiş ancak serbest bırakılmamış (dangling pointer, memory leak) bellek blokları içeren, RAM tüketimi logaritmik olarak artan test uygulaması.

*Sistemimizin başarısı; Senaryo B çalıştığında arka uçtaki C++ analiz motorunun sızıntıyı anında tespit edip, ön uçtaki Python arayüzünde "Kritik Bellek Uyarısı" verebilmesi ile ölçülecektir.*

**2. Veri Akışı ve İletişim Mimarisi Kararı**
İzole bir Linux/WSL2 ortamında çalışan C++ analiz motorunun ürettiği verilerin, çapraz platform destekli Python arayüzüne (Frontend) aktarılabilmesi için yapılandırılmış bir veri formatı kullanılacaktır. Valgrind'den elde edilen karmaşık terminal çıktıları, arka uçta **JSON** formatına dönüştürülerek (Ayrıştırma/Parsing) Python arayüzüne iletilecek; böylece veri kaybı ve senkronizasyon sorunları önlenecektir.

** Haftalık Görev Dağılımı ve Ekip Hedefleri**

* **Mustafa:** Simülasyon senaryolarının mimari tasarımını yapmak, JSON tabanlı iletişim protokolünün standartlarını belirlemek ve takım içi kod çakışmalarını (Merge Conflict) yöneterek proje akışını sağlamak.
* **Yusuf (Backend):** Senaryo A ve Senaryo B'ye ait C++ simülasyon kodlarını yazmak ve Valgrind üzerinden ilk manuel testlerini gerçekleştirmek.
* **Semanur (Altyapı):** Kurulan WSL2 ortamında yazılan simülasyon kodlarının derlenmesi (G++) ve izole ortamda çalıştırılabilirliğini denetlemek.
* **Sümeyra (Gereksinim/Arayüz):** Elde edilecek JSON verilerinin Python tarafında hangi grafik türleriyle (Örn: Zaman-RAM tüketim grafiği) gösterileceğinin taslak UI/UX gereksinimlerini belirlemek.



## 2.2 Detaylı Risk Analizi ve Risk Yönetim Planı

**Sorumlu:** Mustafa Şahingöz 

**1. Risk Analizi ve Önceliklendirme Matrisi**
Projenin ilk haftasında belirlenen riskler analiz edilmiş; Olasılık ve Etki derecelerine göre (1: Düşük, 2: Orta, 3: Yüksek) aşağıdaki tablo oluşturulmuştur:

| Risk Kodu | Risk Tanımı | Olasılık | Etki | Risk Skoru | Öncelik |
| :--- | :--- | :---: | :---: | :---: | :--- |
| **RSK-01** | Valgrind Analizinin Sistemi Yavaşlatması (Performans Yükü) | 3 | 3 | 9 |  Kritik |
| **RSK-02** | Terminal Verilerinin Yanlış Ayrıştırılması (Parsing Hatası) | 3 | 2 | 6 |  Yüksek |
| **RSK-03** | C++ Arka Uç ve Python Ön Uç Arasında İletişim Gecikmesi | 2 | 3 | 6 |  Yüksek |
| **RSK-04** | Çapraz Platform (Windows/macOS) Uyumsuzlukları | 1 | 3 | 3 |  Orta |

**2. Risk Çözüm ve Yönetim Planı**
Yukarıdaki risklerin projeyi aksatmaması için aşağıdaki somut adımlar planlanmıştır:

* **RSK-01 Çözümü (Performans):** Analiz motorumuz tüm sistemi değil, sadece test edilen uygulamanın işlem kimliğini (PID) izleyecek şekilde kısıtlanacaktır.
* **RSK-02 Çözümü (Veri Hatası):** C++ arka ucu, karmaşık metin çıktılarını standart **JSON** formatına çevirecek; Python sadece bu kayıpsız JSON dosyasını okuyacaktır.
* **RSK-03 Çözümü (Arayüz Donması):** Arka planda analiz sürerken Python arayüzünün donmasını engellemek için, ön uçta Çoklu İş Parçacığı (**Multi-threading**) mimarisi kullanılacaktır.
* **RSK-04 Çözümü (İşletim Sistemi):** C++ kodlarının dışarıda değil, tamamen izole bir **WSL2 veya Docker** ortamında çalıştırılması kuralı sıkı bir şekilde uygulanacaktır.



# 2.3 🗄️ Görev: Veri Tabanı ve API Tasarımı

**👤 Sorumlu:** Semanur Buhan
**📅 İlgili Hafta:** Hafta 2

Bu döküman, **RAM Kurtarıcıları** projemizin arka planda çalışan C++ motoru ile kullanıcı arayüzümüz (Python) arasındaki haberleşme mimarisini ve veri saklama stratejilerini içermektedir.

---

## 🛠️ 1. Kullanılacak Teknolojiler ve Mimari Kararlar

Projemizin "sistem dostu ve hafif" olması temel amacımızdır. Bu nedenle ağır sunucu kurulumları gerektiren teknolojiler yerine, lokalde hızlı çalışan aşağıdaki araçlar tercih edilmiştir:

* **Veri Tabanı: SQLite + JSON 🪶**
    * **Neden?** SQLite, sunucu kurulumu gerektirmeyen, doğrudan proje klasörümüzde `.db` dosyası olarak çalışan çok hafif bir veri tabanıdır. 
    * **JSON Kullanımı:** Valgrind'den gelen karmaşık ve binlerce satırlık bellek sızıntısı loglarını (dökümlerini) veri tabanına yazıp sistemi yavaşlatmak yerine, bu detaylı raporları `.json` dosyaları olarak kaydedeceğiz. SQLite veri tabanımızda sadece uygulamanın adını, tarihini ve bu JSON dosyasının *yolunu (path)* tutacağız.
* **API (Haberleşme Köprüsü): FastAPI ⚡**
    * **Neden?** C++ ile yazdığımız donanım seviyesindeki motor ile Python arayüzümüzün doğrudan ve güvenli bir şekilde konuşabilmesi için yerel (lokal) bir RESTful API kuracağız. FastAPI, Python'daki en hızlı API araçlarından biridir ve bize anında test edebileceğimiz otomatik bir dökümantasyon sayfası (Swagger UI) sunar.

---

## 📊 2. Veri Tabanı Şeması (SQLite)

Veri tabanımızda şimdilik iki temel tablo bulunacaktır:

### Tablo: `analiz_gecmisi`
Geçmişte yapılan RAM analizlerinin özet bilgilerini tutar.
* `id` (Integer, Primary Key): Kayıt numarası.
* `uygulama_adi` (Text): Analiz edilen programın adı (Örn: chrome.exe).
* `tarih` (Datetime): Analizin yapıldığı zaman.
* `tespit_edilen_sizinti_mb` (Real): Bulunan gereksiz bellek tüketimi (MB cinsinden).
* **`detay_json_yolu` (Text):** Valgrind'in ürettiği detaylı teknik raporun bulunduğu JSON dosyasının bilgisayardaki konumu.

### Tablo: `ayarlar`
Kullanıcının uygulama içi tercihlerini tutar.
* `id` (Integer, Primary Key)
* `tema` (Text): Arayüz teması (Dark/Light).

---

## 🔌 3. API Dokümantasyonu (Endpoints)

Python arayüzümüzün (Frontend), C++ motorumuzu (Backend) tetiklemek ve veri almak için kullanacağı "Gişeler" (Endpoint'ler) aşağıdadır:

### 🚀 1. Analiz Başlatma
* **İstek:** `POST /api/analiz/baslat`
* **Açıklama:** Arayüzden gelen uygulama adını/PID'sini alır ve arka planda C++ (Valgrind) motorunu çalıştırır.
* **Gönderilen Veri (Body):** `{"uygulama_adi": "chrome.exe"}`
* **Dönen Cevap (Response):** `{"durum": "basarili", "mesaj": "Analiz basladi..."}`

### 📂 2. Geçmiş Raporları Listeleme
* **İstek:** `GET /api/raporlar`
* **Açıklama:** SQLite veri tabanına bağlanarak geçmişte yapılan tüm analizlerin özet listesini arayüze gönderir.
* **Dönen Cevap (Response):** Özet analiz listesi (JSON formatında liste).

### 🔍 3. Detaylı Rapor Görüntüleme
* **İstek:** `GET /api/raporlar/{rapor_id}`
* **Açıklama:** Kullanıcı listeden bir rapora tıkladığında, ilgili id'ye ait JSON dosyasının içeriğini okur ve arayüze detaylı Valgrind dökümünü iletir.



---
*Not: Yukarıdaki tüm mimari kararlar ve teknoloji seçimleri, RAM Kurtarıcıları ekibi olarak yaptığımız  fikir alışverişleri sonucunda projemizin ihtiyaçlarına en uygun yapı olarak ortaklaşa belirlenmiştir. Bu mimari sayesinde kodlarımız birbirine girmeyecek, C++ ve Python tam bir uyum içinde çalışacaktır.*


# Proje Akışı

## Hafta 2

### Mustafa Şahingöz
Detaylı Risk Analizi ve Yönetim Planı Oluşturuldu.

### Semanur Buhan
Veri Tabanı ve API Tasarımı yapıldı.


# 3. HAFTA ÇALIŞMALARI (15 Mart - 23 Mart 2026)
---

## 3.1 Risk Yönetim Planı Uygulama Tasarımı

**Görev Sorumlusu:** Mustafa Şahingöz  

Geçtiğimiz hafta projemizin önündeki olası riskleri ve başımıza açabilecekleri sorunları belirlemiştik. Bu hafta ise olarak "Peki bu riskler gerçeğe dönüşürse sahada tam olarak ne yapacağız, ve hangi araçları kullanacağız?" sorularına yanıt veren somut bir eylem planı hazırlandı. İşler ters gittiğinde devreye sokacağımız alternatif senaryolarımız (B planlarımız) ve yapmamız gerekenler aşağıdadır.

### 1. Sistem ve Performans Yükü Riski (RSK-01)
* **Nasıl Çözeceğiz?** Analiz motorumuzun tüm bilgisayarı tarayıp sistemi dondurmasını engellemek için, motoru sadece hedef uygulamanın PID'sine (İşlem Kimliğine) kilitleyeceğiz. Ayrıca verileri saniyede 100 kere değil, sistemi yormayacak şekilde 500 milisaniyelik periyotlarla çekeceğiz.
* **Sorumlu Kişi:** Yusuf Tuğra Deveci
* **Kullanılacak Kaynaklar:** Linux Procfs, CPU/RAM Profiler Araçları.
* **B Planımız:** Eğer Valgrind aracı bu ayarlara rağmen sistemi yormaya devam ederse, Valgrind'i tamamen projeden çıkarıp çok daha hafif çalışan Linux `/proc/meminfo` dosyası üzerinden okuma yapmaya geçeceğiz.

### 2. Veri Ayrıştırma (Parsing) Hataları (RSK-02)
* **Nasıl Çözeceğiz?** Karmaşık terminal çıktılarını arayüze yollarken verilerin yolda bozulmasını önlemek için, veriyi daha C++ tarafındayken güvenli JSON formatına çevireceğiz. Python arayüzümüz de bu veriyi kabul etmeden önce mutlaka bir doğrulama (validation) süzgecinden geçirecek.
* **Kullanılacak Kaynaklar:** C++ `nlohmann/json` kütüphanesi.
* **Sorumlu Kişi:** Semanur Buhan
* **B Planımız:** JSON veri akışında sürekli kopmalar veya format hataları yaşarsak, bu aracı köprüyü iptal edeceğiz. C++ arka ucumuz verileri doğrudan ortak bir SQLite veri tabanına yazacak, arayüz de veriyi oradan çekecek.

### 3. İletişim Gecikmesi ve Arayüzün Donması (RSK-03)
* **Nasıl Çözeceğiz?** Arka planda ağır bir C++ analizi dönerken kullanıcının tıkladığı arayüzün kilitlenmemesi çok kritik. Bu yüzden veri akışını FastAPI'nin asenkron (`async`) yapısı ve eşzamanlı veri aktarımı sağlayan WebSocket protokolü ile kuracağız.
* **Kullanılacak Kaynaklar:** FastAPI, Uvicorn, Python Asyncio.
* **Sorumlu Kişi:** Sümeyra Adıyaman
* **B Planımız:** WebSocket mimarisi geliştirme aşamasında beklediğimizden karmaşık çıkarsa veya bağlantı düşerse, arayüzün saniyede bir API'ye düzenli istek attığı standart "HTTP Polling" (Sürekli Yoklama) sistemine döneceğiz.

### 4. Çapraz Platform Uyumsuzluğu (RSK-04)
* **Nasıl Çözeceğiz?** Ekipte hem Mac hem Windows kullananlar olduğu için "Benim bilgisayarımda çalışıyordu, sende neden bozuldu?" sorunu yaşamamak adına, tüm geliştirme sürecimizi tamamen izole edilmiş WSL2 (Ubuntu) ortamında standartlaştıracağız.
* **Kullanılacak Kaynaklar:** Windows Subsystem for Linux (WSL2)
* **Sorumlu Kişi:** Mustafa Şahingöz
* **B Planımız:** WSL2'de donanım erişimi sorunları yaşarsak, tüm sistemi Docker konteyner mimarisine taşıyarak tek bir imaj üzerinden çalıştıracağız.

### 🎯 Zaman Çizelgesi ve Ölçülebilirlik (Nasıl Test Edeceğiz?)
Bu tasarladığımız önlemlerin gerçekten işe yarayıp yaramadığını  kodlama testimizde göreceğiz.


## 3.2 👩‍💻 Sümeyra Adıyaman – API Endpoint Spesifikasyonları

### Görev Tanımı
Bu hafta kapsamında, Akıllı Bellek Yönetimi Uygulaması için API endpoint spesifikasyonları oluşturulmuş ve FastAPI ile çalışan bir mock prototip geliştirilmiştir. Amaç, Python tabanlı kullanıcı arayüzü ile C++ analiz motoru arasındaki iletişimi standart ve modüler hale getirmektir.

---

## API Tasarım Amacı
API tasarımı, sistem bileşenleri arasında veri alışverişini düzenlemek amacıyla hazırlanmıştır. Kullanıcıdan gelen isteklerin analiz motoruna iletilmesi ve sonuçların kullanıcıya aktarılması hedeflenmiştir.

---

## Temel API Endpointleri
- `POST /api/v1/analiz/baslat` → analiz başlatır, oturum_id döner
- `GET /api/v1/analiz/durum/{oturum_id}` → analiz durumunu ve anlık RAM verisini gösterir
- `POST /api/v1/analiz/durdur/{oturum_id}` → analizi durdurur ve raporu kaydeder
- `GET /api/v1/raporlar` → geçmiş analizleri listeler
- `GET /api/v1/raporlar/{rapor_id}` → detaylı rapor getirir
- `GET /api/v1/canli/ram-anlık` → anlık RAM verisi verir
- `GET /api/v1/sistem/saglik` → sistemin çalışma durumunu döner

---

## Mock Prototip
Tasarımı somutlaştırmak amacıyla FastAPI ile çalışan bir mock prototip geliştirilmiştir. Prototip şu an örnek verilerle çalışmaktadır; ilerleyen haftalarda C++ motoruna bağlanacaktır.

**Prototip dosyası:** `src/api/main.py`

**Çalıştırma:**
```bash
pip install fastapi uvicorn
uvicorn main:app --reload
```

**Test arayüzü:** `http://127.0.0.1:8000/docs`

---

## Sonuç
Hazırlanan API tasarımı ve mock prototip sayesinde sistem bileşenleri arasındaki iletişim somut hale getirilmiştir. Endpointler gerçekten çalışmakta, istek atılabilmekte ve cevap dönmektedir.

---
# RAM Kurtarıcıları - Kullanıcı Arayüzü (UI) Tasarım Süreci 🛡️

**Sorumlu:** Semanur Buhan 👩‍💻

Bu dosya, projenin arka planında çalışan C++ ve Valgrind analiz süreçlerinin kullanıcıya sunulacağı arayüzün Figma üzerinde nasıl tasarlandığını adım adım açıklamaktadır.

## ✅ Yapılan İşlemler ve Tasarım Adımları

Uygulamanın arayüzü, modern masaüstü standartlarına uygun olarak iki ana ekran üzerinde kurgulanmıştır:

### 1. Uygulama İskeleti ve Giriş Ekranı
* **Pencere Yapısı:**  kırmızı, sarı ve yeşil kontrol düğmelerine sahip, gri renkli bir üst başlık çubuğu (`v1.0`) oluşturuldu. 🖥️
* **Sol Menü (Sidebar):** Kullanıcının hızlı erişimi için "Ana Ekran", "Geçmiş Raporlar" ve "Ayarlar" butonları eklendi. Her buton uygun profesyonel ikonlarla (klasör, çark, kalkan) desteklendi. 📂
* **Analiz Giriş Alanı:** Ekranın merkezine, analiz edilecek dosyanın adının yazılacağı beyaz bir metin kutusu yerleştirildi. Kutunun içine klavye ve mikrofon ikonları eklendi. ⌨️
* **Başlat Butonu:** "VALGRIND ANALİZİNİ BAŞLAT 🚀" yazılı, belirgin mavi bir buton tasarlandı. Butona derinlik katması için gölge (drop shadow) efekti uygulandı.

### 2. Analiz Sonuç Ekranı
* **Görsel Veri:** Analiz sonuçlarını özetlemek için kırmızı renkli bir halka grafik (donut chart) çizildi. Grafiğin ortasına tespit edilen toplam sızıntı miktarı (16 Bayt) yazıldı. 📊
* **Hata Raporlama:** Valgrind çıktılarının (leak logları) listeleneceği, okunabilirliği yüksek, açık gri renkli geniş bir rapor paneli eklendi. 📝
* **Zaman Damgası:** Analizin yapıldığı tarih ve saat bilgisi başlık altına eklendi.

### 3. Görsel ve Teknik Detaylar
* **Renk Geçişi (Gradient):** Uygulamanın iç arka planına modern ve temiz bir görünüm katması için açık mavi (`DAE1F2`) ve açık mor (`EEDDEE`) tonlarında yumuşak bir renk geçişi (linear gradient) uygulandı. 🎨
* **Hiyerarşi:** Başlıklar ve önemli metinler kalınlaştırılarak (Semibold) görsel okunabilirlik artırıldı.
* **Kenar Yumuşatma:** Tüm buton ve kutuların köşeleri modern bir görünüm için  yuvarlatıldı.


---
*Bu tasarım süreci, projenin profesyonel bir son ürün haline gelmesi için kullanıcı deneyimi (UX) odaklı yürütülmüştür.*


# Proje Akışı

## Hafta 3

### Mustafa Şahingöz
Risk Yönetim Planı Uygulama Tasarımı yapıldı.

### Sümeyra Adıyaman
API Endpoint Spesifikasyonları oluşturuldu ve mock prototip geliştirildi.

### Semanur Buhan
Uygulama arayüzü için prototip UX wireframe tasarımı yapıldı.

