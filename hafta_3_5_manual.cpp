#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/hafta_3_5_salt_pepper.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return 1;
    }

    Mat result = img.clone();

    for (int y = 1; y < img.rows - 1; y++) {
        for (int x = 1; x < img.cols - 1; x++) {
            uchar values[9];
            int k = 0;

            // Collect the 3x3 neighborhood
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    values[k++] = img.at<uchar>(y + ky, x + kx);
                }
            }

            // Sort the 9 values
            sort(values, values + 9);

            // Middle value is the median
            result.at<uchar>(y, x) = values[4];
        }
    }

    imwrite("/content/hafta_3_5_a_salt_pepper_manual_filtered.png", result);


    return 0;
}
