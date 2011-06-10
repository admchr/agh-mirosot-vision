#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>

#include <iostream>

using namespace std;
using namespace cv;

bool is_in_patch(int r, int g, int b){
    return r>150;
}

// SLOOOOOOOOOOOOOW & BUGGY
struct Deming {
    vector<Point> v;
    void add(Point p){
        v.push_back(p);
    }
    Point getMean(){
        int n = v.size();
        long long int x=0, y=0;
        for (int i=0;i<n;i++){
            x+=v[i].x;
            y+=v[i].y;
        }
        x/=n;
        y/=n;
        return Point(x, y);
    }
    
    double get(){
        Point mean = getMean();
        int n = v.size();
        double vx=0, vy=0, cov=0;
        for (int i=0;i<n;i++){
            Point p = v[i];
            vx+=(p.x-mean.x)*(p.x-mean.x);
            vy+=(p.y-mean.y)*(p.y-mean.y);
            cov+=(p.x-mean.x)*(p.y-mean.y);
        }
        vx/=n-1;
        vy/=n-1;
        cov/=n-1;
        
        double beta1 = (vy - vx + sqrt((vy-vx)*(vy-vx)+4*cov*cov))/(2*cov);
        return beta1;
    }
};


int absi(int i){
    if (i<0) return -i;
    return i;
}
int absz(int i, int j){ return absi(i-j);}

int main(int argc, char**argv){
    cv::Mat img = cv::imread(std::string(argv[1]));
    cv::Mat_<Vec3b> img_out = (Mat_<Vec3b>)img.clone();
    std::vector<cv::Mat> planes;
    split(img, planes);
    cv::Point start(33, 33);
    std::vector<cv::Point> v;
    v.push_back(start);
    
    Deming m;
    while (!v.empty()) {
        cv::Point p = v.back();
        uchar& b = planes[0].at<uchar>(p.y, p.x);
        uchar& g = planes[1].at<uchar>(p.y, p.x);
        uchar& r = planes[2].at<uchar>(p.y, p.x);
        v.pop_back();
        if (r==255 && g ==0 && b==0)
            continue;
        m.add(p);
        
        //std::cout<<p.x<<" "<<p.y<<":"<<b+0<<" "<<g+0<<" "<<r+0<<"|"<<std::endl;
        if (
            is_in_patch(r, g, b) && 
            p.x> 0 && p.y > 0 && p.x < planes[0].cols-1 && p.y < planes[0].rows-1
        ) {
            b=0;
            g=0;
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
    
    {
        double angle = m.get();
        Point c = m.getMean();
        
        for (int i=0;i<20 && abs(i/angle) < 20;i++){
            Point p(c.x+i, c.y-i/angle);
            planes[0].at<uchar>(p.y, p.x) = 0;
            planes[1].at<uchar>(p.y, p.x) = 255;
            planes[2].at<uchar>(p.y, p.x) = 0;
        }
    }
    
    cv::merge(planes, img);
    
    cv::imwrite(std::string("tri-out.png"), img);
    
}
