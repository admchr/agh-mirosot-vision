#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MODULE set2d_tests
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "set2d.hpp"

BOOST_AUTO_TEST_CASE(set2d_test){
    Set2d<bool> set(3, 5);
    set.set(1, 2, true);
    
    BOOST_CHECK(set.get(1, 2));
    BOOST_CHECK(!set.get(2, 1));
}
