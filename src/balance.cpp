
#include "balance.hpp"

using namespace cv;
using namespace std;


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

Vec3b get_white(vector<pair<image_pos, Vec3b> > white_points, image_pos pos) {
    double weight_sum = 0;
    Vec3d res;
    for (int i=0;i<white_points.size();i++) {
        image_pos white_pos = white_points[i].first;
        Vec3b white_color = white_points[i].second;
        
        image_pos diff;
        diff.x = white_pos.x - pos.x;
        diff.y = white_pos.y - pos.y;
        
        int dist_sqr = diff.x*diff.x + diff.y*diff.y;
        // dist_sqr can be 0
        double weight = 1.0/(dist_sqr + 1);
        
        for (int j=0;j<3;j++)
            res[j]+=weight*white_color[j];
        weight_sum+=weight;
    }
    res=res*(1.0/weight_sum);
    return res;
}

void white_balance(Image* img, mirosot_vision_config* config) {
    const int TILE=16;
    for (int i=0;i<img->size().width-TILE; i+=TILE)
        for (int j=0;j < img->size().height-TILE; j+=TILE) {
            Image buf=(*img)(cv::Rect(i,j,TILE,TILE));
            buf*=0.5;
        }
} 
