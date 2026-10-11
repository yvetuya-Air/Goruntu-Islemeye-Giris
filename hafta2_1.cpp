#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat image = cv::imread("/content/image1.png");

    if (image.empty()) {
        std::cout << "Goruntu okunamadi!" << std::endl;
        return 1;
    }

    cv::Mat resizedImage;

    cv::resize(image, resizedImage, cv::Size(1024, 768));

    if (cv::imwrite("/content/resized_image.png", resizedImage)) {
    std::cout << "Goruntu basariyla kaydedildi!" << std::endl;
    } else {
        std::cout << "Goruntu kaydedilemedi!" << std::endl;
    }

    std::cout << "Yeni genislik: " << resizedImage.cols << std::endl;
    std::cout << "Yeni yukseklik: " << resizedImage.rows << std::endl;

    return 0;
}
