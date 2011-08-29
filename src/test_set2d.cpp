#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "set2d.hpp"

BOOST_AUTO_TEST_CASE(set2d_test){
    Set2d<bool> set(3, 5);
    
    BOOST_CHECK(!set.get(1, 2));
    
    set.set(1, 2, true);
    
    BOOST_CHECK(set.get(1, 2));
    BOOST_CHECK(!set.get(2, 1));
}

BOOST_AUTO_TEST_CASE(set2d_test_outofrange) {
    Set2d<bool> set(3, 5);
    set.set(19, 2, true);
    
}

struct Fun {
    Fun() {
        ok = true;
        count = 0;
    }
    bool ok;
    int count;
    int operator()(int x, int y, int val) {
        ok = ok && x < 3 && y < 5 && x >= 0 && y >= 0;
        count++;
        if (x == 2 && y == 4) {
            ok = ok && val == 123;
            return 124;
        }
        return val;
    }
};

BOOST_AUTO_TEST_CASE(set2d_test_foreach) {
    Set2d<int> set;
    set.resize(cv::Size(3, 5));
    Fun fun;
    
    set.set(2, 4, 123);
    set.set(19, 2, 1245);
    
    set.forEach(fun);
    
    BOOST_CHECK_EQUAL(fun.count, 3*5);
    BOOST_CHECK(fun.ok);
    BOOST_CHECK_EQUAL(set.get(2, 4), 124);
}