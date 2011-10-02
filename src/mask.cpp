
#include "mask.hpp"

#include <algorithm>
using namespace cv;

/** calculate line crossing point for line and y axis.
 *
 * The first line (not a segment) is given by two points, the second
 * is given by y axis value.
 */
int crossingPoint(Point line1, Point line2, int y) {
    int x1 = line1.x;
    int y1 = line1.y;
    int x2 = line2.x;
    int y2 = line2.y;

    double x_pos = x1 + ((x2-x1)*1.0/(y2-y1)*(y-y1));
    return x_pos;
}

bool pointInPolygon(Point point, const std::vector<Point>& polygon) {
	std::vector<int> intersections;
	for (int i=0; i<polygon.size(); i++) {
		Point p0 = polygon[i];
		Point p1 = polygon[(i+1)%polygon.size()];

		if (p0.y > p1.y) std::swap(p0, p1);

		if (p0.y <= point.y && point.y < p1.y) {
			intersections.push_back(crossingPoint(p0, p1, point.y));
		}
	}
	sort(intersections.begin(), intersections.end());

	bool odd = false;
	for (int i=0; i<intersections.size() && intersections[i] < point.x; i++) {
		odd = !odd;
	}
	return odd;
}

void ImageMask::init(vector<Point> poly, Size dimensions) {
	mask.resize(dimensions);
	if (poly.empty()) return;
	for (int y=0; y<dimensions.height; y++)
		for (int x=0; x<dimensions.width; x++) {
			mask.set(x, y, !pointInPolygon(Point(x, y), poly));
		}
}

void ImageMask::apply(Image& img) {
	//mask.get(img.cols, img.rows);
	for (int y=0; y<img.rows; y++)
		for (int x=0; x<img.cols; x++) {
			if (mask.get(x, y))
				img(y, x) = Vec3b(0, 0, 0);
		}
}
