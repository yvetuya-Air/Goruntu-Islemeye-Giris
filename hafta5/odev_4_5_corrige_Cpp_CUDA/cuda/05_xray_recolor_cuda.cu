#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iomanip>
namespace fs=std::filesystem;
using namespace cv;
using namespace std;
#define CHECK(call) do {cudaError_t e=(call); if(e!=cudaSuccess) {cerr<<"CUDA: "<<cudaGetErrorString(e)<<" line "<<__LINE__<<'\n';return 1;}}while(0)
__global__ void grayKernel(const uchar* bgr,uchar* gray,int n) {
    int i=blockIdx.x*blockDim.x+threadIdx.x;
    if(i<n) {int b=bgr[3*i],g=bgr[3*i+1],r=bgr[3*i+2];
        gray[i]=(uchar)((3735*b+19235*g+9798*r+16384)>>15);}
}
__global__ void viridisKernel(const uchar* gray,uchar* bgr,const uchar* lut,int n) {
    int i=blockIdx.x*blockDim.x+threadIdx.x;
    if(i<n) {int v=gray[i];for(int c=0;c<3;c++)bgr[3*i+c]=lut[3*v+c];}
}
vector<fs::path> imageFiles(const fs::path& folder) {
    vector<fs::path> files;
    for(const auto& e:fs::directory_iterator(folder)) {
        if(!e.is_regular_file())continue;
        string s=e.path().extension().string();
        transform(s.begin(),s.end(),s.begin(),[](unsigned char c){return (char)tolower(c);});
        if(s==".png"||s==".jpg"||s==".jpeg"||s==".bmp"||s==".tif"||s==".tiff")files.push_back(e.path());
    }
    sort(files.begin(),files.end());return files;
}
long long absError(const Mat& a,const Mat& b) {
    if(a.size()!=b.size()||a.type()!=b.type())return -1;
    Mat diff;absdiff(a,b,diff);Scalar s=sum(diff);return (long long)(s[0]+s[1]+s[2]);
}
double maeBgr(const Mat& a,const Mat& b) {
    Mat diff;absdiff(a,b,diff);Scalar s=mean(diff);return (s[0]+s[1]+s[2])/3.0;
}
int main(int argc,char** argv) {
    fs::path input=argc>1?argv[1]:"images";
    fs::path output=argc>2?argv[2]:"outputs/cuda";
    if(!fs::is_directory(input)){cerr<<"Klasor bulunamadi: "<<input<<'\n';return 1;}
    auto files=imageFiles(input);
    if(files.size()!=5){cerr<<"Tam 5 goruntu gereklidir; bulunan: "<<files.size()<<'\n';return 1;}
    fs::create_directories(output);
    ofstream report(output/"results.csv");
    report<<"image,width,height,gray_cuda_vs_opencv_error,viridis_cuda_vs_opencv_error,original_vs_viridis_mae\n";
    // OpenCV VIRIDIS paletinin 256 BGR girisini CPU'da bir kez olustur.
    // Piksel renklendirmesi LUT kullanilarak CUDA kernelinde yapilir.
    Mat ramp(1,256,CV_8UC1);
    for(int v=0;v<256;v++)ramp.at<uchar>(0,v)=(uchar)v;
    Mat lut;applyColorMap(ramp,lut,COLORMAP_VIRIDIS);
    uchar* d_lut=nullptr;
    CHECK(cudaMalloc((void**)&d_lut,256*3));
    CHECK(cudaMemcpy(d_lut,lut.data,256*3,cudaMemcpyHostToDevice));
    for(const auto& p:files) {
        Mat original=imread(p.string(),IMREAD_COLOR);
        if(original.empty()){cerr<<"Okunamadi: "<<p<<'\n';return 1;}
        if(!original.isContinuous())original=original.clone();
        int n=original.rows*original.cols;
        uchar *d_img=nullptr,*d_gray=nullptr,*d_color=nullptr;
        CHECK(cudaMalloc((void**)&d_img,3*n));CHECK(cudaMalloc((void**)&d_gray,n));
        CHECK(cudaMalloc((void**)&d_color,3*n));
        CHECK(cudaMemcpy(d_img,original.data,3*n,cudaMemcpyHostToDevice));
        int threads=256,blocks=(n+threads-1)/threads;
        grayKernel<<<blocks,threads>>>(d_img,d_gray,n);
        CHECK(cudaGetLastError());
        viridisKernel<<<blocks,threads>>>(d_gray,d_color,d_lut,n);
        CHECK(cudaGetLastError());CHECK(cudaDeviceSynchronize());
        Mat gray(original.size(),CV_8UC1),color(original.size(),CV_8UC3);
        CHECK(cudaMemcpy(gray.data,d_gray,n,cudaMemcpyDeviceToHost));
        CHECK(cudaMemcpy(color.data,d_color,3*n,cudaMemcpyDeviceToHost));
        Mat refGray,refColor;
        cvtColor(original,refGray,COLOR_BGR2GRAY);
        applyColorMap(refGray,refColor,COLORMAP_VIRIDIS);
        long long ge=absError(gray,refGray),ce=absError(color,refColor);
        double mae=maeBgr(original,color);
        string name=p.stem().string();
        imwrite((output/(name+"_original.png")).string(),original);
        imwrite((output/(name+"_gray.png")).string(),gray);
        imwrite((output/(name+"_viridis.png")).string(),color);
        cout<<name<<" | Gray error="<<ge<<" | VIRIDIS error="<<ce<<" | Original MAE="<<mae<<'\n';
        report<<name<<","<<original.cols<<","<<original.rows<<","<<ge<<","<<ce<<","<<fixed<<setprecision(6)<<mae<<'\n';
        cudaFree(d_img);cudaFree(d_gray);cudaFree(d_color);
    }
    cudaFree(d_lut);return 0;
}
