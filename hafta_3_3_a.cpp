#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/hafta_3.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Error: Image not found!" << endl;
        return 1;
    }

    // 2. Create the CLAHE object
    Ptr<CLAHE> clahe = createCLAHE();

    // 3. Apply CLAHE
    Mat result;
    clahe->apply(img, result);

    // 4. Save the result
    imwrite("/content/hafta_3_3_clahe_result.png", result);

    return 0;
}
