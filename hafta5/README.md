# Görüntü İşleme — Ödev 1, 2, 3

C++ / OpenCV et CUDA. **Uniquement les questions 1 à 3** ; les conversions de la question 4 ne figurent pas dans ce dépôt.

## Contenu

| Question | Fichier | Description |
|---|---|---|
| 1 | `01_histogram_opencv.cpp` | Lire une image couleur, calculer et enregistrer chaque histogramme B, G, R avec OpenCV `calcHist`. |
| 2 | `02_histogram_manual.cpp` | Fonction personnelle `myCalcHist` pour chaque canal ; sans `cv::calcHist`. |
| 3 | `03_grayscale_compare.cpp` | Conversion manuelle BGR → niveaux de gris et comparaison avec `cvtColor`. |
| CUDA 1–2 | `cuda/01_02_histogram_cuda.cu` | Calcul GPU par canal avec `atomicAdd`, vérification CPU, export TXT. |
| CUDA 3 | `cuda/03_grayscale_cuda.cu` | Conversion niveaux de gris GPU, comparaison OpenCV. |

OpenCV lit les images couleur en **BGR**, pas RGB. La somme des différences absolues est utilisée comme mesure d'erreur. Une erreur nulle dépend de l'image et de la version d'OpenCV, surtout pour les conversions avec arrondi ; ne pas l'affirmer sans exécuter les programmes.

## Google Colab

1. Choisir un runtime GPU pour les fichiers CUDA (Runtime → Change runtime type → GPU).
2. Importer votre image dans `/content` (`cow.jpg` ou autre nom).
3. Importer les fichiers `.cpp`/`.cu` du dépôt dans `/content`, en gardant le sous-dossier `cuda/` si possible.
4. Installer OpenCV pour C++ si absent :

```bash
!sudo apt-get update -qq
!sudo apt-get install -y libopencv-dev pkg-config
```

Les commandes suivantes sont à exécuter **dans une cellule Colab** (elles commencent par `!`). Un chemin d'image peut être donné comme argument ; les exemples supposent `/content/cow.jpg`.

### Question 1

```python
!g++ /content/01_histogram_opencv.cpp -o /content/hist_opencv $(pkg-config --cflags --libs opencv4)
!cd /content && ./hist_opencv /content/cow.jpg
```

### Question 2

```python
!g++ /content/02_histogram_manual.cpp -o /content/hist_manual $(pkg-config --cflags --libs opencv4)
!cd /content && ./hist_manual /content/cow.jpg
```

### Question 3

```python
!g++ /content/03_grayscale_compare.cpp -o /content/gray_cpu $(pkg-config --cflags --libs opencv4)
!cd /content && ./gray_cpu /content/cow.jpg
```

### CUDA (questions 1–2)

```python
!nvcc /content/cuda/01_02_histogram_cuda.cu -o /content/hist_cuda $(pkg-config --cflags --libs opencv4)
!cd /content && ./hist_cuda /content/cow.jpg
```

### CUDA (question 3)

```python
!nvcc /content/cuda/03_grayscale_cuda.cu -o /content/gray_cuda $(pkg-config --cflags --libs opencv4)
!cd /content && ./gray_cuda /content/cow.jpg
```

### Visualiser les résultats dans Colab

```python
from IPython.display import display
from PIL import Image

display(Image.open('/content/02_hist_mavi.png'))
display(Image.open('/content/02_hist_yesil.png'))
display(Image.open('/content/02_hist_kirmizi.png'))
display(Image.open('/content/03_gri_manual.png'))
```

Les fichiers `cuda_hist_*.txt` contiennent deux colonnes : intensité (0–255) et effectif GPU. La visualisation des données peut être faite avec matplotlib. Ne pas importer des fichiers compilés ; importer seulement les sources, le README et éventuellement vos résultats.

## Résultats

Valeurs communiquées lors des essais précédents sur `cow.jpg` : erreur 0 pour la conversion niveaux de gris CPU/CUDA et pour les trois histogrammes CUDA. **Les sources de ce ZIP sont regroupées et n'ont pas été exécutées dans ce conteneur** (OpenCV C++ et NVCC n'y sont pas installés). Refaire les tests dans Colab avant dépôt et joindre les sorties réellement obtenues.
