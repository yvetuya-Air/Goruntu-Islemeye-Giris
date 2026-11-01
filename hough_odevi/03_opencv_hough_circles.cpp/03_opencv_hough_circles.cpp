#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

int main() {
Mat img = imread("images/image.jpg", IMREAD_COLOR);

    if (img.empty()) {
        cout << "Goruntu okunamadi!" << endl;
        return 1;
    }

    fs::create_directories("results");

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

Mat edges;
Canny(gray, edges, 50, 150);

Mat filtered;
medianBulur(edges,filtered, 3);
vector<Vec3f>circles;

HoughCircles(
filtered,
        circles,
        HOUGH_GRADIENT,
        1,
        20,
        100,
        18,
        10,
        150
  );
Mat result = img.clone();

for(Vec3f c : circles){

Point center(cvRound(c[0]), cvRound(c[1]));
int radius = cvRound(c[2]);

circle(result, center, radius,
               Scalar(0, 255, 0), 2, LINE_AA);

circle(result, center, 3,
               Scalar(0, 0, 255), FILLED);
    }

imwrite("results/03_canny.png", edges);
    imwrite("results/03_median.png", filtered);
    imwrite("results/03_hough_circles.png", result);

    cout << "Bulunan cember sayisi: "
         << circles.size() << endl;

    return 0;
}




