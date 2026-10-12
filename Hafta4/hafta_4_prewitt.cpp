#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/cow.jpg", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image bulunmadı!" << endl;
        return 1;
    }

    // PREWİT
    // Step 1: Light Gaussian smoothing
    Mat smoothed;
    GaussianBlur(img, smoothed, Size(3, 3), 0.8);

    // Step 2: Prewitt filter
    Mat result = Mat::zeros(img.size(), CV_8UC1);

    int kernel[3][3] = {
        {-1, 0, 1},
        {-1, 0, 1},
        {-1, 0, 1}
    };

    for (int y = 1; y < smoothed.rows - 1; y++) {
        for (int x = 1; x < smoothed.cols - 1; x++) {
            int sum = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += kernel[ky + 1][kx + 1] *
                           smoothed.at<uchar>(y + ky, x + kx);
                }
            }

            result.at<uchar>(y, x) =
                (uchar)min(255, abs(sum));
        }
    }

    imwrite("/content/hafta_4_gaussian_yumuşak.png", smoothed);
    imwrite("/content/hafta_4_prewitt_result.png", result);


    return 0;
}
