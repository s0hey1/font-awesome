/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <utility> // for pair

/**
 * A FreeType2 Font
 */
class Font {
	public:
		typedef std::pair<signed long, signed long> Pen;

		Font(const std::string & filename, int size);
		~Font();

		/**
		 * Get the space required to render a string of text
		 */
		Pen size(const std::string & text);

	protected:
		void release();

	private:
		FT_Library library_;
		FT_Face face_;
};