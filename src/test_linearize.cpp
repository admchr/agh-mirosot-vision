#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "linearize.hpp"

using namespace cv;



BOOST_AUTO_TEST_CASE(test_srgb_to_rgb) {
	BOOST_CHECK_EQUAL(srgb_to_rgb(0), 0);
	BOOST_CHECK_EQUAL(srgb_to_rgb(255), 254);
	BOOST_CHECK_EQUAL(srgb_to_rgb(128), 55);
	BOOST_CHECK_EQUAL(srgb_to_rgb(188), 128);
}

BOOST_AUTO_TEST_CASE(test_rgb_to_srgb) {
	BOOST_CHECK_EQUAL(rgb_to_srgb(0), 0);
	BOOST_CHECK_EQUAL(rgb_to_srgb(255), 255);
	BOOST_CHECK_EQUAL(rgb_to_srgb(55), 128);
	BOOST_CHECK_EQUAL(rgb_to_srgb(128), 188);
}

BOOST_AUTO_TEST_CASE(test_linearize) {
	Image img(1, 1);

	img(0, 0) = Vec3b(255, 0, 128);
	linearize(img);
	BOOST_CHECK(img(0, 0) == Vec3b(254, 0, 55));
}
BOOST_AUTO_TEST_CASE(test_delinearize) {
	Image img(1, 1);

	img(0, 0) = Vec3b(255, 0, 128);
	delinearize(img);
	BOOST_CHECK(img(0, 0) == Vec3b(255, 0, 188));
}
