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



# 4. HAFTA ÇALIŞMALARI (21 Nisan  - 28 Nisan 2026)
---

## 4.1 👩‍💻 Sümeyra Adıyaman – Veri Yapılarının Optimizasyonu

### Görev Tanımı
Bu hafta kapsamında kullanılan veri yapıları bellek kullanımı açısından analiz edilmiş ve optimizasyon çalışmaları yapılmıştır. Ek olarak eksik kalan C++ simülasyon kodları yazılmıştır.

---

### Yapılan Çalışmalar

**1. Veri Yapısı Optimizasyonu (`src/api/main.py`)**
- `sessions = {}` yapısı sınırsız büyüyebiliyordu
- Maksimum 100 oturum tutan `OrderedDict` yapısına geçildi
- Kapasite aşıldığında en eski oturum otomatik siliniyor
- Bellek kullanımını anlık raporlayan yeni endpoint eklendi: `GET /api/v1/sistem/bellek`

**2. C++ Simülasyon Kodları**
- `api/senaryo_a.py` → Sağlıklı bellek yönetimi, Valgrind'de 0 hata bekleniyor
- `api/senaryo_b.py` → Bilinçli bellek sızıntısı, ~840 byte sızıntı bekleniyor


4.2 👨‍💻 Yusuf Tuğra Deveci – Nesne Havuzu ve Bellek Optimizasyonu
Görev Tanımı
Bu hafta kapsamında, backend mimarisinde nesne yönetimini optimize etmek amacıyla Object Pool (Nesne Havuzu) tasarım deseni entegre edilmiş ve C++ tarafında bellek güvenliğini sağlamak için RAII prensipleri uygulanmıştır.

Yapılan Çalışmalar

1.Nesne Havuzu Entegrasyonu (backend/VeriHavuzu.cpp)
Gereksiz nesne oluşumunu (object instantiation) engellemek için geri dönüşümlü bir havuz yapısı kuruldu.
Sistem yükü optimize edilerek işlemci maliyeti düşürüldü.

2.Bellek Güvenliği ve RAII
Manuel bellek yönetiminden kaynaklanabilecek memory leak (bellek sızıntısı) riskleri RAII prensibiyle ortadan kaldırıldı.
Modüler bir yapı oluşturularak VeriHavuzu.h ve VeriHavuzu.cpp dosyaları sisteme eklendi.

3.C++ Simülasyon Testleri

Hazırlanan yapıların stabil çalışıp çalışmadığı kontrol edildi ve backend klasörüne başarılı şekilde pushlandı.

---
## 4.3 Mustafa Şahingöz - Bellek Sızıntısı Analizi ve Düzeltilmesi

##  Görev Özeti
Bu hafta, projenin temelini oluşturan "Bellek Havuzu" motoru ile test senaryolarının entegrasyonu gerçekleştirilmiş ve sistemdeki bellek sızıntıları (memory leaks) analiz edilerek tamamen giderilmiştir.

## Gerçekleştirilen İşlemler
1. **Kod Entegrasyonu:** Yusuf'un geliştirdiği `VeriHavuzu` motoru ile Sümeyra'nın hazırladığı test senaryoları birleştirildi.
2. <img width="544" height="55" alt="Ekran Resmi 2026-04-25 17 49 55" src="https://github.com/user-attachments/assets/16193443-97b0-49d6-944c-2ca4376f8acf" />

3. **Sızıntı Analizi:** Senaryo-B üzerinde yapılan testlerde ~840 byte miktarında kritik bellek sızıntısı tespit edildi.
4. <img width="345" height="89" alt="Ekran Resmi 2026-04-25 17 29 21" src="https://github.com/user-attachments/assets/5ec03f89-caa0-48c2-b5f5-0ef6d5d96b75" />



5. **Teknik Onarım:** - `new[]` ile açılan diziler `delete[]` ile serbest bırakıldı.
6. <img width="925" height="394" alt="Ekran Resmi 2026-04-25 18 05 47" src="https://github.com/user-attachments/assets/ab2a8eaf-369b-4897-9afb-19b6fddb9af5" />

   - Döngü içerisinde sahipsiz kalan nesneler için `delete` komutları eklendi.
   - *Dangling pointer* (sarkan işaretçi) riskine karşı `nullptr` atamaları yapıldı.
7. **Git Yönetimi:** Uzak depodaki (remote) değişiklikler yerel kodla birleştirildi (Merge), çakışmalar çözüldü ve stabil sürüm GitHub'a fırlatıldı.
8. **Düzeltme:** Senaryo B kısmındaki sızıntılar tespil edildi ve düzeltilmiş hali yüklendi.

## Sonuç
- **Hata Sayısı:** 0
- **Bellek Durumu:** "All heap blocks were freed" (Tüm bellek geri kazanıldı).
- **Dosya Yapısı:** Orijinal testlere dokunulmadan `senaryo_b_duzeltilmis.cpp` adıyla optimize edilmiş sürüm sisteme eklendi.


**Durum:** ✅ Tamamlandı

---

## 4.4 Semanur Buhan - Bellek Yönetimi Araçları Entegrasyonu ve Teknik Raporlama

##  Görev Özeti
Proje genelinde bellek kullanımını izlemek, analiz etmek ve elde edilen verileri raporlanabilir formata getirmek amacıyla Valgrind entegrasyonu sağlandı.

## Yapılan Çalışmalar

1. **Otomasyon Betiği (Integration):** C++ test senaryolarını (`senaryo_b_duzeltilmis.cpp` vb.) otomatik olarak derleyen ve Valgrind üzerinden çalıştıran `valgrind_analiz.sh` betiği yazıldı. Bu sayede manuel test süreçleri tek komutla çalışabilir hale getirildi.
2. **Raporlama Standardizasyonu:** Valgrind analiz sonuçlarının standart bir `valgrind_raporu.log` dosyasına yönlendirilmesi sağlandı. Bu altyapı, ileride tasarladığımız arayüzün (UI) veri okuyacağı ana motoru oluşturmaktadır.
3. **Sistem Doğrulaması:** Ekip arkadaşlarımın Nesne Havuzu (Object Pool) ve RAII kullanarak sunduğu çözümler, yazdığım bu otomasyon motoruyla test edildi.

**🔍 Teknik Analiz Raporu (Valgrind Results):**
Analiz edilen dosya: `tests/senaryo_b_duzeltilmis.cpp`

| Kategori | Sonuç |
| :--- | :--- |
| **Hata Sayısı (Errors)** | 0 (Sıfır Hata) |
| **Kesin Kayıp (Definitely Lost)** | 0 bytes in 0 blocks |
| **Toplam Bellek Kullanımı** | All heap blocks were freed (Tüm bellek geri kazanıldı) |
| **Durum** | ✅ Güvenli / Stabil |

## Sonuç
 - Valgrind entegrasyonu başarıyla tamamlandı.
 - Yapılan testler sonucunda sistemin bellek sızıntılarından tamamen arındırıldığı teknik olarak raporlanmıştır.

**Durum:** ✅ Tamamlandı.

---

## 4.5  Aylin Akagündüz – Büyük Boyutlu Verilerin İşlenmesinde Bellek Optimizasyonu

### Görev Tanımı

Bu hafta kapsamında, büyük boyutlu verilerin (resim buffer'ları ve video akışları) işlenmesi sırasında bellek kullanımının nasıl optimize edileceği araştırılmış ve projemize uygun teknikler belirlenmiştir.

---

### Uygulanan Optimizasyon Teknikleri

**1. Sıkıştırma (zlib / DEFLATE)**
Valgrind çıktıları ve JSON raporları diske yazılmadan önce sıkıştırılır. Ham boyutu %40–65 oranında azaltır, kayıpsız çalışır.

**2. Parçalı İşleme (Stream Processing)**
Büyük veri tüm buffer'a yüklenmek yerine parça parça okunur. Bellekte aynı anda yalnızca tek bir parça bulunduğundan RAM kullanımı sabit kalır.

**3. Bellek Havuzu (Memory Pool)**
Yusuf'un bu hafta teslim ettiği `VeriHavuzu` modülü büyük veri nesneleri için de kullanılacak; sürekli `malloc/free` döngüsünden kaçınılır.

**4. Tembel Yükleme (Lazy Loading)**
Geçmiş raporlar listesinde yalnızca özet bilgi tutulur; tam Valgrind verisi yalnızca kullanıcı raporu açtığında yüklenir. Semanur'un `detay_json_yolu` mimarisiyle uyumludur.

---

### Sonuç

Belirlenen dört teknik birbirini tamamlayıcı niteliktedir ve mevcut mimariyi bozmadan projeye entegre edilecektir. Bu strateji aynı zamanda Hafta 2'de tanımlanan RSK-01 ve RSK-02 risklerine karşı ek güvence sağlamaktadır.


# Proje Akışı

## Hafta 4

### Sümeyra Adıyaman
Veri yapıları optimize edildi ve C++ simülasyon kodları yazıldı.

**Yusuf Tuğra Deveci**
* Uygulama genelinde gereksiz nesne oluşumunu engellemek amacıyla **Object Pool (Nesne Havuzu)** tasarım deseni backend mimarisine entegre edildi.
* RAII prensibi kullanılarak bellek sızıntıları (memory leak) önlendi ve sistem yükü optimize edildi.
* `arka uç` klasörüne `VeriHavuzu.h` ve `VeriHavuzu.cpp` dosyaları modüler yapıda eklendi.

### Mustafa Şahingöz
Uygulamadaki bellek sızıntılarını tespit edildi ve bu sızıntıları gidermek için gerekli düzeltmeler yapıldı. Performans testleri ile doğrulandı.

### Semanur Buhan
Proje genelinde bellek kullanımını otomatize etmek ve analiz sonuçlarını standart bir formata dönüştürmek amacıyla Valgrind entegrasyonu sağlandı.

* Test süreçlerini hızlandırmak için C++ senaryolarını otomatik derleyip test eden `valgrind_analiz.sh` scripti yazıldı.
* Analiz çıktıları için `.log` tabanlı standart bir raporlama altyapısı kuruldu.
* Geliştirilen otomasyon motoru üzerinden yapılan son doğrulamalarda, sistemde **0 bayt sızıntı** ve **0 hata** olduğu teknik olarak raporlanarak projenin bellek güvenliği onaylandı.

### Aylin Akagündüz
Büyük boyutlu verilerin bellek optimizasyonu araştırıldı, uygulama stratejisi belirlendi.



# 5. HAFTA ÇALIŞMALARI (2 Mayıs  - 9 Mayıs 2026)
---

## 5.1 👩‍💻 Sümeyra Adıyaman – Profilleme Aracı Entegrasyonu

### Görev Tanımı
Bu hafta kapsamında, FastAPI tabanlı API'ye Python'ın yerleşik `tracemalloc` kütüphanesi entegre edilerek gerçek zamanlı bellek profilleme özelliği kazandırılmıştır.

---

### Yapılan Çalışmalar

**1. tracemalloc Entegrasyonu (`src/api/main.py`)**
- API başladığında `tracemalloc.start()` ile profilleme otomatik olarak devreye giriyor
- Anlık ve zirve bellek kullanımı sürekli takip ediliyor

**2. Yeni Endpoint: `GET /api/v1/sistem/profil`**
- Anlık bellek kullanımı (byte ve KB cinsinden)
- Zirve bellek kullanımı (byte ve KB cinsinden)
- En çok bellek tüketen 5 kod noktası (dosya adı, satır numarası, boyut)

**3. Test Sonucu**
- Anlık bellek: ~2.497 KB
- Zirve bellek: ~2.607 KB
- En çok bellek tüketen nokta: Python'un kendi import mekanizması (normal)

---

### Sonuç
Profilleme aracı başarıyla entegre edilmiştir. API çalışırken bellek kullanımı anlık olarak izlenebilmekte ve raporlanabilmektedir.

**Güncellenen dosya:** `src/api/main.py`

---
## 5.2 Mustafa Şahingöz - Bellek Optimizasyonu Araştırması ve Analiz Raporu

##  Giriş
Bu rapor, "Akıllı Bellek Yönetimi" projesinin 5. haftası kapsamında; sistem performansını artırmak, bellek sızıntılarını önlemek ve kaynak kullanımını optimize etmek amacıyla incelenen tekniklerin analizini sunar.

---

##  Araştırılan Teknikler

### 1. Object Pooling (Nesne Havuzlama)
**Tanım:** Sık kullanılan nesnelerin her seferinde yeniden oluşturulması yerine, önceden ayrılmış bir bellek havuzunda saklanıp ihtiyaç duyulduğunda tekrar kullanılmasıdır.

*   **Avantajları:** `new` ve `delete` gibi sistem çağrılarının maliyetini minimize eder. Bellek parçalanmasını (fragmentation) engeller.
*   **Dezavantajları:** Kullanılmayan nesneler havuzda yer kapladığı için başlangıçta sabit bir bellek yükü oluşturur.
*   **Değerlendirme:** Yüksek frekanslı analiz kayıtları için uygulanması önerilir.

### 2. RAII ve Akıllı İşaretçiler (Smart Pointers)
**Tanım:** Bellek yönetiminin nesne ömrüne bağlanmasıdır. C++ standart kütüphanesindeki `std::unique_ptr` ve `std::shared_ptr` kullanılarak manuel bellek yönetiminden kaçınılır.

*   **Avantajları:** Bellek sızıntılarını (memory leaks) ve geçersiz işaretçi (dangling pointer) hatalarını yazılım seviyesinde önler.
*   **Dezavantajları:** `shared_ptr` kullanımı, atomik referans sayacı nedeniyle çok düşük bir performans kaybı yaratabilir.
*   **Değerlendirme:** Projenin genel mimarisi ve güvenlik standartları için zorunludur.

### 3. Data Locality (Veri Yerelliği)
**Tanım:** Verilerin bellekte birbirine yakın adreslerde tutularak CPU önbellek (Cache) hit oranının artırılmasıdır.

*   **Avantajları:** Bellek erişim hızını önemli ölçüde artırır ve işlemci verimini maksimize eder.
*   **Dezavantajları:** Veri yapılarının (örneğin linked-list yerine array kullanımı) daha katı tasarlanmasını gerektirir.
*   **Değerlendirme:** Büyük veri setleri üzerinde yapılan analizlerde kritik öneme sahiptir.

---

##  Teknik Karşılaştırma Tablosu

| Teknik | Performans Kazancı | Uygulama Zorluğu | Güvenlik Etkisi |
| :--- | :---: | :---: | :---: |
| **Object Pooling** | ⭐⭐⭐⭐⭐ | Orta | Orta |
| **Smart Pointers** | ⭐⭐⭐ | Düşük | ⭐⭐⭐⭐⭐ |
| **Data Locality** | ⭐⭐⭐⭐⭐ | Yüksek | Orta |

---

## Proje Uygulama Planı ve Kararlar
Yapılan analizler sonucunda Hafta 5 ve sonrası için şu kararlar alınmıştır:

1.  **Güvenlik:** Projedeki tüm dinamik bellek tahsisleri `std::unique_ptr` yapısına geçirilecek.
2.  **Hız:** Analiz sonuçlarını tutan sınıflar için bir `Object Pool` katmanı prototip olarak geliştirilecek.
3.  **Analiz:** Valgrind çıktıları ile optimizasyon öncesi ve sonrası bellek grafiği karşılaştırılacak.

---


# Proje Akışı

## Hafta 5

### Sümeyra Adıyaman
Profilleme aracı (`tracemalloc`) API'ye entegre edildi. Bellek kullanımını anlık raporlayan `GET /api/v1/sistem/profil` endpoint'i eklendi.


### Mustafa Şahingöz
Farklı bellek optimizasyonu tekniklerini araştırıldı ve projemizde kullanılabilecek potansiyel yöntemleri belirlendi. Bulduğunuz yöntemlerin avantaj ve dezavantajlarını analiz edildi.
=======


# 6. HAFTA ÇALIŞMALARI (2 Mayıs - 9 Mayıs 2026)
---

## 6.1 👩‍💻 Sümeyra Adıyaman – Performans Testleri ve Optimizasyon

### Görev Tanımı
Bu hafta kapsamında API'nin performansı test edilmiş ve optimizasyon çalışmaları yapılmıştır.

---

### Performans Testi Sonuçları (`tests/performans_testi.py`)

| Endpoint | Toplam İstek | Başarılı | Ortalama Süre | En Fazla Süre |
| :--- | :---: | :---: | :---: | :---: |
| GET /api/v1/sistem/saglik | 50 | 50 | 21.08 ms | 39.35 ms |
| GET /api/v1/sistem/bellek | 50 | 50 | 23.09 ms | 47.96 ms |
| GET /api/v1/sistem/profil | 50 | 50 | 95.31 ms | 242.84 ms |
| POST /api/v1/analiz/baslat | 20 | 20 | 12.74 ms | 18.59 ms |
| GET /api/v1/raporlar | 50 | 50 | 13.04 ms | 31.66 ms |

### Değerlendirme
- Tüm isteklerde **0 başarısız** istek — sistem stabil çalışıyor
- Profilleme endpoint'i diğerlerine göre yavaş (~95ms) — bellek snapshot aldığı için beklenen bir durum
- Diğer tüm endpoint'ler 25ms altında yanıt veriyor — performans kabul edilebilir seviyede

---

### Sonuç
API performans testleri başarıyla tamamlanmıştır. Sistem yük altında stabil çalışmakta ve tüm isteklere yanıt vermektedir.

**Dosyalar:** `tests/performans_testi.py`, `src/api/main.py`

---

# Proje Akışı

## Hafta 6

### Sümeyra Adıyaman
API performans testleri yapıldı. 170 istek atıldı, 0 hata alındı. Sonuçlar raporlandı.

