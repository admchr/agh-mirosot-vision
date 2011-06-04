#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>

#include <iostream>

int absi(int i){
    if (i<0) return -i;
    return i;
}
int absz(int i, int j){ return absi(i-j);}

int main(){
    cv::Mat img = cv::imread(std::string("img.png"));
    std::vector<cv::Mat> planes;
    split(img, planes);
    cv::Point start(327, 116);
    std::vector<cv::Point> v;
    v.push_back(start);
    
    uchar stb = planes[0].at<uchar>(start.y, start.x);
    uchar stg = planes[1].at<uchar>(start.y, start.x);
    uchar str = planes[2].at<uchar>(start.y, start.x);
    
    std::cout<<stb+0<<" "<<stg+0<<" "<<str+0<<"*";
    int diff = 150;
    std::cout<<"a"<<std::endl;
    while (!v.empty()) {
        cv::Point p = v.back();
        v.pop_back();
        uchar& b = planes[0].at<uchar>(p.y, p.x);
        uchar& g = planes[1].at<uchar>(p.y, p.x);
        uchar& r = planes[2].at<uchar>(p.y, p.x);
        //std::cout<<p.x<<" "<<p.y<<":"<<b+0<<" "<<g+0<<" "<<r+0<<"|"<<std::endl;
        if (
            (absi(b-(int)stb)+
            absi(g-(int)stg)+
            absi(r-(int)str))<diff &&
            r!=255
            && p.x> 0 && p.y > 0 && p.x < planes[0].cols-1 && p.y < planes[0].rows-1
        ) {
            b=255;
            g=255;
            r=255;
            v.push_back(cv::Point(p.x-1, p.y-1));
            v.push_back(cv::Point(p.x-1, p.y));
            v.push_back(cv::Point(p.x-1, p.y+1));
            v.push_back(cv::Point(p.x, p.y-1));
            v.push_back(cv::Point(p.x, p.y));
            v.push_back(cv::Point(p.x, p.y+1));
            v.push_back(cv::Point(p.x+1, p.y-1));
            v.push_back(cv::Point(p.x+1, p.y));
            v.push_back(cv::Point(p.x+1, p.y+1));
        }
    }
    
    std::cout<<"a"<<std::endl;
    cv::merge(planes, img);
    
    cv::imwrite(std::string("img2.png"), img);
    //cv::imshow("image with grain", img);
    //cv::waitKey();
}
