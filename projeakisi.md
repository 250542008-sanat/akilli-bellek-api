# 1. HAFTA ÇALIŞMALARI (6 Mart - 12 Mart 2026)
---

## 1.1 Proje Analizi ve Kapsam Tanımlama

**Sorumlu:** Mustafa Şahingöz

**1. Projenin Temel Hedefleri**

* Elde edilen teknik verileri yapılandırılmış bir formatta işleyerek arayüz üzerinden kullanıcıya raporlamak.
* Arka planda çalışan analiz motorunun ürettiği karmaşık terminal çıktılarını, Python tabanlı kullanıcı dostu bir grafik arayüz (GUI) ile görselleştirerek (grafikler, metrikler ve uyarılar halinde) raporlamak.

**2. Proje Kapsamı**

* **Kapsam İçi:** Hedef uygulamanın Linux ortamında bellek yönetim araçlarıyla (Valgrind) çalıştırılması, anlık/periyodik bellek tüketim verilerinin toplanması ve bu verilerin Python arayüzünde anlamlı raporlara dönüştürülmesi.
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

- Sistem farklı işletim sistemlerinde çalışabilecek şekilde tasarlanmalıdır.
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

## 2.2 Detaylı Risk Analizi ve Risk Yönetim Planı

**Sorumlu:** Mustafa Şahingöz (Proje Yöneticisi)

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

