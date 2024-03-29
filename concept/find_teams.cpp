#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>


#include <iostream>

#include "tools.h"

using namespace std;
using namespace cv;

cv::Mat_<Vec3b> img;

const int min_pixels = 200;

const int max_x = 1000;
const int max_y = 1000;
bool visited[max_x][max_y];


bool is_black(Vec3b c){
    return c[2]<120;
}
bool is_lil_blue(Vec3b c){
    return !is_black(c) && c[0]>90 && c[0]<110 && c[1]*c[2]>128*128;
}
bool is_blue(Vec3b c){
    if (!is_lil_blue(c)) return false;
    return c[0]>90 && c[0]<110 && c[1]*c[2]>128*128;
}
bool is_lil_yellow(Vec3b c){
    return !is_black(c) && c[0]>15 && c[0]<40;// && c[1]*c[2]>128*128/2;
}
bool is_yellow(Vec3b c){
    if (!is_lil_yellow(c)) return false;
    return c[0]>25 && c[0]<35;
}

bool is_orange(Vec3b c){
    return c[0]>5 && c[0]<15 && c[1]*c[2]>128*128;
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

void draw_line(Mat_<Vec3b>& im, Vec3b color, Point where, double regression){
    
    
    for (double i=0;true;i+=0.01){
        Point p(where.x+i, where.y+i*regression);
        Point d=where-p;
        if (d.x*d.x+d.y*d.y > 1600)
            return;
        if (p.x<0 || p.y<0 || p.x>=im.cols || p.y>=im.rows)
            return;
        im(p) = color;
    }

}

int absi(int i){
    if (i<0) return -i;
    return i;
}
int absz(int i, int j){ return absi(i-j);}



Deming getRobot(Point p0, bool yellow, int& pixels) {
    int i=0;
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
    pixels=i;
    return m;
}

int main(int argc, char**argv){
    if (argc<6){
        cout<<"usage: in_file out_file white_x white_y white_radius"<<endl;
        return 1;
    }
    cv::Mat_<Vec3b> img0 = cv::imread(std::string(argv[1]));
    Point white_point = Point(atoi(argv[3]), atoi(argv[4]));
    white_correct(img0, white_point, atoi(argv[5]));
    
    img = (Mat_<Vec3b>)img0.clone();
    cv::Mat_<Vec3b> img_out = (Mat_<Vec3b>)img0.clone();
    std::vector<cv::Point> yellow, blue, orange;
    
    
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
            if (is_orange(px)){
                orange.push_back(Point(x, y));
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
            int pixels;
            Deming m = getRobot(v[i], yell, pixels);
            if (pixels < min_pixels) continue;
            double b = m.get();
            Point c = m.getMean();
            draw_line(img_out, Vec3b(0, 0, 255), c, b);
        }
    }
    
    
    for (int y=0;y<img.rows;y++) {
        for (int x=0;x<img.cols;x++) {
            Vec3b& c = img_out(y, x);
            if (!visited[x][y])
                continue;
            if (c==Vec3b(0, 0, 255))
                continue;
            
            c = Vec3b(0, 0, 0);
        }
        
    }
    
    for (int i=0;i<yellow.size();i++){
        img_out(yellow[i])=Vec3b(0, 255, 255);
    }
    for (int i=0;i<blue.size();i++){
        img_out(blue[i])=Vec3b(255, 0, 0);
    }

    for (int i=0;i<orange.size();i++){
        img_out(orange[i])=Vec3b(255, 255, 255);
    }

    img_out(white_point)=Vec3b(0, 0, 255);
    
    cv::imwrite(std::string(argv[2]), img_out);
    
}
