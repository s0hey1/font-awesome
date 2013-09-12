/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Color.h"

#include <cstdlib>


Color::Color() :
	red_(0x00),
	green_(0x00),
	blue_(0x00),
	alpha_(0xFF) {
}


Color::Color(const std::string & hex) {
	fromHex(hex);
}

float Color::floatVal(Channel channel) const {
	unsigned char c;
	switch (channel) {
		case CHANNEL_RED:
			c = red_;
			break;
		case CHANNEL_GREEN:
			c = green_;
			break;
		case CHANNEL_BLUE:
			c = blue_;
			break;
		case CHANNEL_ALPHA:
			c = alpha_;
			break;
	}
	float val = c / 255.;
	return val;
}


bool Color::fromHex(const std::string & hex) {
	if (!validateHex(hex)) {
		return false;
	}
	unsigned long color = strtol(hex.c_str(), NULL, 16);

	red_ = (color >> 24) & 0xFF;
	green_ = (color >> 16) & 0xFF;
	blue_ = (color >> 8) & 0xFF;
	alpha_ = (color) & 0xFF;

	return true;
}

bool Color::validateHex(const std::string & hex) const {
	if (hex.length() != 8) {
		return false;
	}
	int character;
	for (unsigned int index = 0; index < 8; ++index) {
		character = hex[index];
		if (!(
			(character >= 48 && character <= 57) // 0-9
			||
			(character >= 65 && character <= 70) // A-F
			||
			(character >= 97 && character <= 102) // a-f
		)) {
			return false;
		}
	}
	return true;
}

unsigned char Color::red() const {
	return red_;
}

unsigned char Color::blue() const {
	return blue_;
}

unsigned char Color::green() const {
	return green_;
}

unsigned char Color::alpha() const {
	return alpha_;
}

