// wycina obszar z obrazka
// usage: ./crop file_in file_out x y width height

#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>

#include <iostream>

using namespace std;
using namespace cv;

int main(int argc,char**argv){
    
    cv::Mat img0 = cv::imread(std::string(argv[1]));
    Mat_<Vec3b> img_out(img0, Rect(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6])));
    cv::imwrite(std::string(argv[2]), img_out);
}
