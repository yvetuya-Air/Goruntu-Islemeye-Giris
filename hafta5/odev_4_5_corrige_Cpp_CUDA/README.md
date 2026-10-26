# Görüntü İşleme Ödevi — 4. ve 5. Sorular (C++ / CUDA)

## 4. Soru — Renk Uzayı Dönüşümleri

C++ kullanılarak BGR → RGB, BGR → GRAY, BGR → HSV ve BGR → YCrCb dönüşümleri için manuel fonksiyonlar yazılmıştır. Fonksiyonlar `Conversion` fonksiyon işaretçisi (function pointer) üzerinden çağrılmıştır. Elde edilen görüntüler, OpenCV `cvtColor()` referansıyla piksel bazında karşılaştırılmıştır.

Toplam mutlak hata, tüm pikseller ve kanallar üzerinde `sum(abs(manuel - OpenCV))` olarak hesaplanmıştır.

| Dönüşüm | C++ toplam mutlak hata | CUDA toplam mutlak hata |
|---|---:|---:|
| BGR → RGB | 0 | 0 |
| BGR → GRAY | 0 | 0 |
| BGR → HSV | 0 | 0 |
| BGR → YCrCb | 0 | 0 |

**Test görüntüsü:** `20240116_171350.jpg` (Colab'da kullanılan test dosyası; bu arşive eklenmemiştir). Yukarıdaki sonuçlar bu görüntünün Colab'da test edilmesinden elde edilmiştir; farklı görüntüler ve OpenCV sürümleri için sıfır hata önceden garanti edilmez.

Kaynak kodları:
- `cpp/04_color_conversions_pointer.cpp`
- `cuda/04_color_conversions_cuda.cu`

## 5. Soru — X-Ray Görüntülerinin Gri Tonlamaya Dönüştürülmesi ve Renklendirilmesi

Kaynak veri seti: https://www.kaggle.com/datasets/orvile/hums-x-ray-dataset

Veri setinin **`ThreatsRGB`** klasöründen aşağıdaki beş gerçek görüntü seçilmiştir (ödev metninde `ThreadsRGB` yazmaktadır):
- `image_0_1000.jpg`
- `image_0_1054.jpg`
- `image_0_1062.jpg`
- `image_0_1071.jpg`
- `image_0_1072.jpg`

C++ ve OpenCV ile görüntüler gri tonlamaya dönüştürülmüş, ardından VIRIDIS renk paletiyle renklendirilmiştir. Aynı işlemler CUDA çekirdekleri (kernels) kullanılarak GPU üzerinde tekrar gerçekleştirilmiştir. Her görüntü için orijinal, gri tonlu ve VIRIDIS çıktı dosyaları `outputs/cpu/` ve `outputs/cuda/` klasörlerinde bulunur.

### VIRIDIS paleti neden seçildi?

VIRIDIS renk paletinde parlaklık görece düzenli artar ve gri seviye yoğunluk farklılıkları çeşitli renklerle gösterilebilir. Böylece güvenlik operatörlerinin bazı yoğunluk değişimlerini ve sınırları görsel olarak ayırt etmesi kolaylaşabilir. Bununla birlikte yalnızca bu çalışmadan, güvenlik operatörlerinin tespit başarısının arttığı sonucu çıkarılamaz.

### Orijinal renkli görüntü ile yeniden renklendirilmiş görüntü arasındaki fark

Orijinal X-ray görüntülerinde malzeme veya enerji bilgileri sahte renklerle kodlanmış olabilir. RGB görüntü gri tonlamaya çevrilirken üç kanaldaki bilgi tek kanala indirgenir. VIRIDIS ise yalnızca gri seviyeden yeni bir renk üretir; bu nedenle orijinal malzeme bilgisini geri getirmez. Sayısal MAE, iki renkli görüntü arasındaki ortalama mutlak BGR farkıdır; operatör başarısını veya malzeme tanıma doğruluğunu ölçmez.

### CUDA doğrulaması

| Görüntü | CUDA–OpenCV gri hata | CUDA–OpenCV VIRIDIS hata | Orijinal–VIRIDIS MAE |
|---|---:|---:|---:|
| image_0_1000 | 0 | 0 | 69.437039 |
| image_0_1054 | 0 | 0 | 70.078379 |
| image_0_1062 | 0 | 0 | 69.487398 |
| image_0_1071 | 0 | 0 | 72.497847 |
| image_0_1072 | 0 | 0 | 59.770183 |

Sonuçlar `outputs/cpu/results.csv` ve `outputs/cuda/results.csv` dosyalarında kaydedilmiştir. Beş test görüntüsünde CUDA çıktılarına ait toplam mutlak hata sıfırdır.

Kaynak kodları:
- `cpp/05_xray_recolor.cpp`
- `cuda/05_xray_recolor_cuda.cu`

## Google Colab — Derleme ve Çalıştırma

OpenCV C++ kütüphaneleri için: `sudo apt-get install -y libopencv-dev pkg-config`. CUDA için GPU çalışma zamanını etkinleştirin.

Proje dizinini `ROOT` değişkenine göre ayarlayın. Colab kod hücrelerinde terminal komutlarının başına `!` koyun.

```bash
ROOT=/content/devoir/odev_4_5_corrige_Cpp_CUDA

g++ -std=c++17 "$ROOT/cpp/04_color_conversions_pointer.cpp" -o /content/color_cpp $(pkg-config --cflags --libs opencv4)
/content/color_cpp /content/20240116_171350.jpg

nvcc -std=c++17 "$ROOT/cuda/04_color_conversions_cuda.cu" -o /content/color_cuda $(pkg-config --cflags --libs opencv4)
/content/color_cuda /content/20240116_171350.jpg

g++ -std=c++17 "$ROOT/cpp/05_xray_recolor.cpp" -o /content/xray_cpp $(pkg-config --cflags --libs opencv4)
/content/xray_cpp "$ROOT/images" "$ROOT/outputs/cpu"

nvcc -std=c++17 "$ROOT/cuda/05_xray_recolor_cuda.cu" -o /content/xray_cuda $(pkg-config --cflags --libs opencv4)
/content/xray_cuda "$ROOT/images" "$ROOT/outputs/cuda"
```

Bu komutlardaki `20240116_171350.jpg` örnek test görüntüsünün Colab'da bulunması gerekir; istenirse başka bir renkli görüntü kullanılabilir.

## Deney Durumu

Bu arşivdeki kaynak kodlar, beş özgün HUMS görüntüsü ve elde edilen CPU/CUDA görüntüleri bulunmaktadır. Dördüncü sorunun C++ ve CUDA kodları Colab'da test edilmiş ve raporlanan hatalar sıfırdır. Beşinci sorunun C++ ve CUDA uygulamaları çalıştırılmış; CUDA doğrulamasında her beş görüntü için sıfır hata elde edilmiştir. Sonuç görüntülerinin operatörlerce kullanılabilirliği için ayrıca görsel değerlendirme önerilir.
