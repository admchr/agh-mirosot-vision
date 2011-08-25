#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE basic_tests
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "balance.h"

using namespace cv;

BOOST_AUTO_TEST_CASE(median_test) {
    Image img = Image::zeros(cv::Size(50, 40));
    
    img(10, 21) = Vec3b(6, 12, 3);
    img(11, 21) = Vec3b(4, 12, 3);
    img(12, 21) = Vec3b(3, 12, 3);
    
    img(10, 22) = Vec3b(9, 12, 3);
    img(11, 22) = Vec3b(2, 12, 3);
    img(12, 22) = Vec3b(5, 12, 4);
    
    img(10, 23) = Vec3b(8, 12, 3);
    img(11, 23) = Vec3b(7, 12, 3);
    img(12, 23) = Vec3b(1, 12, 3);
    
    image_pos pos;
    pos.x=22;
    pos.y=11;
    BOOST_CHECK(median(img, pos, 1) == Vec3b(5, 12, 3));
    
    pos.x=0;
    pos.y=0;
    BOOST_CHECK(median(img, pos, 1) == Vec3b(0, 0, 0));
    pos.x=49;
    pos.y=39;
    BOOST_CHECK(median(img, pos, 1) == Vec3b(0, 0, 0));
    BOOST_CHECK(median(img, pos, 100) == Vec3b(0, 0, 0));
    
}
