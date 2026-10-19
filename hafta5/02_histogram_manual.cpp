#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
// Hazir calcHist fonksiyonu kullanilmadan histogram hesaplanir.
void myCalcHist(const Mat& img, int channel, int hist[256]) {
    for (int i=0;i<256;++i) hist[i]=0;
    for (int y=0;y<img.rows;++y) {
        const Vec3b* row=img.ptr<Vec3b>(y);
        for (int x=0;x<img.cols;++x) ++hist[row[x][channel]];
    }
}
Mat drawHist(const int hist[256], const Scalar& color) {
    int maximum=1;
    for (int i=0;i<256;++i) if(hist[i]>maximum) maximum=hist[i];
    Mat plot(300,512,CV_8UC3,Scalar(0,0,0));
    for (int i=1;i<256;++i) {
        int h1=hist[i-1]*299/maximum, h2=hist[i]*299/maximum;
        line(plot,Point(2*(i-1),299-h1),Point(2*i,299-h2),color,1);
    }
    return plot;
}
int main(int argc,char** argv) {
    string path=argc>1?argv[1]:"cow.jpg";
    Mat img=imread(path,IMREAD_COLOR);
    if(img.empty()){cerr<<"Goruntu bulunamadi: "<<path<<'\n';return 1;}
    const char* names[]={"mavi","yesil","kirmizi"};
    const Scalar colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255)};
    for(int c=0;c<3;++c){
        int hist[256]; myCalcHist(img,c,hist);
        string out=string("02_hist_")+names[c]+".png";
        imwrite(out,drawHist(hist,colors[c]));
        cout<<out<<" kaydedildi\n";
    }
    return 0;
}
