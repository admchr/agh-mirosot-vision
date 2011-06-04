#include <opencv/cv.h>

#include <opencv/highgui.h>
#include <string>

int main(){
    cv::Mat img = cv::imread(std::string("img.png"));
    cv::imshow("image with grain", img);
    cv::waitKey();
}
