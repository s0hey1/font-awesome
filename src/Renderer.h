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
 * Render a string of text into a raw image buffer
 *
 */
class Renderer {
	public:
		Renderer(bool debug, bool gracefulEmpty, bool missing);
		~Renderer();

		boost::shared_ptr<Image> render(const Font & font, const Color & color, const std::wstring & text, const Color & emptyColor);
		void blit(const boost::shared_ptr<Image> & image, const Glyph & glyph, const Color & color);
		void drawRect(const boost::shared_ptr<Image> & image, size_t x, size_t y, size_t width, size_t height, const Color & color);
		void drawPoint(const boost::shared_ptr<Image> & image, size_t pixel, const Color & color, unsigned char alpha);

	private:
		bool debug_;
		bool gracefulEmpty_;
		bool missing_;
};