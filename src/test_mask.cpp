#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "mask.hpp"

#include <vector>
using namespace cv;
using namespace std;

BOOST_AUTO_TEST_CASE(test_cross_bottomleft_topright) {
    int x=10, y=50;
    Point p1(x+0, y+0), p2(x+1, y+1);

    BOOST_CHECK_EQUAL(crossingPoint(p1, p2, y+2), x+2);
}

BOOST_AUTO_TEST_CASE(test_cross_topleft_bottomright) {
    int x=-10, y=5000;
    Point p1(x+0, y+5), p2(x+10, y+0);

    BOOST_CHECK_EQUAL(crossingPoint(p1, p2, y+4), x+2);
}

BOOST_AUTO_TEST_CASE(test_cross_special) {
    BOOST_CHECK_EQUAL(crossingPoint(Point(0, 0), Point(1000, 1), 0), 0);
    BOOST_CHECK_EQUAL(crossingPoint(Point(0, 0), Point(1000, 1), 1), 1000);

    crossingPoint(Point(0, 0), Point(1, 0), 0);
}

BOOST_AUTO_TEST_CASE(test_polygon) {
	/*
	  /\
	 /__\

	 */
	vector<Point> poly;
	poly.push_back(Point(2, 0));
	poly.push_back(Point(0, 2));
	poly.push_back(Point(4, 2));

	BOOST_CHECK(pointInPolygon(Point(2, 1), poly));
	BOOST_CHECK(!pointInPolygon(Point(5, 0), poly));
	BOOST_CHECK(!pointInPolygon(Point(2, 5), poly));
}
