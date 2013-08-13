/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include <stdexcept>

/**
 * Application specific exception type
 */
class FontAwesomeException : public std::runtime_error {
	public:
		FontAwesomeException(const std::string & what);
};