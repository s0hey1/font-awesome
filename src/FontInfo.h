/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include "Font.h"

#include <string>
#include <vector>


class FontInfo {
	public:
		typedef struct {
			size_t 		length_;
			size_t 		emptyCount_;
			size_t 		missingEmpty_;
			size_t 		hitCount_;
			Font::Range size_;
		} TextInfo;

		std::string 	name_;
		std::string 	family_;
		std::string 	style_;
		std::string 	format_;
		size_t 			sizes_;
		size_t 			glyphCount_;
		bool 			spline_;
		std::vector<std::pair<std::wstring, std::wstring> > splineNames_;
		bool 			horizontal_;
		bool 			vertical_;
		bool 			bold_;
		bool 			italic_;
		bool 			kerning_;
		bool 			scalable_;
		size_t 			maxAdvance_;
		std::vector<wchar_t> charmap_;
		std::string 	encoding_;
		bool 			haveGlyphNames_;
		bool 			multipleMasters_;
		std::vector<std::string> glyphNames_;
		// OpenType Table Tags
		std::vector<std::pair<std::wstring, std::wstring> > features_;
		std::vector<std::pair<std::wstring, std::wstring> > languages_;
		std::vector<std::pair<std::wstring, std::wstring> > scripts_;

		TextInfo 		textInfo_;

		FontInfo();

		/**
		 * Get information about what a font can do with a string of text
		 */
		void parse(const Font & font, const std::wstring & text);

	protected:
		void parseNames(Font & font);
		std::wstring convertNameValue(unsigned char * str, size_t len, bool unicode);
		std::wstring splineNameFromId(int id) const;
		std::wstring splinePlatformFromId(int id) const;
		std::wstring featureTagName(const std::string & s) const;
		std::wstring featureDescriptionFromName(const std::wstring & tag) const;
		std::wstring scriptDescriptionFromName(const std::wstring & tag) const;
};
