#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/hafta_3_5_salt_pepper.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return 1;
    }

    Mat result;

    // Apply OpenCV median filter with a 3x3 kernel
    medianBlur(img, result, 3);

    imwrite("/content/hafta_3_5_a_salt_pepper_filtered.png", result);

    return 0;
}
