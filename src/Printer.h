/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include "Font.h"

class Printer {
	public:

		typedef enum {
			FORMAT_RAW,
			FORMAT_JSON
		} Format;

		Printer(Format format);

		void printMetrics(const std::wstring & text, const Font::TextInfo & info) const;
		void printGlyphInfo(const std::string & character, const Font::Glyph & glyph) const;

	protected:
		void printBooleanProperty(const std::string & label, bool value) const;
		std::string mapJSONKeyName(const std::string & label) const;
		std::string printPropertyValue(const std::string & value, bool last = false) const;
		std::wstring printPropertyValue(const std::wstring & value, bool last = false) const;
		std::string printPropertyValue(size_t value, bool last = false) const;
		std::string printPropertyValue(const std::vector<std::string> & array, bool last = false) const;
		std::wstring printPropertyValue(const std::vector<wchar_t> & array, bool last = false) const;
		std::string printPropertyValue(const std::vector<size_t> & array, std::vector<std::string> labels, bool last = false) const;

	private:
		Format format_;
};