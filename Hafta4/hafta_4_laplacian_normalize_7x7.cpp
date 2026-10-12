#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/cow.jpg", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return 1;
    }

    Mat smoothed;
    GaussianBlur(img, smoothed, Size(3, 3), 0.8);

    // 7x7 LoG kernel
    int kernel[7][7] = {
        { 0,  0, -1, -1, -1,  0,  0},
        { 0, -1, -3, -3, -3, -1,  0},
        {-1, -3,  0,  7,  0, -3, -1},
        {-1, -3,  7, 24,  7, -3, -1},
        {-1, -3,  0,  7,  0, -3, -1},
        { 0, -1, -3, -3, -3, -1,  0},
        { 0,  0, -1, -1, -1,  0,  0}
    };

    Mat response = Mat::zeros(smoothed.size(), CV_32F);

    // Manual convolution
    for (int y = 3; y < smoothed.rows - 3; y++) {
        for (int x = 3; x < smoothed.cols - 3; x++) {
            int sum = 0;

            for (int ky = -3; ky <= 3; ky++) {
                for (int kx = -3; kx <= 3; kx++) {
                    sum += kernel[ky + 3][kx + 3] *
                           smoothed.at<uchar>(y + ky, x + kx);
                }
            }

            response.at<float>(y, x) = abs(sum);
        }
    }

    Mat result;
    normalize(response, result, 0, 255, NORM_MINMAX);
    result.convertTo(result, CV_8UC1);

    imwrite("/content/hafta_4_laplacian_normalize_7x7.png", result);

    cout << "7x7 LoG filtering completed." << endl;

    return 0;
}
