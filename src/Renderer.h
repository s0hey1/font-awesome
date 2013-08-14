/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once


#include "Font.h"
#include "Color.h"

#include <boost/shared_ptr.hpp>


class Image;

class Renderer {
	public:
		Renderer(bool debug, bool gracefulEmpty);
		~Renderer();

		boost::shared_ptr<Image> render(const Font & font, const Color & color, const std::wstring & text);
		void blit(const boost::shared_ptr<Image> & image, const Font::Glyph & glyph, const Color & color);

	private:
		bool debug_;
		bool gracefulEmpty_;
};