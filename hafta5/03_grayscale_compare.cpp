#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdlib>
using namespace cv;
using namespace std;
Mat myGrayscale(const Mat& img) {
    Mat gray(img.rows,img.cols,CV_8UC1);
    for(int y=0;y<img.rows;++y){
        const Vec3b* src=img.ptr<Vec3b>(y);
        uchar* dst=gray.ptr<uchar>(y);
        for(int x=0;x<img.cols;++x){
            int B=src[x][0], G=src[x][1], R=src[x][2];
            dst[x]=static_cast<uchar>((3735*B+19235*G+9798*R+16384)>>15);
        }
    }
    return gray;
}
int main(int argc,char** argv){
    string path=argc>1?argv[1]:"cow.jpg";
    Mat img=imread(path,IMREAD_COLOR);
    if(img.empty()){cerr<<"Goruntu bulunamadi: "<<path<<'\n';return 1;}
    Mat reference; cvtColor(img,reference,COLOR_BGR2GRAY);
    Mat manual=myGrayscale(img);
    long long error=0; int differentPixels=0;
    for(int y=0;y<img.rows;++y){
        const uchar* a=manual.ptr<uchar>(y); const uchar* b=reference.ptr<uchar>(y);
        for(int x=0;x<img.cols;++x){
            int d=abs(int(a[x])-int(b[x]));error+=d; if(d)++differentPixels;
        }
    }
    imwrite("03_gri_opencv.png",reference);
    imwrite("03_gri_manual.png",manual);
    cout<<"Toplam mutlak hata: "<<error<<"\nFarkli piksel sayisi: "<<differentPixels<<'\n';
    return 0;
}
