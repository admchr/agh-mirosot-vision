#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "mask.hpp"

using namespace cv;

BOOST_AUTO_TEST_CASE(test_right_of_bottomleft_topright) {
    int x=10, y=50;
    Point p1(x+0, y+0), p2(x+1, y+1);

    BOOST_CHECK(!rightOf(
            Point(x+2, y+2), p1, p2
        ));// on line
    BOOST_CHECK(rightOf(
            Point(x+3, y+2), p1, p2
        ));
    BOOST_CHECK(!rightOf(
            Point(x+1, y+2), p1, p2
        ));
}

BOOST_AUTO_TEST_CASE(test_right_of_topleft_bottomright) {
    int x=-10, y=5000;
    Point p1(x+0, y+5), p2(x+10, y+0);

    BOOST_CHECK(!rightOf(
            Point(x+2, y+4), p1, p2
        ));// on line
    BOOST_CHECK(rightOf(
            Point(x+3, y+4), p1, p2
        ));
    BOOST_CHECK(!rightOf(
            Point(x+1, y+4), p1, p2
        ));
}

BOOST_AUTO_TEST_CASE(test_right_of_special) {
    BOOST_CHECK(rightOf(Point(2, 0), Point(0, 0), Point(1000, 1)));
    BOOST_CHECK(!rightOf(Point(1000-1, 1), Point(0, 0), Point(1000, 1)));

    rightOf(Point(2, 0), Point(0, 0), Point(1, 0));
}
