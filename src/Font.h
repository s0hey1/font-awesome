/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Glyph.h"

#include <string>
#include <vector>

/**
 * A FreeType2 Font
 */
class Font {
	public:
		typedef struct {
			Glyph::Vector size_;
			Glyph::Vector min_;
			Glyph::Vector max_;
		} Range;

		Font(const std::string & filename, int size);
		~Font();

		/**
		 * Get the point size of the font
		 *
		 * @return int
		 */
		int pointSize() const;

		/**
		 * Get the space required to render a string of text
		 */
		Range size(const std::wstring & text) const;

		/**
		 * Get the rendered glyph for a single character
		 */
		Glyph glyph(wchar_t character, Glyph::Vector pen) const;

		/**
		 * Get the empty glyph
		 */
		Glyph emptyGlyph() const;

		size_t penDPI() const;
		size_t dpi() const;

		/**
		 * Check glyph 0 to see if it exists or not
		 */ 
		bool missingExists() const;

		FT_Face face() const;

	protected:
		void release();
		std::wstring splineNameFromId(int id) const;
		std::wstring splinePlatformFromId(int id) const;

	private:
		FT_Library library_;
		FT_Face face_;
		size_t dpi_;
		size_t penDPI_;
		int size_;
};