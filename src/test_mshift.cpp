#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "mshift.hpp"

using namespace std;
using namespace cv;

string toString(const pair<Point, Vec3b>& p){
    stringstream ss;
    ss << "[";
    ss << p.first.x << ", ";
    ss << p.first.y << "][";
    ss << p.second[0]+0 << ", ";
    ss << p.second[1]+0 << ", ";
    ss << p.second[2]+0 << "]";
    return ss.str();
}

BOOST_AUTO_TEST_CASE(step_orphan) {
    Image img = Image::zeros(Size(7, 7));
    img(0, 0) = Vec3b(200, 0, 1);
    img(3, 3) = Vec3b(149, 0, 0);
    img(4, 3) = Vec3b(151, 0, 0);// only one
    BOOST_CHECK_EQUAL(
            toString(meanShiftStep(Point(3, 4), Vec3b(200, 0, 0), img, 2, 50)),
            toString(make_pair(Point(3, 4), Vec3b(151, 0, 0)))
        );
}
BOOST_AUTO_TEST_CASE(goto_zero) {
    Image img = Image::zeros(Size(7, 7));
    BOOST_CHECK_EQUAL(
            toString(meanShiftStep(Point(3, 4), Vec3b(49, 0, 0), img, 2, 50)),
            toString(make_pair(Point(3, 4), Vec3b(0, 0, 0)))
        );
}

BOOST_AUTO_TEST_CASE(goto_zero_on_border) {
    Image img = Image::zeros(Size(7, 17));
    BOOST_CHECK_EQUAL(
            toString(meanShiftStep(Point(6, 16), Vec3b(10, 0, 0), img, 3, 50)),
            toString(make_pair(Point(4, 14), Vec3b(0, 0, 0)))
        );
    // here we can assume almost equal weights for all pixels
    BOOST_CHECK_EQUAL(
            toString(meanShiftStep(Point(6, 16), Vec3b(10, 0, 0), img, 300, 50)),
            toString(make_pair(Point(6/2, 16/2), Vec3b(0, 0, 0)))
        );
}
