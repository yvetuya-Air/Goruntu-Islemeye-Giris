#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/hafta_3_img.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Error: Image not found!" << endl;
        return 1;
    }

    int minVal = 255;
    int maxVal = 0;

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            int pixel = img.at<uchar>(y, x);

            if (pixel < minVal)
                minVal = pixel;

            if (pixel > maxVal)
                maxVal = pixel;
        }
    }

    cout << "Minimum value: " << minVal << endl;
    cout << "Maximum value: " << maxVal << endl;

    Mat result(img.rows, img.cols, CV_8UC1);

    if (maxVal == minVal) {
        cout << "All pixels have the same value; cannot scale." << endl;
        return 1;
    }

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            int oldPixel = img.at<uchar>(y, x);

            double newPixel =
                (double)(oldPixel - minVal) * 255.0
                / (maxVal - minVal);

            result.at<uchar>(y, x) = (uchar)(newPixel + 0.5);
        }
    }

    imwrite("/content/hafta_3_1_contrast_stretched.png", result);

    return 0;
}
