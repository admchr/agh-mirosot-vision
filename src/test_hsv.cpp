#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "hsvconverter.hpp"

#include <iostream>

using namespace cv;
using namespace std;

Vec3b print(Vec3b c) {
    cout << c[0]+0 << " " << c[1]+0 << " " << c[2]+0 << endl;
    return c;
}

BOOST_AUTO_TEST_CASE(test_to_hsl) {
    BOOST_CHECK(hsvconverter.fromBGRToHSL(Vec3b(0, 0, 0)) == Vec3b(0, 0, 0));
    BOOST_CHECK(hsvconverter.fromBGRToHSL(Vec3b(255, 255, 255)) == Vec3b(0, 0, 255));

    BOOST_CHECK(hsvconverter.fromBGRToHSL(Vec3b(0, 0, 255)) == Vec3b(0, 255, 255/3));
    BOOST_CHECK(print(hsvconverter.fromBGRToHSL(Vec3b(0, 255, 255))) == Vec3b(30, 255, 2*255/3));
    BOOST_CHECK(hsvconverter.fromBGRToHSL(Vec3b(0, 255, 0)) == Vec3b(30*2, 255, 255/3));
    BOOST_CHECK(print(hsvconverter.fromBGRToHSL(Vec3b(255, 255, 0))) == Vec3b(30*3, 255, 2*255/3));
    BOOST_CHECK(hsvconverter.fromBGRToHSL(Vec3b(255, 0, 0)) == Vec3b(30*4, 255, 255/3));
    BOOST_CHECK(print(hsvconverter.fromBGRToHSL(Vec3b(255, 0, 255))) == Vec3b(30*5, 255, 2*255/3));

    BOOST_CHECK(hsvconverter.fromBGRToHSL(Vec3b(123, 123, 123)) == Vec3b(0, 0, 123));
    BOOST_CHECK(hsvconverter.fromBGRToHSL(Vec3b(128, 255, 128)) == Vec3b(30*2, 127, (2*128 + 255)/3));
}
