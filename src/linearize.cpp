#include <opencv/cv.h>
#include <vector>
#include <opencv/highgui.h>
#include <string>
#include <cmath>

#include <iostream>

#include "tools.h"

using namespace std;
using namespace cv;

int main(int argc, char**argv){
    string type(argv[3]);
    bool lin = type == "lin";
    cv::Mat_<Vec3b> img = cv::imread(std::string(argv[1]));
    
    for (int y=0;y<img.rows;y++) {
        for (int x=0;x<img.cols;x++) {
            if (lin)
                img(y, x) = srgb_to_rgb(img(y, x));
            else
                img(y, x) = rgb_to_srgb(img(y, x));
        }
    }
    
    cv::imwrite(std::string(argv[2]), img);
}
