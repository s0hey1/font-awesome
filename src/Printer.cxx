/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Printer.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>

#include <boost/algorithm/string.hpp>


Printer::Printer(Format format) :
	format_(format)
{
}


std::string Printer::mapJSONKeyName(const std::string & label) const {
	if (format_ == FORMAT_RAW) {
		std::string key = label + std::string(": ");
		return key;
	}
	std::string key = label;
	std::replace(key.begin(), key.end(), ' ', '_');
	boost::algorithm::to_lower(key);
	key = std::string("\"") + key + std::string("\": ");
	return key;
}


std::string Printer::printPropertyValue(const std::string & value, bool last) const {
	if (format_ == FORMAT_RAW) {
		return value;
	}
	std::string json = std::string("\"") + value + std::string("\"");
	//std::replace(json.begin(), json.end(), '\"', "\\\"");
	if (!last) {
		json += std::string(",");
	}
	return json;
}

std::string Printer::printPropertyValue(size_t value, bool last) const {
	std::ostringstream ss;
	ss << value;

	if (format_ == FORMAT_RAW) {
		return ss.str();
	}

	std::string json = ss.str();
	if (!last) {
		json += std::string(",");
	}
	return json;
}

std::string Printer::printPropertyValue(const std::vector<std::string> & array, bool last) const {
	std::ostringstream ss;
	std::vector<std::string>::const_iterator it = array.begin();
	size_t count = array.size();
	size_t index = 0;
	if (format_ == FORMAT_JSON) {
		ss << "[";
	}
	for (; it != array.end(); ++it) {
		index++;
		if (format_ == FORMAT_JSON) {
			ss << "\"";
			if ((*it) == std::string("\"")) {
				ss << "\\";
			}
		}
		ss << (*it);
		if (format_ == FORMAT_JSON) {
			ss << "\"";
		}
		if (index < count) {
			ss << ", ";
		}
	}
	if (format_ == FORMAT_JSON) {
		ss << "]";
		if (!last) {
			ss << ",";
		}
	}
	return ss.str();
}

std::string Printer::printPropertyValue(const std::vector<size_t> & array, std::vector<std::string> labels, bool last) const {
	std::ostringstream ss;
	std::vector<size_t>::const_iterator it = array.begin();
	size_t count = array.size();
	size_t labelCount = labels.size();
	size_t index = 0;
	ss << "{";
	for (; it != array.end(); ++it) {
		if (format_ == FORMAT_JSON) {
			if (labelCount > index) {
				ss << "\"" << labels[index] << "\": ";
			}
		}
		index++;
		ss << (*it);
		if (index < count) {
			ss << ", ";
		}
	}
	ss << "}";
	if (!last && format_ == FORMAT_JSON) {
		ss << ",";
	}
	return ss.str();
}

std::wstring Printer::printPropertyValue(const std::vector<wchar_t> & array, bool last) const {
	std::wostringstream ss;
	std::vector<wchar_t>::const_iterator it = array.begin();
	size_t count = array.size();
	size_t index = 0;
	if (format_ == FORMAT_JSON) {
		ss << L"[";
	}
	wchar_t quote = '"';
	wchar_t backslash = '\\';
	for (; it != array.end(); ++it) {
		index++;
		if (format_ == FORMAT_JSON) {
			ss << L"\"";
			if ((*it) == quote || (*it) == backslash) {
				ss << L"\\";
			}
		}
		ss << (*it);
		if (format_ == FORMAT_JSON) {
			ss << L"\"";
		}
		if (index < count) {
			ss << L", ";
		}
	}
	if (format_ == FORMAT_JSON) {
		ss << L"]";
		if (!last) {
			ss << L",";
		}
	}
	return ss.str();
}


std::wstring Printer::printPropertyValue(const std::wstring & value, bool last) const {
	if (format_ == FORMAT_RAW) {
		return value;
	}
	std::wostringstream ss;
	ss << std::wstring(L"\"") << value << std::wstring(L"\"");
	if (!last) {
		ss << std::wstring(L",");
	}
	return ss.str();
}


void Printer::printMetrics(const std::wstring & text, const Font::TextInfo & info) const {
	if (format_ == FORMAT_JSON) {
		std::wcout << "{" << std::endl;
	}
	std::wcout << mapJSONKeyName("Text Input").c_str() << printPropertyValue(text) << std::endl;
	std::wcout << mapJSONKeyName("Font Name").c_str() << printPropertyValue(info.faceInfo_.name_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Font Family").c_str() << printPropertyValue(info.faceInfo_.family_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Font Style").c_str() << printPropertyValue(info.faceInfo_.style_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Available Glyphs").c_str() << printPropertyValue(info.faceInfo_.glyphCount_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Available Sizes").c_str() << printPropertyValue(info.faceInfo_.sizes_).c_str() << std::endl;

	printBooleanProperty("Bold", info.faceInfo_.bold_);
	printBooleanProperty("Italic", info.faceInfo_.italic_);
	printBooleanProperty("Kerning Available", info.faceInfo_.kerning_);
	printBooleanProperty("Scalable", info.faceInfo_.scalable_);
	printBooleanProperty("Have Glyph Names", info.faceInfo_.haveGlyphNames_);
	printBooleanProperty("Multiple Masters", info.faceInfo_.multipleMasters_);

	std::wcout << mapJSONKeyName("Text Length").c_str() << printPropertyValue(info.length_).c_str() << std::endl;

	printBooleanProperty("Missing Empty Glyph", info.missingEmpty_);

	std::wcout << mapJSONKeyName("Renderable Glyphs").c_str() << printPropertyValue(info.hitCount_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Non-renderable Glyphs").c_str() << printPropertyValue(info.emptyCount_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Encoding").c_str() << printPropertyValue(info.faceInfo_.encoding_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Character Map").c_str() << printPropertyValue(info.faceInfo_.charmap_) << std::endl;
	if (info.faceInfo_.haveGlyphNames_) {
		std::wcout << mapJSONKeyName("Glyph Names").c_str() << printPropertyValue(info.faceInfo_.glyphNames_).c_str() << std::endl;
	}
	std::wcout << mapJSONKeyName("Max Advance Width").c_str() << printPropertyValue(info.faceInfo_.maxAdvance_, true).c_str() << std::endl;

	if (format_ == FORMAT_JSON) {
		std::wcout << "}" << std::endl;
	}
}

void Printer::printGlyphInfo(const std::string & character, const Font::Glyph & glyph) const {
	if (format_ == FORMAT_JSON) {
		std::wcout << "{" << std::endl;
	}

	printBooleanProperty("Empty", glyph.empty_);
	if (!glyph.empty_) {
		std::wcout << mapJSONKeyName("Glyph Character").c_str() << printPropertyValue(character).c_str() << std::endl;
	}
	std::vector<std::string> dimLabels;
	dimLabels.push_back("x");
	dimLabels.push_back("y");

	std::vector<std::string> sizeLabels;
	sizeLabels.push_back("width");
	sizeLabels.push_back("height");

	std::vector<size_t> advance;
	advance.push_back(glyph.advance_.first);
	advance.push_back(glyph.advance_.second);

	std::vector<size_t> size;
	size.push_back(glyph.size_.first);
	size.push_back(glyph.size_.second);

	std::vector<size_t> position;
	position.push_back(glyph.position_.first);
	position.push_back(glyph.position_.second);

	std::wcout << mapJSONKeyName("Index").c_str() << printPropertyValue(glyph.index_).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Advance").c_str() << printPropertyValue(advance, dimLabels).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Size").c_str() << printPropertyValue(size, sizeLabels).c_str() << std::endl;
	std::wcout << mapJSONKeyName("Position").c_str() << printPropertyValue(position, dimLabels, true).c_str() << std::endl;

	if (format_ == FORMAT_JSON) {
		std::wcout << "}" << std::endl;
	}
}


void Printer::printBooleanProperty(const std::string & label, bool value) const {
	std::wcout << mapJSONKeyName(label.c_str()).c_str();

	if (format_ == FORMAT_JSON) {
		if (value) {
			std::wcout << "true";
		}
		else {
			std::wcout << "false";
		}
		std::wcout << ",";
	}
	else {

		if (value) {
			std::wcout << "Yes";
		}
		else {
			std::wcout << "No";
		}
	}
	std::wcout << std::endl;
}
