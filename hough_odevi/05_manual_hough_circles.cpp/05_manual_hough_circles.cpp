#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

int main() {

    Mat resim = imread("images/image.jpg");

    if (resim.empty()) {
        cout << "resim bulunamadi" << endl;
        return 1;
    }

    fs::create_directories("results");

    Mat petit;
    double scale = min(1.0, 320.0 / max(resim.cols, resim.rows));

    if (scale < 1.0)
        resize(resim, petit, Size(), scale, scale, INTER_AREA);
    else
        petit = resim.clone();

    Mat gris, flou, kenar;

    cvtColor(petit, gris, COLOR_BGR2GRAY);
    GaussianBlur(gris, flou, Size(5,5), 1.0);
    Canny(flou, kenar, 50, 150);

    int w = petit.cols;
    int h = petit.rows;

    int minR = 10;
    int maxR = min(90, min(w,h)/2 - 2);
    int pas = 2;
    int nbAngle = 72;
    int esik = 25;
    int farkR = 10;

    if (maxR < minR) {
        cout << "resim cok kucuk" << endl;
        return 1;
    }

    int nbR = (maxR - minR) / pas + 1;

    vector<double> sinDeger(nbAngle);
    vector<double> cosDeger(nbAngle);

    for (int t = 0; t < nbAngle; t++) {
        double aci = 2.0 * CV_PI * t / nbAngle;
        sinDeger[t] = sin(aci);
        cosDeger[t] = cos(aci);
    }

    vector<int> oy3D((size_t)w*h*nbR, 0);
    vector<int> merkezOy((size_t)w*h, 0);

    auto adres = [&](int a, int b, int ri) -> size_t {
        return ((size_t)b*w + a)*nbR + ri;
    };

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            if (kenar.ptr<uchar>(y)[x] == 0)
                continue;

            for (int ri = 0; ri < nbR; ri++) {

                int r = minR + ri*pas;

                for (int t = 0; t < nbAngle; t++) {

                    int a = cvRound(x - r*cosDeger[t]);
                    int b = cvRound(y - r*sinDeger[t]);

                    if (a < 0 || a >= w || b < 0 || b >= h)
                        continue;

                    size_t pos = adres(a,b,ri);

                    oy3D[pos]++;

                    if (oy3D[pos] == esik) {

                        for (int k = 0; k < nbR; k++) {

                            if (abs(k-ri)*pas < farkR)
                                continue;

                            if (oy3D[adres(a,b,k)] >= esik)
                                merkezOy[(size_t)b*w+a]++;
                        }
                    }
                }
            }
        }
    }

    Mat sonuc = resim.clone();

    int bulunanMerkez = 0;
    int bulunanCember = 0;

    double oranX = (double)resim.cols / w;
    double oranY = (double)resim.rows / h;

    for (int b = 5; b < h-5; b++) {
        for (int a = 5; a < w-5; a++) {

            int puan = merkezOy[(size_t)b*w+a];

            if (puan == 0)
                continue;

            bool maksimum = true;

            for (int dy = -5; dy <= 5 && maksimum; dy++) {
                for (int dx = -5; dx <= 5; dx++) {

                    if (dx == 0 && dy == 0)
                        continue;

                    int xx = a + dx;
                    int yy = b + dy;

                    int komsu = merkezOy[(size_t)yy*w+xx];

                    if (komsu > puan ||
                        (komsu == puan &&
                        (yy < b || (yy == b && xx < a)))) {
                        maksimum = false;
                        break;
                    }
                }
            }

            if (!maksimum)
                continue;

            vector<pair<int,int>> candidats;

            for (int ri = 1; ri < nbR-1; ri++) {

                int v = oy3D[adres(a,b,ri)];

                if (v >= esik &&
                    v >= oy3D[adres(a,b,ri-1)] &&
                    v >= oy3D[adres(a,b,ri+1)]) {

                    candidats.push_back({v, minR+ri*pas});
                }
            }

            sort(candidats.begin(), candidats.end(),
                 [](const auto& x, const auto& y) {
                     return x.first > y.first;
                 });

            vector<int> rayons;

            for (auto c : candidats) {

                bool uygun = true;

                for (int r : rayons) {
                    if (abs(r-c.second) < farkR) {
                        uygun = false;
                        break;
                    }
                }

                if (uygun)
                    rayons.push_back(c.second);
            }

            if (rayons.size() < 2)
                continue;

            Point centre(cvRound(a*oranX), cvRound(b*oranY));

            for (int r : rayons) {

                int rayon = cvRound(r*(oranX+oranY)/2.0);

                circle(sonuc, centre, rayon,
                       Scalar(0,255,0), 2, LINE_AA);

                bulunanCember++;
            }

            circle(sonuc, centre, 3,
                   Scalar(0,0,255), FILLED);

            bulunanMerkez++;
        }
    }

    Mat centreMat(h,w,CV_32SC1);
    Mat tranche(h,w,CV_32SC1);

    int milieu = nbR/2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            centreMat.ptr<int>(y)[x] = merkezOy[(size_t)y*w+x];
            tranche.ptr<int>(y)[x] = oy3D[adres(x,y,milieu)];
        }
    }

    Mat centreVue, trancheVue;

    normalize(centreMat, centreVue, 0, 255,
              NORM_MINMAX, CV_8UC1);

    normalize(tranche, trancheVue, 0, 255,
              NORM_MINMAX, CV_8UC1);

    imwrite("results/05_edges.png", kenar);
    imwrite("results/05_votes_centres.png", centreVue);
    imwrite("results/05_votes_3d_slice.png", trancheVue);
    imwrite("results/05_concentric_circles.png", sonuc);

    cout << "oylama 3D : " << w << " x " << h
         << " x " << nbR << endl;

    cout << "merkez sayisi : " << bulunanMerkez << endl;
    cout << "cember sayisi : " << bulunanCember << endl;

    return 0;
}
