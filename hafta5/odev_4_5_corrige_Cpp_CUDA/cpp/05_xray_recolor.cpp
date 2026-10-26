#include <opencv2/opencv.hpp>
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

vector<fs::path> imageFiles(const fs::path& folder) {
    vector<fs::path> files;
    for(const auto& entry:fs::directory_iterator(folder)) {
        if(!entry.is_regular_file()) continue;
        string ext=entry.path().extension().string();
        transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return (char)tolower(c);});
        if(ext==".png"||ext==".jpg"||ext==".jpeg"||ext==".bmp"||ext==".tif"||ext==".tiff")
            files.push_back(entry.path());
    }
    sort(files.begin(),files.end());
    return files;
}

// MAE: kanallar arasindaki mutlak farklarin ortalamasi
// Bu deger renk farkini gosterir; tespit basarisini olcmez.
double maeBgr(const Mat& a,const Mat& b) {
    Mat diff; absdiff(a,b,diff);
    Scalar m=mean(diff);
    return (m[0]+m[1]+m[2])/3.0;
}
int main(int argc,char** argv) {
    fs::path input=argc>1?argv[1]:"images";
    fs::path output=argc>2?argv[2]:"outputs/cpu";
    if(!fs::is_directory(input)) {cerr<<"Klasor bulunamadi: "<<input<<'\n'; return 1;}
    auto files=imageFiles(input);
    if(files.size()!=5) {cerr<<"Tam 5 goruntu gereklidir; bulunan: "<<files.size()<<'\n'; return 1;}
    fs::create_directories(output);
    ofstream report(output/"results.csv");
    report<<"image,width,height,original_vs_viridis_mae\n";
    for(const auto& p:files) {
        Mat original=imread(p.string(),IMREAD_COLOR);
        if(original.empty()) {cerr<<"Okunamadi: "<<p<<'\n'; return 1;}
        Mat gray,colorized;
        cvtColor(original,gray,COLOR_BGR2GRAY);
        applyColorMap(gray,colorized,COLORMAP_VIRIDIS);
        string name=p.stem().string();
        imwrite((output/(name+"_original.png")).string(),original);
        imwrite((output/(name+"_gray.png")).string(),gray);
        imwrite((output/(name+"_viridis.png")).string(),colorized);
        double err=maeBgr(original,colorized);
        report<<name<<","<<original.cols<<","<<original.rows<<","<<fixed<<setprecision(6)<<err<<'\n';
        cout<<name<<" | original vs VIRIDIS MAE: "<<fixed<<setprecision(6)<<err<<'\n';
    }
    cout<<"5 goruntu tamamlandi. Sonuclar: "<<output<<'\n';
}
