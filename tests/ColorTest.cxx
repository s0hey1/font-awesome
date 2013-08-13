#include <boost/test/unit_test.hpp>

#include "../src/Color.h"

BOOST_AUTO_TEST_CASE (color_test) {

	// test default constructor
	Color color;
	BOOST_CHECK_EQUAL(color.red(), 0x00);
	BOOST_CHECK_EQUAL(color.green(), 0x00);
	BOOST_CHECK_EQUAL(color.blue(), 0x00);
	BOOST_CHECK_EQUAL(color.alpha(), 0xFF);

	
}