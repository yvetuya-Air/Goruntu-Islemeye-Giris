#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>
using namespace cv;
using namespace std;
using namespace chrono;

void calculateHistogram(Mat& img, int hist[256]) {
    for (int i = 0; i < 256; i++)
        hist[i] = 0;

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            hist[img.at<uchar>(y, x)]++;
        }
    }
}

int main() {
    Mat img = imread("/content/image1.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return -1;
    }

    int w = img.cols, h = img.rows;
    int mx = w / 2, my = h / 2;

    Mat sub_img1 = img(Rect(0, 0, mx, my));
    Mat sub_img2 = img(Rect(mx, 0, w - mx, my));
    Mat sub_img3 = img(Rect(0, my, mx, h - my));
    Mat sub_img4 = img(Rect(mx, my, w - mx, h - my));

    int hist1[256], hist2[256], hist3[256], hist4[256];
    int mainHist[256] = {0};

    int hist_1_thread[256];

    auto start1 = high_resolution_clock::now();
    calculateHistogram(img, hist_1_thread);
    auto end1 = high_resolution_clock::now();

    // Four-thread histogram
    auto start4 = high_resolution_clock::now();

    thread t1(calculateHistogram, ref(sub_img1), hist1);
    thread t2(calculateHistogram, ref(sub_img2), hist2);
    thread t3(calculateHistogram, ref(sub_img3), hist3);
    thread t4(calculateHistogram, ref(sub_img4), hist4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    for (int i = 0; i < 256; i++) {
        mainHist[i] = hist1[i] + hist2[i] + hist3[i] + hist4[i];
    }

    auto end4 = high_resolution_clock::now();

    double time1 = duration<double, milli>(end1 - start1).count();
    double time4 = duration<double, milli>(end4 - start4).count();

    cout << "1 thread: " << time1 << " ms" << endl;
    cout << "4 threads: " << time4 << " ms" << endl;

    cout << "\nIntensity \t1 Thread Hist \t4 Threads Hist  \n";
    for (int i = 0; i < 256; i++) {
        cout << i << "\t\t" << mainHist[i] << "\t\t" << hist_1_thread[i] << endl;
    }

    return 0;
}
