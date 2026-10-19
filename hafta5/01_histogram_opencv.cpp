#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char** argv) {
    string path = argc > 1 ? argv[1] : "cow.jpg";
    Mat img = imread(path, IMREAD_COLOR);
    if (img.empty()) { cerr << "Goruntu bulunamadi: " << path << '\n'; return 1; }
    const char* names[] = {"mavi", "yesil", "kirmizi"};
    const Scalar colors[] = {Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};
    int channels[] = {0}; int histSize=256; float range[] = {0,256}; const float* ranges[]={range};
    for (int c=0;c<3;++c) {
        channels[0]=c; Mat hist;
        calcHist(&img,1,channels,Mat(),hist,1,&histSize,ranges,true,false);
        double maxVal; minMaxLoc(hist,nullptr,&maxVal);
        Mat plot(300,512,CV_8UC3,Scalar(0,0,0));
        for (int i=1;i<256;++i) {
            int h1=cvRound(hist.at<float>(i-1)*299.0/max(1.0,maxVal));
            int h2=cvRound(hist.at<float>(i)*299.0/max(1.0,maxVal));
            line(plot,Point(2*(i-1),299-h1),Point(2*i,299-h2),colors[c],1);
        }
        string out=string("01_hist_")+names[c]+".png";
        imwrite(out,plot); cout << out << " kaydedildi\n";
    }
    return 0;
}
