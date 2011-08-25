
#include "balance.h"

using namespace cv;

cv::Vec3b median(const Image& img, image_pos pos, int radius) {
    std::vector<int> channel[3];
    for (int dx = -radius; dx<=radius; dx++)
        for (int dy = -radius; dy<=radius; dy++) {
            int x = pos.x+dx, y = pos.y+dy;
            if (x<0 || y<0 || x>=img.size().width || y>=img.size().height)
                continue;
            cv::Vec3b color = img(pos.y+dy, pos.x+dx);
            
            for (int i=0;i<3;i++)
                channel[i].push_back(color[i]);
        }
    
    for (int i=0;i<3;i++)
        std::sort(channel[i].begin(), channel[i].end());
    
    int median = channel[0].size()/2;
    
    return cv::Vec3b(channel[0][median], channel[1][median], channel[2][median]);
}

Vec3b get_average(image_pos* white_points, int white_points_len, image_pos pos) {
    // TODO
}

void white_balance(Image* img, mirosot_vision_config* config) {
    const int TILE=16;
    for (int i=0;i<img->size().width-TILE; i+=TILE)
        for (int j=0;j < img->size().height-TILE; j+=TILE) {
            Image buf=(*img)(cv::Rect(i,j,TILE,TILE));
            buf*=0.5;
        }
} 
