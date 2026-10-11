#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
using namespace cv;
using namespace std;

void calculateHistogram(Mat& img, int x1, int y1, int x2, int y2, int hist[256]) {
    for (int i = 0; i < 256; i++)
        hist[i] = 0;

    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int pixel = img.at<uchar>(y, x);
            hist[pixel]++;
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

    int hist1[256], hist2[256], hist3[256], hist4[256];
    int mainHist[256] = {0};

    thread t1(calculateHistogram, ref(img), 0, 0, mx, my, hist1);
    thread t2(calculateHistogram, ref(img), mx, 0, w, my, hist2);
    thread t3(calculateHistogram, ref(img), 0, my, mx, h, hist3);
    thread t4(calculateHistogram, ref(img), mx, my, w, h, hist4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    for (int i = 0; i < 256; i++)
        mainHist[i] = hist1[i] + hist2[i] + hist3[i] + hist4[i];

    cout << "Intensity\tPixel Count" << endl;

    for (int i = 0; i < 256; i++) {
        cout << i << "\t\t" << mainHist[i] << endl;
    }

    return 0;
}
