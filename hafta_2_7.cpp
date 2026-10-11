#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

// Function to multiply pixel values
void multiplyImage(Mat& img, double value) {
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            int pixel = img.at<uchar>(y, x);
            int result = pixel * value;

            if (result > 255) result = 255;
            if (result < 0) result = 0;

            img.at<uchar>(y, x) = result;
        }
    }
}

// Function to add a value to pixel values
void addImage(Mat& img, int value) {
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            int pixel = img.at<uchar>(y, x);
            int result = pixel + value;

            if (result > 255) result = 255;
            if (result < 0) result = 0;

            img.at<uchar>(y, x) = result;
        }
    }
}

int main() {
    Mat img = imread("/content/image1.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return -1;
    }

    // First image: only multiply by 0.75
    Mat img1 = img.clone();
    multiplyImage(img1, 0.75);

    // Second image: multiply by 0.75, then add 20
    Mat img2 = img.clone();
    multiplyImage(img2, 0.75);
    addImage(img2, 20);

    imwrite("/content/hafta_2_7_multiply.png", img1);
    imwrite("/content/hafta_2_7_multiply_add.png", img2);

    cout << "Original average: " << mean(img)[0] << endl;
    cout << "Multiply average: " << mean(img1)[0] << endl;
    cout << "Multiply + add average: " << mean(img2)[0] << endl;

    return 0;
}
