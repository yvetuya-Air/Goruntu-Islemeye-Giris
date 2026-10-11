#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/hafta_3_4_image.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return 1;
    }

    Mat result = img.clone();

    // 3x3 mean filter using convolution
    for (int y = 1; y < img.rows - 1; y++) {
        for (int x = 1; x < img.cols - 1; x++) {
            int sum = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += img.at<uchar>(y + ky, x + kx);
                }
            }

            result.at<uchar>(y, x) = sum / 9;
        }
    }

    imwrite("/content/hafta_4_filtered.png", result);

    return 0;
}
