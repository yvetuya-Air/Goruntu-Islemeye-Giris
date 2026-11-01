#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

__global__ void voteCuda(int2* points, int n, float* cosT,
                         float* sinT, int* oy, int rhoMax, int nRho) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i >= n) { return; }

    int x = points[i].x;
    int y = points[i].y;

    for (int t = 0; t < 180; t++) {
        int r = __float2int_rn(x*cosT[t] + y*sinT[t]) + rhoMax;

        if (r >= 0 && r < nRho) {
            atomicAdd(&oy[r*180 + t], 1);
        }
    }
}

int main() {
    Mat resim = imread("images/image.jpg");

    if (resim.empty()) {
        cout << "resim okunamadi" << endl;
        return 1;
    }

    int cihaz = 0;
    if (cudaGetDeviceCount(&cihaz) != cudaSuccess || cihaz == 0) {
        cout << "CUDA GPU bulunamadi" << endl;
        return 1;
    }

    fs::create_directories("results");

    Mat gris, kenar, filtre;
    cvtColor(resim, gris, COLOR_BGR2GRAY);
    Canny(gris, kenar, 50, 150);
    medianBlur(kenar, filtre, 3);

    vector<int2> points;

    for (int y = 0; y < filtre.rows; y++) {
        for (int x = 0; x < filtre.cols; x++) {
            if (filtre.ptr<uchar>(y)[x] > 0) {
                points.push_back(make_int2(x,y));
            }
        }
    }

    int n = points.size();

    if (n == 0) {
        cout << "kenar bulunamadi" << endl;
        return 1;
    }

    vector<float> cosT(180), sinT(180);

    for (int t = 0; t < 180; t++) {
        float angle = t * CV_PI / 180.0;
        cosT[t] = cos(angle);
        sinT[t] = sin(angle);
    }

    int rhoMax = ceil(hypot(resim.cols, resim.rows));
    int nRho = 2*rhoMax + 1;
    int total = nRho*180;

    vector<int> oyCPU(total, 0);
    vector<int> oyGPU(total, 0);

    auto debut = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
        int x = points[i].x;
        int y = points[i].y;

        for (int t = 0; t < 180; t++) {
            int r = nearbyint(x*cosT[t] + y*sinT[t]) + rhoMax;

            if (r >= 0 && r < nRho) {
                oyCPU[r*180 + t]++;
            }
        }
    }

    auto fin = chrono::high_resolution_clock::now();

    double tempsCPU = chrono::duration<double, milli>(fin-debut).count();

    int2* dPoints;
    float *dCos, *dSin;
    int* dOy;

    cudaMalloc((void**)&dPoints, n*sizeof(int2));
    cudaMalloc((void**)&dCos, 180*sizeof(float));
    cudaMalloc((void**)&dSin, 180*sizeof(float));
    cudaMalloc((void**)&dOy, total*sizeof(int));

    cudaMemcpy(dPoints, points.data(), n*sizeof(int2), cudaMemcpyHostToDevice);
    cudaMemcpy(dCos, cosT.data(), 180*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dSin, sinT.data(), 180*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemset(dOy, 0, total*sizeof(int));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);

    voteCuda<<<(n+255)/256, 256>>>(dPoints, n, dCos, dSin,
                                   dOy, rhoMax, nRho);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    cudaError_t erreur = cudaGetLastError();

    if (erreur != cudaSuccess) {
        cout << cudaGetErrorString(erreur) << endl;
        return 1;
    }

    float tempsGPU = 0;
    cudaEventElapsedTime(&tempsGPU, start, stop);

    cudaMemcpy(oyGPU.data(), dOy, total*sizeof(int), cudaMemcpyDeviceToHost);

    long long fark = 0;

    for (int i = 0; i < total; i++) {
        fark += abs(oyCPU[i] - oyGPU[i]);
    }

    Mat matrice(nRho, 180, CV_32SC1, oyGPU.data());
    Mat affichage;

    normalize(matrice, affichage, 0, 255, NORM_MINMAX, CV_8UC1);
    imwrite("results/06_cuda_votes.png", affichage);

    Mat sonuc = resim.clone();
    int esik = 60;
    int sayi = 0;

    for (int r = 2; r < nRho-2; r++) {
        for (int t = 2; t < 178; t++) {
            int v = oyGPU[r*180+t];

            if (v < esik) { continue; }

            bool uygun = true;

            for (int dr = -2; dr <= 2 && uygun; dr++) {
                for (int dt = -2; dt <= 2; dt++) {
                    int k = oyGPU[(r+dr)*180+t+dt];

                    if (k > v || (k == v &&
                       (dr < 0 || (dr == 0 && dt < 0)))) {
                        uygun = false;
                        break;
                    }
                }
            }

            if (!uygun) { continue; }

            double rho = r-rhoMax;
            double a = cosT[t];
            double b = sinT[t];
            double x0 = rho*a;
            double y0 = rho*b;

            Point p1(cvRound(x0-2000*b), cvRound(y0+2000*a));
            Point p2(cvRound(x0+2000*b), cvRound(y0-2000*a));

            line(sonuc, p1, p2, Scalar(0,0,255), 2);
            sayi++;
        }
    }

    imwrite("results/06_cuda_lines.png", sonuc);

    cout << "CPU sure : " << tempsCPU << " ms" << endl;
    cout << "CUDA sure : " << tempsGPU << " ms" << endl;
    cout << "Oy farki : " << fark << endl;
    cout << "Bulunan dogru : " << sayi << endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    cudaFree(dPoints);
    cudaFree(dCos);
    cudaFree(dSin);
    cudaFree(dOy);

    return 0;
}
