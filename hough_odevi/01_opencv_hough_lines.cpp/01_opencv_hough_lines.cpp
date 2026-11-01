#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem> 

using namespace cv;
using namespace std;
namespace fs = std::filesystem
int main(){
  Mat imag = imread("images/image.jpg",IMREAD_COLOR);
if(img.empty()){
cout <<"goruntu okunamadi"<<endl;
return 1;
}
fs:: create_directories("results");
Mat gray;
cvtColor(img,gray,COLOR_BGR2GRAY);
Mat edges;
canny(gray,edges,50,150);
Mat filtered;
medianBlur(edges,filtered,3); 
vector<Vec2f> lines;
HoughtLines(filtered,lines,1,CV_PI/180,60);
Mat result1 = img.clone();
for (Vec2f 1 : lines){
  double rho = 1[0];
  double theta = 1[1];

double a = cost(theta);
double b = sin(theta);

double x0 = a * rho;
double y0 = b * rho;

point p1(
cvRound(x0 + 2000 * (-b)),
cvRound(y0 + 2000 * a)
);

point p2(
cvRound(x0 - 2000 * (-b))
cvRound(y0 -2000 * a)
);
line(result1,p1,p2,Scalar(0,0,255), 2);
vector<Vec4i> segments;

HoughLinesP(filtered,segments,1,CV_PI/180,40,30,10);
Mat result2 = img.clone();
for (Vec4i s : segments){
line(result2,
point(s[0],s[1]),
point(s[2],s[3]),
Scalar(0,,255,0),2);
 imwrite("results/01_canny.png", edges);
    imwrite("results/01_median.png", filtered);
    imwrite("results/01_hough_lines.png", result1);
    imwrite("results/01_hough_lines_p.png", result2);

    cout << "HoughLines: " << lines.size() << endl;
    cout << "HoughLinesP: " << segments.size() << endl;
    return o;
}


