/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include <string>

/**
 * A simple utility for dealing with color specifiers
 */
class Color {
	public:
		/**
		 * Default constructor
		 */
		Color();

		/**
		 * Construct a color from a raw hex color string
		 */
		Color(const std::string & hex);

		bool fromHex(const std::string & hex);
		bool validateHex(const std::string & hex) const;

		unsigned char red() const;
		unsigned char blue() const;
		unsigned char green() const;
		unsigned char alpha() const;

	private:
		unsigned char red_;
		unsigned char green_;
		unsigned char blue_;
		unsigned char alpha_;
};
