#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

int main() {

    Mat resim = imread("images/image.jpg");

    if (resim.empty()) {
        cout << "resim okunamadi" << endl;
        return 1;
    }

    fs::create_directories("results");

    Mat gris, kenar;
    cvtColor(resim, gris, COLOR_BGR2GRAY);
    Canny(gris, kenar, 50, 150);

    int w = resim.cols;
    int h = resim.rows;

    int minR = 10;
    int maxR = min(80, min(w, h) / 3);
    int nbAngle = 72;
    int esik = 35;

    if (maxR < minR) return 1;

    vector<double> sinus(nbAngle), cosinus(nbAngle);

    for (int i = 0; i < nbAngle; i++) {
        double angle = 2 * CV_PI * i / nbAngle;
        sinus[i] = sin(angle);
        cosinus[i] = cos(angle);
    }

    int nbR = maxR - minR + 1;

    vector<int> oy((size_t)w * h * nbR, 0);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

          
        if (kenar.ptr<uchar>(y)[x] == 0)
              continue;

           for (int r = minR; r <= maxR; r++) {
              for (int t = 0; t < nbAngle; t++) {

                int a = cvRound(x - r * cosinus[t]);
                 int b = cvRound(y - r * sinus[t]);

                  if (a >= 0 && a < w && b >= 0 && b < h) {
                        size_t pos = ((size_t)b * w + a) * nbR
                                     + (r - minR);
                        oy[pos]++;
                    }}}}}

    Mat resultat = resim.clone();
    int sayi = 0;

    for (int b = 2; b < h - 2; b++) {
        for (int a = 2; a < w - 2; a++) {
            for (int r = minR + 1; r < maxR; r++) {

                int ri = r - minR;
                size_t pos = ((size_t)b * w + a) * nbR + ri;

                int v = oy[pos];

                if (v < esik) continue;

                bool uygun = true;

                for (int dy = -2; dy <= 2 && uygun; dy++) {
                    for (int dx = -2; dx <= 2 && uygun; dx++) {
                        for (int dr = -1; dr <= 1; dr++) {

                            if (dx == 0 && dy == 0 && dr == 0)
                                continue;

                            size_t p2 = ((size_t)(b + dy) * w
                                      + (a + dx)) * nbR + ri + dr;

                            if (oy[p2] > v) {
                                uygun = false;
                                break;
                            }
                        }
                    }
                }

                if (uygun) {
                    circle(resultat, Point(a, b), r,
                           Scalar(0, 255, 0), 2);

                    circle(resultat, Point(a, b), 2,
                           Scalar(0, 0, 255), FILLED);

                    sayi++;
                }
            }
        }
    }

    Mat matrice(h, w, CV_32SC1);
    int rayon = (minR + maxR) / 2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            size_t pos = ((size_t)y * w + x) * nbR
                         + rayon - minR;

            matrice.ptr<int>(y)[x] = oy[pos];
        }
    }

    Mat affichage;
    normalize(matrice, affichage, 0, 255,
              NORM_MINMAX, CV_8UC1);

    imwrite("results/04_edges.png", kenar);
    imwrite("results/04_votes.png", affichage);
    imwrite("results/04_manual_circles.png", resultat);

    cout << "oylama matrisi: "
         << w << " x " << h << " x " << nbR << endl;

    cout << "bulunan cember: " << sayi << endl;

    return 0;
}
