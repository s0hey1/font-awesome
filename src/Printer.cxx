/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Printer.h"

#include <iostream>
#include <string>

void Printer::printMetrics(const std::wstring & text, const Font::TextInfo & info) const {
	std::wcout << "Text Input: " << text << std::endl;
	std::wcout << "Font Name: " << info.faceInfo_.name_.c_str() << std::endl;
	std::wcout << "Font Family: " << info.faceInfo_.family_.c_str() << std::endl;
	std::wcout << "Font Style: " << info.faceInfo_.style_.c_str() << std::endl;
	std::wcout << "Available Glyphs: " << info.faceInfo_.glyphCount_ << std::endl;
	std::wcout << "Available Sizes: " << info.faceInfo_.sizes_ << std::endl;

	printBooleanProperty("Bold: ", info.faceInfo_.bold_);
	printBooleanProperty("Italic: ", info.faceInfo_.italic_);
	printBooleanProperty("Kerning Available: ", info.faceInfo_.kerning_);
	printBooleanProperty("Scalable: ", info.faceInfo_.scalable_);

	std::wcout << "Text Length: " << info.length_ << std::endl;

	printBooleanProperty("Missing Empty Glyph: ", info.missingEmpty_);

	std::wcout << "Renderable Glyphs: " << info.hitCount_ << std::endl;
	std::wcout << "Non-renderable Glyphs: " << info.emptyCount_ << std::endl;
	std::wcout << "Character Map: " << info.charmap_ << std::endl;
	std::wcout << "Max Advance Width: " << info.faceInfo_.maxAdvance_ << std::endl;
}

void Printer::printGlyphInfo(const std::string & character, const Font::Glyph & glyph) const {
	printBooleanProperty("Empty: ", glyph.empty_);
	if (!glyph.empty_) {
		std::wcout << "Glyph Character: " << character.c_str() << std::endl;
	}
	std::wcout << "Index: " << glyph.index_ << std::endl;
	std::wcout << "Advance: [" << glyph.advance_.first << ", " << glyph.advance_.second << "]" << std::endl;
	std::wcout << "Size: [" << glyph.size_.first << "x" << glyph.size_.second << "]" << std::endl;
	std::wcout << "Position: [" << glyph.position_.first << ", " << glyph.position_.second << "]" << std::endl;

}


void Printer::printBooleanProperty(const std::string & label, bool value) const {
	std::wcout << label.c_str();

	if (value) {
		std::wcout << "Yes";
	}
	else {
		std::wcout << "No";
	}
	std::wcout << std::endl;
}
