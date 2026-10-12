#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/cow.jpg", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return 1;
    }

    // Gaussian smoothing
    Mat smoothed;
    GaussianBlur(img, smoothed, Size(3, 3), 0.8);

    // Canny edge detection
    Mat result;
    Canny(smoothed, result, 50, 150);

    imwrite("/content/hafta_4_canny.png", result);

    cout << "Canny completed." << endl;

    return 0;
}
