
#include "patch.hpp"
#include "visionstate.hpp"
#include "mshift.hpp"
#include "util.hpp"
#include "debug.hpp"

#include <opencv/cv.h>
#include <queue>
#include <utility>
#include <set>
#include <complex>

using namespace cv;
using namespace std;


void PatchFinder::setImages(amv_state* state, Image img, Image img_hsv) {
    this->state = state;
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
    img_hsv(p) = hsvconverter.fromBGRToHSL(img(p));
    meanshifted.set(p.x, p.y, true);
}

void PatchFinder::extendPatch(Point np, Point p, Patch* pt, vector<Point>& Q) {
    if (area_map.get(np.x, np.y))
        return;
    if (!pt->add(np, p)) {
        if (this->state->config->meanshift_radius == 0) {
            //this avoids having to redo much of the work

            return;
        }
        preparePixel(np);
        if (!pt->add(np, p))
            return;
    }
    area_map.set(np.x, np.y, pt);

    Q.push_back(np);

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
                if (p.x > 0)
                        extendPatch(Point(p.x - 1, p.y), p, pt, Q);
                if (p.y > 0)
                        extendPatch(Point(p.x, p.y + 1), p, pt, Q);
                if (p.x + 1 < img.cols)
                        extendPatch(Point(p.x + 1, p.y), p, pt, Q);
                if (p.y + 1 > img.rows)
                        extendPatch(Point(p.x, p.y + 1), p, pt, Q);
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
	if (!is_patch(this->type->config, &this->type->team, hsv(p), type->is_yellow))
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
    if (abs(getBoundingBox().height - getBoundingBox().width) > 9.3*type->config->px_per_cm)
        return 0;
    double result = 1;
    result *= positive_point_certainty(type->getPatchSize(), moments.getCount());

    Vec3b hsv = hsvconverter.fromBGRToHSL(getMeanColor());

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
    result *= positive_point_certainty(ball_area, moments.getCount());

    Vec3b hsv = hsvconverter.fromBGRToHSL(getMeanColor());

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
	int mean_offset = 7.5/3/sqrt(2)*type->config->px_per_cm;
    Point p = moments.getMean();
    double angle = getAngle() - M_PI*0.5;
    p.x+=cos(angle)*mean_offset;
    p.y+=sin(angle)*mean_offset;
    return p;
}

double Patch::getRobotAngle() {
    return getAngle()+M_PI*0.25;
}

double Patch::getAngle() {
    double angle = moments.getAngle();

    if (getAngleFitness(angle, type->config->angle_method) < getAngleFitness(angle + M_PI, type->config->angle_method))
        angle+=M_PI;

    return angle;
}

/* Tests whether the given angle represents the triangle shape well:
 *       A
 *     |\| <- angle
 *     | |
 *     | |\
 *     | o |
 *     |  / <- triangle
 *     | /
 *     |/
 */
double Patch::getAngleFitness(double angle, int DEBUG_ANGLE_METHOD, Image* debug) {
    /** WARNING: the following code may cause eye irritation */
    Point2d u0(cos(angle), sin(angle));
    if (DEBUG_ANGLE_METHOD == 0) {
        // pixel count
        Point2d p = moments.getMean();
        Rect bbox = getBoundingBox();
        int right_of = 0;
        int left_of = 0;
        for (int y=bbox.y; y<=bbox.y+bbox.height; y++)
            for (int x=bbox.x; x<=bbox.x+bbox.width; x++) {
                if (type->map->area_map.get(x, y) == this) {
                    Point2d r(x - p.x, y - p.y);
                    if (r.x*u0.y > r.y*u0.x) {
                        left_of++;

                        if (debug) drawPoint(*debug, Point(x, y), Vec3b(0, 0, 255));
                    } else
                        right_of++;
                }
            }

        if (debug) drawPoint(*debug, getCenter(), Vec3b(0, 0, 0));
        return left_of*1.0/(right_of + left_of);
    } else if (DEBUG_ANGLE_METHOD == 1 || DEBUG_ANGLE_METHOD == 2) {
        // triangle fit
        double r = 6.5*type->config->px_per_cm*sqrt(2)/2 - 1;
        Point2d v0(cos(angle + M_PI/2), sin(angle + M_PI/2));

        Point p = getCenter();

        int good = 0, all = 0;
        std::set<Point, bool(*)(Point, Point)> visited(comparePoint);
        for (double u = -r; u < r; u += 0.5)
            for (double v = -r; v < r; v += 0.5) {
                if (u + v > r*2/3 || v - u > r*2/3 || v < -r/3)
                    continue;

                Point q(p.x + u*u0.x + v*v0.x, p.y + u*u0.y + v*v0.y);
                if (visited.find(q) != visited.end())
                    continue;
                visited.insert(q);

                if (type->map->area_map.get(q) == this &&
                        type->map->area_map.get(q.x+1, q.y) == this &&
                        type->map->area_map.get(q.x-1, q.y) == this &&
                        type->map->area_map.get(q.x, q.y+1) == this &&
                        type->map->area_map.get(q.x, q.y-1) == this//*/
                ) {
                    good++;
                    if (debug) drawPoint(*debug, q, Vec3b(0, 0, 255));
                }
                all++;
            }
        if (debug) drawPoint(*debug, getCenter(), Vec3b(0, 0, 0));
        if (DEBUG_ANGLE_METHOD == 2) good *= -1;
        return good*1.0/all;
    } else if (DEBUG_ANGLE_METHOD == 3 || DEBUG_ANGLE_METHOD == 4) {
        // complex image moment
        int n = 3;
        Rect bbox = getBoundingBox();

        Point2d p(0, 0);
        int count = 0;
        for (int y=bbox.y; y<=bbox.y+bbox.height; y++)
            for (int x=bbox.x; x<=bbox.x+bbox.width; x++) {
                if (type->map->area_map.get(x, y) == this) {
                    if ((
                            type->map->area_map.get(x+1, y) == this &&
                            type->map->area_map.get(x-1, y) == this &&
                            type->map->area_map.get(x, y+1) == this &&
                            type->map->area_map.get(x, y-1) == this
                        ) || DEBUG_ANGLE_METHOD == 3) {
                        if (debug && DEBUG_ANGLE_METHOD == 4)
                            drawPoint(*debug, Point(x, y), Vec3b(0, 0, 255));
                        count++;
                        p += Point2d(x, y);
                    }
                }
            }
        p.x /= count;
        p.y /= count;

        complex<double> sum, sum2;
        for (int y=bbox.y; y<=bbox.y+bbox.height; y++)
            for (int x=bbox.x; x<=bbox.x+bbox.width; x++) {
                if (type->map->area_map.get(x, y) == this) {
                    if ((
                            type->map->area_map.get(x+1, y) == this &&
                            type->map->area_map.get(x-1, y) == this &&
                            type->map->area_map.get(x, y+1) == this &&
                            type->map->area_map.get(x, y-1) == this
                        ) || DEBUG_ANGLE_METHOD == 3) {
                        if (debug && DEBUG_ANGLE_METHOD == 4)
                            drawPoint(*debug, Point(x, y), Vec3b(0, 0, 255));

                        sum += pow(complex<double>(x - p.x, y - p.y), n);
                        sum2 += pow(complex<double>(x - p.x, y - p.y), 2);
                    }
                }
            }
        double ang = arg(sum)/n;
        double ang2 = arg(sum2)/2;;

        if (cos(ang2 - angle) < 0) {
            ang2 += M_PI;
        }
        if (debug) {
            Vec3b l(0, 0, 255);
            for (int i = 0; i<n; i++)
                drawLine(*debug, p, ang + 2*M_PI*i/n, 15, l);
        }
        ang = modulo(ang - (angle + M_PI/2), 2*M_PI/n);
        ang = min(ang, 2*M_PI/n - ang);
        if (debug) {
            stringstream ss;
            ss << "    " << arg(sum)/n << "-" << angle + M_PI/2;
            ss << "=" << ang;
            //drawBorderText(*debug, p, ss.str(), Vec3b(255, 255, 255), Vec3b(0, 0, 0));
            drawLine(*debug, p, angle + M_PI/2, 15, Vec3b(0, 255, 0));
            drawLine(*debug, p, ang2 + M_PI/2, 15, Vec3b(0, 128, 0));
        }
        return -ang;
    } else assert(0);
}
