/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include <utility> // for pair

class Glyph {
	public:
		typedef std::pair<signed long, signed long> Vector;

		size_t index_;
		unsigned char * bitmap_;
		Vector advance_;
		Vector size_;
		Vector position_;
		bool empty_;
};
