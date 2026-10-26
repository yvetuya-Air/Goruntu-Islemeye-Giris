#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cstdlib>
using namespace cv;
using namespace std;

// Donusum fonksiyonlarinin ortak tipi (function pointer).
using Conversion = Mat (*)(const Mat&);

Mat bgrToRgb(const Mat& img) {
    Mat out(img.size(), CV_8UC3);
    for (int y=0; y<img.rows; ++y) {
        const Vec3b* src=img.ptr<Vec3b>(y);
        Vec3b* dst=out.ptr<Vec3b>(y);
        for (int x=0; x<img.cols; ++x)
            dst[x]=Vec3b(src[x][2],src[x][1],src[x][0]);
    }
    return out;
}

Mat bgrToGray(const Mat& img) {
    Mat out(img.size(), CV_8UC1);
    for (int y=0; y<img.rows; ++y) {
        const Vec3b* src=img.ptr<Vec3b>(y);
        uchar* dst=out.ptr<uchar>(y);
        for (int x=0; x<img.cols; ++x) {
            int b=src[x][0],g=src[x][1],r=src[x][2];
            dst[x]=(uchar)((3735*b+19235*g+9798*r+16384)>>15);
        }
    }
    return out;
}

Mat bgrToHsv(const Mat& img) {
    Mat out(img.size(), CV_8UC3);
    const int shift=12, scale=1<<shift;
    int sdiv[256]={0},hdiv[256]={0};
    for(int i=1;i<256;++i) {
        sdiv[i]=cvRound((double)(255*scale)/i);
        hdiv[i]=cvRound((double)(180*scale)/(6*i));
    }
    for(int y=0;y<img.rows;++y) {
        const Vec3b* src=img.ptr<Vec3b>(y);
        Vec3b* dst=out.ptr<Vec3b>(y);
        for(int x=0;x<img.cols;++x) {
            int b=src[x][0],g=src[x][1],r=src[x][2];
            int v=max(r,max(g,b)), mn=min(r,min(g,b)), d=v-mn;
            int s=(d*sdiv[v]+(scale>>1))>>shift;
            int h=0;
            if(d) {
                if(v==r) h=g-b;
                else if(v==g) h=b-r+2*d;
                else h=r-g+4*d;
                h=(h*hdiv[d]+(scale>>1))>>shift;
                if(h<0) h+=180;
            }
            dst[x]=Vec3b((uchar)h,(uchar)s,(uchar)v);
        }
    }
    return out;
}

Mat bgrToYCrCb(const Mat& img) {
    Mat out(img.size(), CV_8UC3);
    for(int y=0;y<img.rows;++y) {
        const Vec3b* src=img.ptr<Vec3b>(y);
        Vec3b* dst=out.ptr<Vec3b>(y);
        for(int x=0;x<img.cols;++x) {
            int b=src[x][0],g=src[x][1],r=src[x][2];
            int Y=(4899*r+9617*g+1868*b+8192)>>14;
            int Cr=(((r-Y)*11682+8192)>>14)+128;
            int Cb=(((b-Y)*9241+8192)>>14)+128;
            dst[x]=Vec3b(saturate_cast<uchar>(Y),saturate_cast<uchar>(Cr),saturate_cast<uchar>(Cb));
        }
    }
    return out;
}

long long difference(const Mat& a, const Mat& b) {
    if(a.size()!=b.size() || a.type()!=b.type()) return -1;
    long long total=0;
    const int channels=a.channels();
    for(int y=0;y<a.rows;++y) {
        const uchar* p=a.ptr<uchar>(y);
        const uchar* q=b.ptr<uchar>(y);
        for(int i=0;i<a.cols*channels;++i) total+=abs((int)p[i]-(int)q[i]);
    }
    return total;
}

int main(int argc,char** argv) {
    const string file=argc>1 ? argv[1] : "/content/cow.jpg";
    Mat img=imread(file,IMREAD_COLOR);
    if(img.empty()) {cerr<<"Goruntu bulunamadi: "<<file<<'\n';return 1;}
    struct Test {const char* name; Conversion fn; int opencvCode;};
    Test tests[]={
        {"BGR -> RGB",bgrToRgb,COLOR_BGR2RGB},
        {"BGR -> GRAY",bgrToGray,COLOR_BGR2GRAY},
        {"BGR -> HSV",bgrToHsv,COLOR_BGR2HSV},
        {"BGR -> YCrCb",bgrToYCrCb,COLOR_BGR2YCrCb}
    };
    for(const Test& t:tests) {
        Conversion converter=t.fn; // Fonksiyon pointeri ile cagirma
        Mat manual=converter(img), reference;
        cvtColor(img,reference,t.opencvCode);
        cout<<t.name<<" | Toplam mutlak hata: "<<difference(manual,reference)<<'\n';
    }
}
