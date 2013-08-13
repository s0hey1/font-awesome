/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "FontAwesomeException.h"

FontAwesomeException::FontAwesomeException(const std::string & what) : std::runtime_error(what) {

}