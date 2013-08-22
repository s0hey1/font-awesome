/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include "Font.h"

class Printer {
	public:


		void printMetrics(const std::wstring & text, const Font::TextInfo & info) const;
		void printGlyphInfo(const std::string & character, const Font::Glyph & glyph) const;

	protected:
		void printBooleanProperty(const std::string & label, bool value) const;
};