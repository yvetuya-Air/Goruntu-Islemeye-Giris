#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
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

    Mat gray,edges, filtered;

   cvtColor(img, gray, COLOR_BGR2GRAY);
Canny( gray, edges, 50,150);
mediabBlur(edges, filtered, 3);

const int thetaBins = 180;
const int rhoMax = (int)ceil(hypot(img.cols, img.rows));
const int rhoBins = 2 * rhoMax + 1;
const int threshold  = 60;
vector<double> sintable(thetaBins);
cector<double> cosTable(thetaBins);

for (int t = 0; t< thetaBins;t++){
double theta = t * CV_PI / 180.0;

sinTable[t] = sin(theta);
cosTable[t] = cos(theta);
}

Mat votes Mat::zeros(rhoBins,thetaBIns,CV_32SC1);

for (int y = 0 ; y<filtered.rows; y++){
const uchar* row = filtered.ptr<uchar>(y);
for (int x =0; x < filtered.cols; x++){
if (row[x] ==x0)
  continue;

for (int t = 0; t < thetaBins; t++){
int rho cvRound(
x * cosTable[t] + y * sinTable[t]
);
int r = rho + rhoMax;
if (r>= 0 && r < rhoBins){
votes.ptr<int>(r)[t]++;
}
}
}
}



Mat voteImage;

normalize(votes, voteImage, 0, 255,NORM_MINMAX, CV_8UC1);


imwrite("result2/02_votes.pnp", voteImage);

Mat result = img.clone();
int found = 0;


for(int r=2; rhoBins -2; r++){
  for(int t = 0; t < thetaBins; t++){
int v = votes.ptr<int>(r)[t];

if (v < threshold)
   continue;

bool maximum = true;

for(int dr = -2; dr <= 2 && maximum; dr++) {
  for(int dt = -2; dt <= 2; dt++) {
       int nr = r + dr;
       int nt = t + dt;
if (nt < 0 || nt >= thetaBins)
                        continue;
ifif (votes.ptr<int>(nr)[nt] > v) {
                        maximum = false;
                        break;
}
  }
}

if (!maximum)
    continue;

 double rho = r - rhoMax;
 double theta = t * CV_PI / 180.0;

double a = cosTable[t];
double b = sinTable[t];

double x0 = a * rho;
double y0 = b * rho;


point p1(
      cvRound(x0 + 2000 * (-b)),
      cvRound(y0 + 2000 * a)
);

line(result, p1, p2,
     Scalar(0, 0, 255), 2, LINE_AA);

found++;
  }
}

imwrite("results/02_edges.png", filtered);
imwrite("results/02_manual_lines.png", result);


cout <<"Oylama matrisi:"
     << found <<endl;
return 0;


 

