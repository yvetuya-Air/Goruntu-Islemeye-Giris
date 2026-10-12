#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/cow.jpg", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image bulunmadı!" << endl;
        return 1;
    }

{
    // PREWİT
    auto start = std::chrono::high_resolution_clock::now();

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

    auto end = std::chrono::high_resolution_clock::now();
    cout << "PREWİT: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << endl;
    //imwrite("/content/hafta_4_gaussian_yumuşak.png", smoothed);
    //imwrite("/content/hafta_4_prewitt_result.png", result);

}
{
    // SOBEL DIKEY
    auto start = std::chrono::high_resolution_clock::now();

    // Step 1: Light Gaussian smoothing
    Mat smoothed;
    GaussianBlur(img, smoothed, Size(3, 3), 0.8);

    // Step 2: Sobel D'key filter
    Mat result = Mat::zeros(img.size(), CV_8UC1);

    int kernel[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
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

    auto end = std::chrono::high_resolution_clock::now();
    cout << "SOBEL DIKEY: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << endl;
    //imwrite("/content/hafta_4_gaussian_yumuşak.png", smoothed);
    //imwrite("/content/hafta_4_sobel_dikey_result.png", result);

}
{
    // SOBEL YATAY
    auto start = std::chrono::high_resolution_clock::now();

    // Step 1: Light Gaussian smoothing
    Mat smoothed;
    GaussianBlur(img, smoothed, Size(3, 3), 0.8);

    // Step 2: Sobel D'key filter
    Mat result = Mat::zeros(img.size(), CV_8UC1);

    int kernel[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
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


    auto end = std::chrono::high_resolution_clock::now();
    cout << "SOBEL YATAY: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << endl;
    //imwrite("/content/hafta_4_gaussian_yumuşak.png", smoothed);
    //imwrite("/content/hafta_4_sobel_yatay_result.png", result);


}
{
    // LAPLACIAN 3X3
    auto start = std::chrono::high_resolution_clock::now();

    Mat smoothed;
    GaussianBlur(img, smoothed, Size(3, 3), 0.8);

    //  Laplacian filter
    Mat result = Mat::zeros(img.size(), CV_8UC1);

    int kernel[3][3] = {
      { 0, -1,  0},
      {-1,  4, -1},
      { 0, -1,  0}
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

        // Normalize for visibility
    Mat result_normalize;
    normalize(result, result_normalize, 0, 255, NORM_MINMAX);


    auto end = std::chrono::high_resolution_clock::now();
    cout << "LAPLACIAN 3X3: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << endl;
    //imwrite("/content/hafta_4_gaussian_yumuşak.png", smoothed);
    //imwrite("/content/hafta_4_laplacian_normalize.png", result_normalize);

}
{
    // LAPLACIAN 7X7
    auto start = std::chrono::high_resolution_clock::now();


    // 7x7 LoG kernel
    int kernel[7][7] = {
        { 0,  0, -1, -1, -1,  0,  0},
        { 0, -1, -3, -3, -3, -1,  0},
        {-1, -3,  0,  7,  0, -3, -1},
        {-1, -3,  7, 32,  7, -3, -1},
        {-1, -3,  0,  7,  0, -3, -1},
        { 0, -1, -3, -3, -3, -1,  0},
        { 0,  0, -1, -1, -1,  0,  0}
    };

    Mat response = Mat::zeros(img.size(), CV_32F);

    // Manual convolution
    for (int y = 3; y < img.rows - 3; y++) {
        for (int x = 3; x < img.cols - 3; x++) {
            int sum = 0;

            for (int ky = -3; ky <= 3; ky++) {
                for (int kx = -3; kx <= 3; kx++) {
                    sum += kernel[ky + 3][kx + 3] *
                           img.at<uchar>(y + ky, x + kx);
                }
            }

            response.at<float>(y, x) = abs(sum);
        }
    }

    Mat result;
    normalize(response, result, 0, 255, NORM_MINMAX);
    result.convertTo(result, CV_8UC1);



    auto end = std::chrono::high_resolution_clock::now();
    cout << "LAPLACIAN 7X7: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << endl;
    //imwrite("/content/hafta_4_laplacian_normalize_7x7.png", result);
}
{
    //CANNY
    auto start = std::chrono::high_resolution_clock::now();

    // Gaussian smoothing
    Mat smoothed;
    GaussianBlur(img, smoothed, Size(3, 3), 0.8);

    // Canny edge detection
    Mat result;
    Canny(smoothed, result, 50, 150);


    auto end = std::chrono::high_resolution_clock::now();
    cout << "CANNY: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms" << endl;
    //imwrite("/content/hafta_4_canny.png", result);


}

}
