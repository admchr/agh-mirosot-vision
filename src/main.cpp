#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>

#include <iostream>

using namespace std;
using namespace cv;

cv::Mat_<Vec3b> img;

bool is_black(Vec3b c){
    return c[2]<100;
}
bool is_lil_blue(Vec3b c){
    return !is_black(c) && c[0]>75 && c[0]<105;
}
bool is_blue(Vec3b c){
    return c[0]>85 && c[0]<95;
}
bool is_lil_yellow(Vec3b c){
    return !is_black(c) && c[0]>15 && c[0]<40;
}
bool is_yellow(Vec3b c){
    return c[0]>25 && c[0]<35;
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

const int max_x = 1000;
const int max_y = 1000;

bool visited[max_x][max_y];

Deming getRobot(Point p0, bool yellow, bool &ok) {
    int i=0;
    ok=false;
    vector<Point> v;
    v.push_back(p0);
    Deming m;
    while (!v.empty()) {
        i++;
        cv::Point p = v.back();
        v.pop_back();
        if (visited[p.x][p.y])
            continue;
        
        visited[p.x][p.y]=true;
        m.add(p);
        
        //std::cout<<p.x<<" "<<p.y<<":"<<b+0<<" "<<g+0<<" "<<r+0<<"|"<<std::endl;
        if ( (
            (yellow && is_lil_yellow(img(p.y, p.x))) ||
            (!yellow && is_lil_blue(img(p))))&&
            p.x>0 && p.y > 0 && p.x < img.cols-1 && p.y < img.rows-1
        ) {
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
    if (i>100) ok=true;
    return m;
}

int main(int argc, char**argv){
    cv::Mat img0 = cv::imread(std::string(argv[1]));
    img = (Mat_<Vec3b>)img0.clone();
    cv::Mat_<Vec3b> img_out = (Mat_<Vec3b>)img0.clone();
    std::vector<cv::Point> yellow, blue;
    
    cvtColor(img, img, CV_BGR2HSV);
    
    for (int y=0;y<img.rows;y++) {
        for (int x=0;x<img.cols;x++) {
            Vec3b px = img.at<Vec3b>(y, x);
            if (is_black(px))
                continue;
            if (is_yellow(px)){
                yellow.push_back(Point(x, y));
            }
            if (is_blue(px)){
                blue.push_back(Point(x, y));
            }
        }
        
    }
    
    
    for (int yell=0; yell<2; yell++) {
        vector<Point> v;
        if (yell)
            v=yellow;
        else 
            v=blue;
        for (int i=0;i<v.size();i++){
            bool ok;
            Deming m = getRobot(v[i], yell, ok);
            if (!ok) continue;
            double angle = m.get();
            Point c = m.getMean();
            
            for (int i=0;i<20 && abs(i/angle) < 20;i++){
                Point p(c.x+i, c.y-i/angle);
                img_out(p) = Vec3b(0, 0, 255);
            }
        }
    }
    
    
    for (int y=0;y<img.rows;y++) {
        for (int x=0;x<img.cols;x++) {
            if (!visited[x][y])
                continue;
            if (img_out(y, x)==Vec3b(0, 0, 255))
                continue;
            img_out(y, x)=Vec3b(0, 0, 0);
        }
        
    }
    cv::imwrite(std::string("tri-out.png"), img_out);
    
}
