#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
using namespace cv;
using namespace std;

int main() {
    Mat img = imread("/content/hafta_3.png", IMREAD_GRAYSCALE);

    if (img.empty()) {
        cout << "Image not found!" << endl;
        return 1;
    }

    int tilesX = 8, tilesY = 8;
    int clipLimit = 40;

    int mapping[8][8][256] = {};

    // Steps 1-4: Build a mapping for every tile
    for (int ty = 0; ty < tilesY; ty++) {
        for (int tx = 0; tx < tilesX; tx++) {
            int xStart = tx * img.cols / tilesX;
            int xEnd = (tx + 1) * img.cols / tilesX;
            int yStart = ty * img.rows / tilesY;
            int yEnd = (ty + 1) * img.rows / tilesY;

            int histogram[256] = {};

            for (int y = yStart; y < yEnd; y++) {
                for (int x = xStart; x < xEnd; x++) {
                    histogram[img.at<uchar>(y, x)]++;
                }
            }

            // Clip histogram
            int excess = 0;
            for (int i = 0; i < 256; i++) {
              int tileArea = (xEnd - xStart) * (yEnd - yStart);
              int actualClipLimit = max(1, clipLimit * tileArea / 256);

              int excess = 0;

              for (int i = 0; i < 256; i++) {
                  if (histogram[i] > actualClipLimit) {
                      excess += histogram[i] - actualClipLimit;
                      histogram[i] = actualClipLimit;
                  }
              }
            }

            // Redistribute excess counts without losing pixels
            int each = excess / 256;
            int remainder = excess % 256;

            for (int i = 0; i < 256; i++)
                histogram[i] += each;

            for (int i = 0; i < remainder; i++)
                histogram[i]++;

            int cdf[256];
            cdf[0] = histogram[0];

            for (int i = 1; i < 256; i++)
                cdf[i] = cdf[i - 1] + histogram[i];

            int cdfMin = 0;
            for (int i = 0; i < 256; i++) {
                if (cdf[i] > 0) {
                    cdfMin = cdf[i];
                    break;
                }
            }

            int total = (xEnd - xStart) * (yEnd - yStart);

            for (int i = 0; i < 256; i++) {
                if (total > cdfMin) {
                    mapping[ty][tx][i] = (int)(
                        (double)(cdf[i] - cdfMin) * 255
                        / (total - cdfMin)
                    );
                } else {
                    mapping[ty][tx][i] = i;
                }
            }
        }
    }

    // Step 5: Apply mappings with bilinear interpolation
    Mat result(img.rows, img.cols, CV_8UC1);

    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            double gx = ((x + 0.5) * tilesX / img.cols) - 0.5;
            double gy = ((y + 0.5) * tilesY / img.rows) - 0.5;

            int x0 = (int)floor(gx);
            int y0 = (int)floor(gy);

            double dx = gx - x0;
            double dy = gy - y0;

            int x1 = x0 + 1;
            int y1 = y0 + 1;

            x0 = max(0, min(x0, tilesX - 1));
            x1 = max(0, min(x1, tilesX - 1));
            y0 = max(0, min(y0, tilesY - 1));
            y1 = max(0, min(y1, tilesY - 1));

            dx = max(0.0, min(dx, 1.0));
            dy = max(0.0, min(dy, 1.0));

            int pixel = img.at<uchar>(y, x);

            double top =
                mapping[y0][x0][pixel] * (1.0 - dx) +
                mapping[y0][x1][pixel] * dx;

            double bottom =
                mapping[y1][x0][pixel] * (1.0 - dx) +
                mapping[y1][x1][pixel] * dx;

            double value = top * (1.0 - dy) + bottom * dy;

            result.at<uchar>(y, x) = (uchar)value;
        }
    }

    imwrite("/content/hafta_3_3_b_clahe_manual.png", result);
    cout << "Manual CLAHE result saved." << endl;

    return 0;
}
