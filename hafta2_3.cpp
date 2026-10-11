#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
using namespace cv;
using namespace std;

void parlaklık_Ayarlama_Bolgede(Mat& img, int x1, int y1, int x2, int y2, int value) {
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
    Mat img = imread("/content/image1.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return -1;
    }

    int w = img.cols;
    int h = img.rows;
    int midX = w / 2;
    int midY = h / 2;

    thread t1(parlaklık_Ayarlama_Bolgede, ref(img), 0, 0, midX, midY, -60);
    thread t2(parlaklık_Ayarlama_Bolgede, ref(img), midX, 0, w, midY, -20);
    thread t3(parlaklık_Ayarlama_Bolgede, ref(img), 0, midY, midX, h, 20);
    thread t4(parlaklık_Ayarlama_Bolgede, ref(img), midX, midY, w, h, 60);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    imwrite("/content/hafta2_3_threads.png", img);
    cout << "Result saved!" << endl;

    return 0;
}
