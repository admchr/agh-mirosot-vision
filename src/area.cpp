
#include "area.hpp"
#include "visionstate.hpp"
#include "mshift.hpp"
#include "util.hpp"
#include "debug.hpp"

#include <opencv/cv.h>
#include <queue>
#include <utility>
#include <set>

using namespace cv;

bool comparePoint(Point p, Point q) {
    if (p.x != q.x)
        return p.x < q.x;
    return p.y < q.y;
}

void Patch::getSecondaryPatches(int* out, Image* debug) {
    std::set<Point, bool(*)(Point, Point)> visited(comparePoint);
    Point p = getCenter();
    double angle = getAngle();

    double front_x = cos(angle);
    double front_y = sin(angle);
    angle-=M_PI/2;
    double side_x = cos(angle);
    double side_y = sin(angle);

    PatchMoments colors[3];
    for (double i=-8; i <= 8; i+=0.5)
        for (double j=5; j <= 8;  j+=0.5) {
            Point q(p.x + front_x*i + side_x*j, p.y + front_y*i + side_y*j);
            if (q.x<0 || q.y <0 || q.x >= type->map->img.cols || q.y >= type->map->img.rows)
                continue;

            if (visited.find(q) != visited.end())
                continue;
            visited.insert(q);

            int in_index = -1;
            Vec3b hsv = type->map->img_hsv(q);
            if (hsv[1] < type->config->minimum_saturation)
                continue;
            if (hsv[2] < type->config->black_cutoff)
                continue;

            int hue = hsv[0];
            for (int k=0; k<3; k++) {
                if (in_hue(type->config->secondary_patches+k, hue))
                    in_index = k;
            }
            if (in_index == -1)
                continue;

            colors[in_index].add(q);
            if (debug) {
                if (in_index==0)
                    paintPoint(*debug, q, Vec3b(255, 0, 255));
                if (in_index==1)
                    paintPoint(*debug, q, Vec3b(0, 255, 0));
                if (in_index==2)
                    paintPoint(*debug, q, Vec3b(0, 0, 255));
            }

        }
    int min_count = min(colors[0].getCount(), min(colors[1].getCount(), colors[2].getCount()));

    if (min_count == colors[0].getCount()) {
        out[0] = 1;
        out[1] = 2;
    }
    if (min_count == colors[1].getCount()) {
        out[0] = 0;
        out[1] = 2;
    }
    if (min_count == colors[2].getCount()) {
        out[0] = 0;
        out[1] = 1;
    }

    Point back_patch(colors[out[0]].getMean());
    Point front_patch(colors[out[1]].getMean());

    int secondary_x = front_patch.x - back_patch.x;
    int secondary_y = front_patch.y - back_patch.y;

    // inner product
    if (secondary_x*front_x + secondary_y*front_y < 0) {
        std::swap(out[0], out[1]);
    }
}

void fillTeam(vector<Patch*> team, amv_team_data* data) {
    data->team_len = 0;
    amv_robot_data* robot = data->team;
    for (unsigned int i = 0; i<team.size(); i++) {
        Patch* patch = team[i];
        patch->getSecondaryPatches(robot->color);
        cv::Point p = patch->getRobotCenter();
        robot->position.x = p.x;
        robot->position.y = p.y;
        robot->angle = patch->getRobotAngle();
        robot->certainty = patch->getRobotCertainty();
        robot++;
        data->team_len++;
    }
}

void fillBall(Patch* ball, amv_vision_data* data) {
    if (!ball) {
        data->ball_pos.x = data->ball_pos.y = -1;
        return;
    }
    data->ball_pos.x = ball->getCenter().x;
    data->ball_pos.y = ball->getCenter().y;
}

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
    meanShiftPointProportional(
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

vector<Patch*> PatchType::getTeam(int size) {
    vector<Patch*> out;
    std::priority_queue<std::pair<double, Patch*> > team;

    for (unsigned int i=0; i<patches.size(); i++) {
        Patch* patch = patches[i];
        if (patch->getCount() <= 1)
            continue;
        team.push(std::make_pair(-patch->getRobotCertainty(), patch));

        if (team.size() > size)
            team.pop();
    }

    while (!team.empty()) {
        Patch* patch = team.top().second;
        team.pop();

        patch->isRobot = true;
        out.push_back(patch);
    }
    return out;
}

Patch* PatchType::getBall() {
    if (patches.empty()) {
        return 0;
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

    return maximal;
}

int PatchType::getPatchSize() {
    double min_size = map->state->config->px_per_cm * 7.5;
    int area = min_size*min_size * 0.5;

    return area;
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
	if (!is_patch(this->type->config, &this->type->team, hsv(p)))
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
    if (abs(getBoundingBox().height - getBoundingBox().width) > 20)
        return 0;
    double result = 1;
    result *= positive_point_certainty(type->getPatchSize(), moments.getCount());

    Vec3b hsv = hsvconverter.get(getMeanColor());

    double color_result = interval_certainty(type->team.hue_min, type->team.hue_max, hsv[0]);
    color_result *= hsv[1]/256.0;

    double white_result;
    if (type->config->white_cutoff)
        white_result = interval_certainty(type->config->white_cutoff, 2*256-type->config->white_cutoff, hsv[2]); // TODO: provisional
    else
        white_result = 0;

    result *= certainty_or(color_result, white_result);

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
    return p;
    double angle = getAngle() - M_PI*0.5;
    p.x+=cos(angle)*MEAN_OFFSET;
    p.y+=sin(angle)*MEAN_OFFSET;
    return p;
}

double Patch::getRobotAngle() {
    return getAngle()+M_PI*0.25;
}

double Patch::getAngle() {
    double angle = moments.getAngle();

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
