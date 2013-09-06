/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include "FontInfo.h"

class Printer {
	public:

		typedef enum {
			FORMAT_RAW,
			FORMAT_JSON,
			FORMAT_XML
		} Format;

		Printer(Format format, bool useCodepoints);

		void printMetrics(const std::wstring & text, const FontInfo & info) const;
		void printGlyphInfo(const std::string & character, const Glyph & glyph) const;

	protected:
		std::wstring mapKeyName(const std::wstring & label) const;

	private:
		Format 	format_;
		bool	codepoints_;
};