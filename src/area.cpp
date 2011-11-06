
#include "area.hpp"
#include "visionstate.hpp"
#include "mshift.hpp"
#include "util.hpp"

#include <opencv/cv.h>
#include <queue>
#include <utility>

using namespace cv;

void PatchFinder::setImages(Image img, Image img_hsv) {
	this->img = img;
	this->img_hsv = img_hsv;
	cv::Size size = img.size();
    precompute_map.resize(size);
    meanshifted.resize(size);
    area_map.resize(size);
    
}

void PatchFinder::preparePixel(cv::Point p) {
    if (meanshifted.get(p.x, p.y))
        return;
    meanShiftPoint(
                img,
                p.x,
                p.y,
                state->config->meanshift_radius, // position radius
                state->config->meanshift_threshold // value radius
            );
    img_hsv(p) = hsvconverter.get(img(p));
    meanshifted.set(p.x, p.y, true);
}

void PatchFinder::getSets() {
    std::vector<Point> Q;

    // seemed to increase performance for 800x600 images
    // reserving 800x600 slots was not as effective
    Q.reserve(1024*1024);

    for (int orig_y=0; orig_y<img.rows; orig_y++) {
        for (int orig_x=0; orig_x<img.cols; orig_x++) {
            PatchType* b = precompute_map.get(orig_x, orig_y);
            if (!b) continue;
            if (area_map.get(orig_x, orig_y)) continue;
            Patch* pt = b->newPatch();

            Q.push_back(Point(orig_x, orig_y));
            // this point will be added to the patch later

            while (!Q.empty()) {
                Point p = Q.back();
                Q.pop_back();
                const int minx = max(p.x - 1, 0);
                const int miny = max(p.y - 1, 0);
                const int maxx = min(p.x + 2, img.cols);
                const int maxy = min(p.y + 2, img.rows);

                for (int nx = minx; nx < maxx; nx++)
                    for (int ny = miny; ny < maxy; ny++) {
                        Point np = Point(nx, ny);

                        if (area_map.get(nx, ny))
                            continue;
                        if (!pt->add(np, p)) {
                            preparePixel(np);
                            if (!pt->add(np, p))
                                continue;
                        }
                        area_map.set(nx, ny, pt);

                        Q.push_back(np);
                }
            }
        }
    }
}

int PatchFinder::colorDistance(Vec3b a, Vec3b b) {
    int res=0;
    for (int i=0;i<3;i++) {
        int d = a[i] - b[i];
        res+=d*d;
    }
    return res;
}

PatchType::~PatchType() {
	for (unsigned int i=0; i<patches.size(); i++) {
		delete patches[i];
	}
}

Patch* PatchType::newPatch()
{
	patches.push_back(new Patch(this));
	return patches.back();
}

void PatchType::fillTeam(amv_team_data* data) {
    data->team_len = 0;
    amv_robot_data* robot = data->team;

    std::priority_queue<std::pair<double, Patch*> > team;

    for (unsigned int i=0; i<patches.size(); i++) {
        Patch* patch = patches[i];
        if (patch->getCount() <= 1)
            continue;
        team.push(std::make_pair(-patch->getRobotCertainty(), patch));

        if (team.size() > config->team_size)
            team.pop();
    }

    while (!team.empty()) {
        Patch* patch = team.top().second;
        double certainty = -team.top().first;
        team.pop();

        patch->isRobot = true;

        cv::Point p = patch->getRobotCenter();
        robot->position.x = p.x;
        robot->position.y = p.y;
        robot->angle = patch->getAngle();
        robot->certainty = certainty;
        robot++;
        data->team_len++;
    }
}

void PatchType::fillBall(amv_vision_data* data) {
    if (patches.empty()) {
        data->ball_pos.x = data->ball_pos.y = -1;
        return;
    }


    Patch* maximal = patches[0];
    for (unsigned int i=0; i<patches.size(); i++) {
        Patch* patch = patches[i];
        if (patch->getCount() <= 1)
            continue;
        if (patch->getBallCertainty() > maximal->getBallCertainty()) {
            maximal = patch;
        }

    }
    maximal->isBall = true;

    data->ball_pos.x = maximal->getCenter().x;
    data->ball_pos.y = maximal->getCenter().y;
}

int PatchType::getMinPatchSize() {
    double min_size = map->state->config->px_per_cm * 3.5;
    int min_area = min_size*min_size * 0.75;

    return min_area;
}

int PatchType::getMaxPatchSize() {
    double max_size = map->state->config->px_per_cm * 8;
    int max_area = max_size*max_size;

    return max_area;
}

int PatchType::getMaxPatchWidth() {
    return map->state->config->px_per_cm * 8 * 2;
}

bool Patch::add(cv::Point p, cv::Point neighbour) {
	Image img = this->type->map->img;
	Image hsv = this->type->map->img_hsv;
    Vec3b color = img(p);
	if (moments.getCount() == 0) {
		origin = img(p);
		aabbox = Rect(p, p);
	}
	int dst = this->type->config->same_color_distance;
	if (PatchFinder::colorDistance(color, img(neighbour)) > dst*dst)
		return false;
	if (!is_robot(this->type->config, &this->type->team, hsv(p)))
		return false;
	moments.add(p);
	color_sum += color;
	aabbox = aabbox | Rect(p, p);
    return true;
}

int Patch::getCount() {
    return moments.getCount();
}
Rect Patch::getBoundingBox() {
    return aabbox;
}

Vec3b Patch::getMeanColor() {
    int count = moments.getCount();
    if (count==0)
        return Vec3b(0,0,0);
    return Vec3b(color_sum[0]/count, color_sum[1]/count, color_sum[2]/count);
}

double Patch::getRobotCertainty() {
    double result = 1;
    result *= positive_interval_certainty(type->getMinPatchSize(), type->getMaxPatchSize(), moments.getCount());

    Vec3b hsv = hsvconverter.get(getMeanColor());

    double color_result = interval_certainty(type->team.hue_min, type->team.hue_max, hsv[0]);
    color_result *= hsv[1]/256.0;

    double white_result;
    if (type->config->white_cutoff)
        white_result = interval_certainty(type->config->white_cutoff, 2*256+type->config->white_cutoff, hsv[2]); // TODO: provisional
    else
        white_result = 0;

    result *= certainty_or(color_result, white_result*0.5);

    return result;
}


double Patch::getBallCertainty() {
    double result = 1;
    // TODO: provisional
    double ball_radius = 4.27/2*type->config->px_per_cm;
    double ball_area = ball_radius*ball_radius*M_PI;
    result *= positive_interval_certainty(ball_area*3/4, ball_area*5/4, moments.getCount());

    Vec3b hsv = hsvconverter.get(getMeanColor());

    double color_result = interval_certainty(type->team.hue_min, type->team.hue_max, hsv[0]);
    color_result *= hsv[1]/256.0;

    double white_result;
    if (type->config->white_cutoff)
        white_result = interval_certainty(type->config->white_cutoff, 2*256+type->config->white_cutoff, hsv[2]); // TODO: provisional
    else
        white_result = 0;

    result *= certainty_or(color_result, white_result*0.5);

    return result;
}

Point Patch::getCenter() {
    return moments.getMean();
}
Point Patch::getRobotCenter() {
	const int MEAN_OFFSET = 3;
    Point p = moments.getMean();
    double angle = getAngle() - M_PI*0.5;
    p.x+=cos(angle)*MEAN_OFFSET;
    p.y+=sin(angle)*MEAN_OFFSET;
    return p;
}

double Patch::getAngle() {
    double angle = moments.getAngle()+M_PI*0.25;

	double a = moments.getRegressionSlope();
	double b = moments.getRegressionPosition();
    Rect bbox = getBoundingBox();
    int up = 0;
    int down = 0;
    for (int y=bbox.y; y<=bbox.y+bbox.height; y++)
    	for (int x=bbox.x; x<=bbox.x+bbox.width; x++) {
    		if (type->map->area_map.get(x, y) == this) {
    			if (a*x + b > y)
    				down++;
    			else
    				up++;
    		}
    	}

    if (down<up)
    	angle+=M_PI;

    return angle;
}
