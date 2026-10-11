#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    // 1. Open image as grayscale
    Mat img = imread("/content/hafta_3.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Error: Image not found!" << endl;
        return 1;
    }

    // 2. Calculate the 256-bin histogram manually
    int histogram[256] = {0};

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            int pixel = img.at<uchar>(y, x);
            histogram[pixel]++;
        }
    }

    // 3. Calculate the CDF manually
    int cdf[256] = {0};
    cdf[0] = histogram[0];

    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // Find the first non-zero CDF value
    int cdfMin = 0;

    for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
            cdfMin = cdf[i];
            break;
        }
    }

    // 4. Equalize the histogram manually
    Mat result(img.rows, img.cols, CV_8UC1);
    int totalPixels = img.rows * img.cols;

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            int oldPixel = img.at<uchar>(y, x);

            int newPixel = 0;

            if (totalPixels > cdfMin) {
                newPixel = (int)(
                    (double)(cdf[oldPixel] - cdfMin) * 255
                    / (totalPixels - cdfMin)
                );
            }

            result.at<uchar>(y, x) = (uchar)newPixel;
        }
    }

    // 5. Save the output image
    imwrite("/content/hafta_3_equalized.png", result);

    cout << "Image size: " << img.cols << " x " << img.rows << endl;
    cout << "Total pixels: " << totalPixels << endl;
    cout << "CDF minimum: " << cdfMin << endl;

    cout << "\nParlaklık\tHistogram\tCDF" << endl;

    for (int i = 0; i < 256; i++) {
        cout << i << "\t\t"
             << histogram[i] << "\t\t"
             << cdf[i] << endl;
    }

    return 0;
}
