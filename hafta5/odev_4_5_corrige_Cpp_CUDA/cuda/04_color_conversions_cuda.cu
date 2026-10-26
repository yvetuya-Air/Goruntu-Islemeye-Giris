#include <opencv2/opencv.hpp>
#include <cuda_runtime.h>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <vector>
using namespace std;
using namespace cv;

#define CUDA_CHECK(call) do {cudaError_t e=(call); if(e!=cudaSuccess){\
    cerr<<"CUDA hatasi: "<<cudaGetErrorString(e)<<" (satir "<<__LINE__<<")\n"; return 1;}} while(0)

// type: 0 RGB, 1 Gray, 2 HSV, 3 YCrCb
__global__ void convertKernel(const unsigned char* input, unsigned char* output,
                              int pixels, int type, const int* sdiv, const int* hdiv) {
    int i=blockIdx.x*blockDim.x+threadIdx.x;
    if(i>=pixels) return;
    int b=input[3*i],g=input[3*i+1],r=input[3*i+2];
    if(type==0) {
        output[3*i]=(unsigned char)r;
        output[3*i+1]=(unsigned char)g;
        output[3*i+2]=(unsigned char)b;
    } else if(type==1) {
        output[i]=(unsigned char)((3735*b+19235*g+9798*r+16384)>>15);
    } else if(type==2) {
        int v=max(r,max(g,b)), mn=min(r,min(g,b)), d=v-mn;
        const int shift=12, scale=1<<shift;
        int s=(d*sdiv[v]+(scale>>1))>>shift, h=0;
        if(d) {
            if(v==r) h=g-b;
            else if(v==g) h=b-r+2*d;
            else h=r-g+4*d;
            h=(h*hdiv[d]+(scale>>1))>>shift;
            if(h<0) h+=180;
        }
        output[3*i]=(unsigned char)h;
        output[3*i+1]=(unsigned char)s;
        output[3*i+2]=(unsigned char)v;
    } else {
        int Y=(4899*r+9617*g+1868*b+8192)>>14;
        int Cr=(((r-Y)*11682+8192)>>14)+128;
        int Cb=(((b-Y)*9241+8192)>>14)+128;
        Y=max(0,min(255,Y));Cr=max(0,min(255,Cr));Cb=max(0,min(255,Cb));
        output[3*i]=(unsigned char)Y;
        output[3*i+1]=(unsigned char)Cr;
        output[3*i+2]=(unsigned char)Cb;
    }
}

long long difference(const Mat& a,const Mat& b) {
    long long total=0;
    for(int y=0;y<a.rows;++y){
        const uchar* p=a.ptr<uchar>(y),*q=b.ptr<uchar>(y);
        for(int j=0;j<a.cols*a.channels();++j) total+=abs((int)p[j]-(int)q[j]);
    }
    return total;
}

int main(int argc,char** argv) {
    string filename=argc>1?argv[1]:"/content/cow.jpg";
    Mat input=imread(filename,IMREAD_COLOR);
    if(input.empty()){cerr<<"Goruntu bulunamadi: "<<filename<<'\n';return 1;}
    if(!input.isContinuous())input=input.clone();
    const int pixels=input.rows*input.cols;
    unsigned char *d_input=nullptr,*d_output=nullptr;
    int *d_sdiv=nullptr,*d_hdiv=nullptr;
    CUDA_CHECK(cudaMalloc((void**)&d_input,pixels*3));
    CUDA_CHECK(cudaMalloc((void**)&d_output,pixels*3));
    CUDA_CHECK(cudaMemcpy(d_input,input.data,pixels*3,cudaMemcpyHostToDevice));
    const int shift=12,scale=1<<shift;
    int sdiv[256]={0},hdiv[256]={0};
    for(int i=1;i<256;++i){
        sdiv[i]=cvRound((double)(255*scale)/i);
        hdiv[i]=cvRound((double)(180*scale)/(6*i));
    }
    CUDA_CHECK(cudaMalloc((void**)&d_sdiv,256*sizeof(int)));
    CUDA_CHECK(cudaMalloc((void**)&d_hdiv,256*sizeof(int)));
    CUDA_CHECK(cudaMemcpy(d_sdiv,sdiv,256*sizeof(int),cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_hdiv,hdiv,256*sizeof(int),cudaMemcpyHostToDevice));
    struct Mode {const char* name;int code;int channels;};
    Mode modes[]={
        {"BGR -> RGB",COLOR_BGR2RGB,3},
        {"BGR -> GRAY",COLOR_BGR2GRAY,1},
        {"BGR -> HSV",COLOR_BGR2HSV,3},
        {"BGR -> YCrCb",COLOR_BGR2YCrCb,3}
    };
    const int threads=256,blocks=(pixels+threads-1)/threads;
    for(int type=0;type<4;++type){
        convertKernel<<<blocks,threads>>>(d_input,d_output,pixels,type,d_sdiv,d_hdiv);
        CUDA_CHECK(cudaGetLastError());
        CUDA_CHECK(cudaDeviceSynchronize());
        Mat result(input.rows,input.cols,CV_MAKETYPE(CV_8U,modes[type].channels));
        CUDA_CHECK(cudaMemcpy(result.data,d_output,pixels*modes[type].channels,cudaMemcpyDeviceToHost));
        Mat reference;
        cvtColor(input,reference,modes[type].code);
        cout<<modes[type].name<<" | Toplam mutlak hata: "<<difference(result,reference)<<'\n';
    }
    cudaFree(d_input);cudaFree(d_output);cudaFree(d_sdiv);cudaFree(d_hdiv);
    return 0;
}
