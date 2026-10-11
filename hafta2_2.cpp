#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat image = cv::imread("/content/image1.png", cv::IMREAD_GRAYSCALE );

    if (image.empty()) {
        std::cout << "Image could not be read!" << std::endl;
        return 1;
    }

    cv::imwrite("/content/hafta2_2_original_8bit.png", image);

    cv::Mat quantized6bit = image / 4;

    cv::Mat display6bit = quantized6bit * 4;

    cv::imwrite("/content/hafta2_2_quantized_6bit.png", display6bit);

    std::cout << "Both images saved successfully!" << std::endl;

    return 0;
}
