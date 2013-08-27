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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <unicode/unistr.h>


Printer::Printer(Format format, bool useCodepoints) :
	format_(format),
	codepoints_(useCodepoints)
{
}

std::wstring Printer::mapKeyName(const std::wstring & label) const {
	if (format_ == FORMAT_RAW) {
		std::wstring key = label + std::wstring(L": ");
		return key;
	}
	std::wstring key = label;
	std::replace(key.begin(), key.end(), ' ', '_');
	boost::algorithm::to_lower(key);

	return key;
}


void Printer::printMetrics(const std::wstring & text, const Font::TextInfo & info) const {
	boost::property_tree::wptree pt;

	pt.put(mapKeyName(L"Text Input"), text);
	pt.put(mapKeyName(L"Text Length"), info.length_);
	pt.put(mapKeyName(L"Font Name"), info.faceInfo_.name_.c_str());
	pt.put(mapKeyName(L"Font Family"), info.faceInfo_.family_.c_str());
	pt.put(mapKeyName(L"Font Style"), info.faceInfo_.style_.c_str());
	pt.put(mapKeyName(L"Available Glyphs"), info.faceInfo_.glyphCount_);
	pt.put(mapKeyName(L"Available Sizes"), info.faceInfo_.sizes_);

	pt.put(mapKeyName(L"Bold"), info.faceInfo_.bold_);
	pt.put(mapKeyName(L"Italic"), info.faceInfo_.italic_);
	pt.put(mapKeyName(L"Kerning Available"), info.faceInfo_.kerning_);
	pt.put(mapKeyName(L"Scalable"), info.faceInfo_.scalable_);
	pt.put(mapKeyName(L"Have Glyph Names"), info.faceInfo_.haveGlyphNames_);
	pt.put(mapKeyName(L"Multiple Masters"), info.faceInfo_.multipleMasters_);
	pt.put(mapKeyName(L"Max Advance Width"), info.faceInfo_.maxAdvance_);

	pt.put(mapKeyName(L"Missing Empty Glyph"), info.missingEmpty_);
	pt.put(mapKeyName(L"Renderable Glyphs"), info.hitCount_);
	pt.put(mapKeyName(L"Non-renderable Glyphs"), info.emptyCount_);
	pt.put(mapKeyName(L"Encoding"), info.faceInfo_.encoding_.c_str());


	std::vector<wchar_t>::const_iterator it = info.faceInfo_.charmap_.begin();
	std::wstring charmapKeyName = mapKeyName(L"Character Map");
	std::wstring childKey;
	if (format_ == FORMAT_XML) {
		childKey = mapKeyName(L"Char");
	}
	else {
		childKey = L"";
	}
	boost::property_tree::wptree arrayChild;
	boost::property_tree::wptree arrayElement;
	icu::UnicodeString ucs;
	for (; it != info.faceInfo_.charmap_.end(); ++it) {
		if (codepoints_) {
			ucs = *it;
			arrayElement.put_value(ucs.char32At(0));
		}
		else {
			arrayElement.put_value(*it);
		}
		arrayChild.push_back(std::make_pair(childKey, arrayElement));
	}
	pt.put_child(charmapKeyName, arrayChild);

	std::wstring glyphKeyName = mapKeyName(L"Glyph Names");
	if (format_ == FORMAT_XML) {
		childKey = mapKeyName(L"Glyph");
	}
	else {
		childKey = L"";
	}
	if (info.faceInfo_.haveGlyphNames_) {
		arrayChild.clear();
		std::vector<std::string>::const_iterator glyphIterator = info.faceInfo_.glyphNames_.begin();
		for (; glyphIterator != info.faceInfo_.glyphNames_.end(); ++glyphIterator) {
			arrayElement.put_value((*glyphIterator).c_str());
			arrayChild.push_back(std::make_pair(childKey, arrayElement));
		}
	}
	pt.put_child(glyphKeyName, arrayChild);

	if (format_ == FORMAT_JSON) {
		boost::property_tree::write_json(std::wcout, pt);
	}
	else if (format_ == FORMAT_XML) {
		boost::property_tree::write_xml(std::wcout, pt);
	}
}

void Printer::printGlyphInfo(const std::string & character, const Font::Glyph & glyph) const {
	boost::property_tree::wptree pt;

	pt.put(mapKeyName(L"Empty"), glyph.empty_);

	if (!glyph.empty_) {
		pt.put(mapKeyName(L"Glyph Character"), character.c_str());
	}

	pt.put(mapKeyName(L"Index"), glyph.index_);

	pt.put(mapKeyName(L"Advance.x"), glyph.advance_.first);
	pt.put(mapKeyName(L"Advance.y"), glyph.advance_.second);

	pt.put(mapKeyName(L"Size.width"), glyph.size_.first);
	pt.put(mapKeyName(L"Size.height"), glyph.size_.second);

	pt.put(mapKeyName(L"Position.x"), glyph.position_.first);
	pt.put(mapKeyName(L"Position.y"), glyph.position_.second);

	if (format_ == FORMAT_JSON) {
		boost::property_tree::write_json(std::wcout, pt);
	}
	else if (format_ == FORMAT_XML) {
		boost::property_tree::write_xml(std::wcout, pt);
	}
}
