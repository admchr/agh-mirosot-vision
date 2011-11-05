#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "moments.hpp"

using namespace cv;

BOOST_AUTO_TEST_CASE(test_small_slope) {
    PatchMoments moments;
    moments.add(Point(400, 200));
    moments.add(Point(200, 100));

    BOOST_CHECK(moments.getMean() == Point(300, 150));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), atan(1.0/2.0), 1e-6);
}

BOOST_AUTO_TEST_CASE(test_undefined_angle) {
    const double UNDEFINED = 0;

    PatchMoments moments;

    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), UNDEFINED, 1e-6);

    moments.add(Point(0, 0));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), UNDEFINED, 1e-6);
    BOOST_CHECK(moments.getMean() == Point(0, 0));

    moments.add(Point(0, 1));
    moments.add(Point(1, 0));
    moments.add(Point(1, 1));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), UNDEFINED, 1e-6);
    BOOST_CHECK(moments.getMean() == Point(0, 0));

}


BOOST_AUTO_TEST_CASE(test_large_slope) {
    PatchMoments moments;
    moments.add(Point(100, 200));
    moments.add(Point(200, 400));

    BOOST_CHECK(moments.getMean() == Point(150, 300));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), atan(2.0/1.0), 1e-6);
}

BOOST_AUTO_TEST_CASE(test_small_slope_neg) {
    PatchMoments moments;
    moments.add(Point(400, -200));
    moments.add(Point(200, -100));

    BOOST_CHECK(moments.getMean() == Point(300, -150));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), -atan(1.0/2.0), 1e-6);
}
BOOST_AUTO_TEST_CASE(test_large_slope_neg) {
    PatchMoments moments;
    moments.add(Point(100, -200));
    moments.add(Point(200, -400));

    BOOST_CHECK(moments.getMean() == Point(150, -300));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), -atan(2.0/1.0), 1e-6);
}

BOOST_AUTO_TEST_CASE(test_two_vertical) {
    PatchMoments moments;
    moments.add(Point(100, 200));
    moments.add(Point(100, 400));

    BOOST_CHECK(moments.getMean() == Point(100, 300));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), M_PI/2, 1e-6);
}
BOOST_AUTO_TEST_CASE(test_two_horizontal) {
    PatchMoments moments;
    moments.add(Point(200, 200));
    moments.add(Point(100, 200));

    BOOST_CHECK(moments.getMean() == Point(150, 200));
    BOOST_CHECK_CLOSE_FRACTION(moments.getAngle(), 0, 1e-6);
}
