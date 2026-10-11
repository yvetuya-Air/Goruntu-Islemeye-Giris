#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>
using namespace cv;
using namespace std;
using namespace chrono;

void parlaklık_Ayarlama(Mat& img, int x1, int y1, int x2, int y2, int value) {
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int pixel = img.at<uchar>(y, x) + value;
            if (pixel > 255) pixel = 255;
            if (pixel < 0) pixel = 0;
            img.at<uchar>(y, x) = pixel;
        }
    }
}

int main() {
    Mat original = imread("/content/image1.png", IMREAD_GRAYSCALE);
    if (original.empty()) {
        cout << "Image not found!" << endl;
        return -1;
    }

    int w = original.cols, h = original.rows;
    int mx = w / 2, my = h / 2;

    // 1 thread
    Mat img1 = original.clone();
    auto start1 = high_resolution_clock::now();

    parlaklık_Ayarlama(img1, 0, 0, mx, my, -60);
    parlaklık_Ayarlama(img1, mx, 0, w, my, -20);
    parlaklık_Ayarlama(img1, 0, my, mx, h, 20);
    parlaklık_Ayarlama(img1, mx, my, w, h, 60);

    auto end1 = high_resolution_clock::now();

    // 4 threads
    Mat img4 = original.clone();
    auto start4 = high_resolution_clock::now();

    thread t1(parlaklık_Ayarlama, ref(img4), 0, 0, mx, my, -60);
    thread t2(parlaklık_Ayarlama, ref(img4), mx, 0, w, my, -20);
    thread t3(parlaklık_Ayarlama, ref(img4), 0, my, mx, h, 20);
    thread t4(parlaklık_Ayarlama, ref(img4), mx, my, w, h, 60);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    auto end4 = high_resolution_clock::now();

    double time1 = duration<double, milli>(end1 - start1).count();
    double time4 = duration<double, milli>(end4 - start4).count();

    cout << "1 thread: " << time1 << " ms" << endl;
    cout << "4 threads: " << time4 << " ms" << endl;

    imwrite("/content/hafta_2_4_1thread.png", img1);
    imwrite("/content/hafta_2_4_4threads.png", img4);

    return 0;
}
