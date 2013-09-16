/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once


#include "Font.h"
#include "Color.h"

#include <boost/shared_ptr.hpp>


class Image;

/**
 * Base renderer class used to render a string of text into a raw image buffer
 *
 */
class Renderer {
	public:
		/**
		 * Default constructor
		 *
		 * @param bool debug
		 * @param bool gracefulEmpty
		 * @param bool missing
		 */
		Renderer(bool debug, bool gracefulEmpty, bool missing);
		virtual ~Renderer();

		virtual boost::shared_ptr<Image> render(const Font & font, const Color & color, const std::wstring & text, const Color & emptyColor) = 0;
		virtual void features(const std::vector<std::string> & features) = 0;

		bool debug() const;
		bool gracefulEmpty() const;
		bool missing() const;

	private:
		bool debug_;
		bool gracefulEmpty_;
		bool missing_;
};