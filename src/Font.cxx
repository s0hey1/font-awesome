/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Font.h"
#include "FontAwesomeException.h"

#include FT_XFREE86_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TABLES_H

#include <algorithm>
#include <iostream>

Font::Font(const std::string & filename, int size) :
	dpi_(100),
	penDPI_(64)
{
	FT_Error error;
	if ((error = FT_Init_FreeType(&library_)) != 0) {
		throw FontAwesomeException("Error initializing freetype library!");
	}

	// load font file
	if ((error = FT_New_Face(library_, filename.c_str(), 0, &face_)) != 0) {
		FT_Done_FreeType(library_);
		if (error == FT_Err_Unknown_File_Format) {
			throw FontAwesomeException("Unsupported font file format!");
		}
		else {
			throw FontAwesomeException("Error creating new freetype face!");
		}
	}

	// set char size
	if ((error = FT_Set_Char_Size(face_, size * penDPI_, 0, dpi_, 0)) != 0) {
		release();
		throw FontAwesomeException("Error setting freetype char size!");
	}
}

Font::~Font() {
	release();
}

size_t Font::penDPI() const {
	return penDPI_;
}

size_t Font::dpi() const {
	return dpi_;
}


void Font::release() {
	FT_Done_Face(face_);
	FT_Done_FreeType(library_);
}

Font::Range Font::size(const std::wstring & text) const {
	FT_Vector pen;
	FT_GlyphSlot slot;
	int top;
	int left;
	slot = face_->glyph;

	pen.x = 0;
	pen.y = 0;

	// Measure glyphs first, to see how big of a canvas we need
	int minx = 0;
	int miny = 0;
	int maxx = 0;
	int maxy = 0;
	size_t textLength = text.length();
	for (size_t i = 0; i < textLength; i++) {
		FT_Set_Transform(face_, NULL, &pen);
		FT_Load_Char(face_, text[i], FT_LOAD_RENDER);
		left = slot->bitmap_left;
		top = 0 - slot->bitmap_top;
		minx = std::min(minx, left);
		miny = std::min(miny, top);
		maxx = std::max(maxx, left + slot->bitmap.width);
		maxy = std::max(maxy, top + slot->bitmap.rows);

		if (slot->advance.x == 0) {
			if (FT_IS_SCALABLE(face_)) {
				pen.x += face_->max_advance_width;
			}
			else {
				pen.x += face_->size->metrics.max_advance;
			}
		}
		else {
			pen.x += slot->advance.x;
		}

		pen.y += slot->advance.y;
	}
	maxx = std::max(maxx, static_cast<int>(pen.x / penDPI_));
	maxy = std::max(maxy, static_cast<int>(pen.y / penDPI_));

	Range range;
	range.size_ = Vector(maxx - minx, maxy - miny);
	range.max_ = Vector(maxx, maxy);
	range.min_ = Vector(minx, miny);

	return range;
}

std::wstring convertNameValue(FT_Byte * str, size_t len, bool unicode) {
	std::wstring nameValue;

	if (!unicode) {
		for (size_t charCount = 0; charCount < len; ++charCount) {
			nameValue.push_back(str[charCount]);
		}
		return nameValue;
	}

	FT_Int ch = 0;
	for (size_t index = 0; index < len; index += 2) {
		ch = (str[index] << 8) | str[index + 1];
		nameValue.push_back(ch);
	}

	return nameValue;
}


Font::TextInfo Font::metrics(const std::wstring & text) const {
	TextInfo info;

	info.length_ 		= text.length();
	info.missingEmpty_ 	= !missingExists();
	info.size_ 			= size(text);
	info.emptyCount_ 	= 0;
	info.hitCount_ 		= 0;

	Vector pen(0, 0);
	for (size_t index = 0; index < info.length_; ++index) {
		Glyph charGlyph = glyph(text[index], pen);

		if (charGlyph.empty_) {
			info.emptyCount_++;
		}
		else {
			info.hitCount_++;
		}
	}

	if (!face_->charmap) {
		info.faceInfo_.encoding_ = "missing";
	}
	else {
		switch (face_->charmap->encoding) {
			case FT_ENCODING_NONE:
				info.faceInfo_.encoding_ = "none";
				break;
			case FT_ENCODING_UNICODE:
				info.faceInfo_.encoding_ = "unicode";
				break;
			case FT_ENCODING_ADOBE_STANDARD:
				info.faceInfo_.encoding_ = "adobe standard";
				break;
			case FT_ENCODING_ADOBE_EXPERT:
				info.faceInfo_.encoding_ = "adobe expert";
				break;
			case FT_ENCODING_MS_SYMBOL:
				info.faceInfo_.encoding_ = "ms symbol";
				break;
			case FT_ENCODING_ADOBE_LATIN_1:
				info.faceInfo_.encoding_ = "adobe latin 1";
				break;
		}
	}

	// populate info about the face itself
	info.faceInfo_.name_ = FT_Get_Postscript_Name(face_);
	info.faceInfo_.haveGlyphNames_ = FT_HAS_GLYPH_NAMES(face_);
	info.faceInfo_.multipleMasters_ = FT_HAS_MULTIPLE_MASTERS(face_);
	info.faceInfo_.glyphCount_ = face_->num_glyphs;
	info.faceInfo_.family_ = face_->family_name;
	info.faceInfo_.style_ = face_->style_name;
	info.faceInfo_.sizes_ = face_->num_fixed_sizes;
	info.faceInfo_.format_ = FT_Get_X11_Font_Format(face_);
	if (face_->style_flags & FT_STYLE_FLAG_BOLD) {
		info.faceInfo_.bold_ = true;
	}
	else {
		info.faceInfo_.bold_ = false;
	}
	if (face_->style_flags & FT_STYLE_FLAG_ITALIC) {
		info.faceInfo_.italic_ = true;
	}
	else {
		info.faceInfo_.italic_ = false;
	}
	info.faceInfo_.vertical_ = FT_HAS_HORIZONTAL(face_);
	info.faceInfo_.horizontal_ = FT_HAS_VERTICAL(face_);
	info.faceInfo_.kerning_ = FT_HAS_KERNING(face_);
	info.faceInfo_.scalable_ = FT_IS_SCALABLE(face_);
	if (info.faceInfo_.scalable_) {
		info.faceInfo_.maxAdvance_ = face_->max_advance_width;
	}
	else {
		info.faceInfo_.maxAdvance_ = face_->size->metrics.max_advance;
	}

	size_t nameCount;
	info.faceInfo_.spline_ = FT_IS_SFNT(face_);
	nameCount = FT_Get_Sfnt_Name_Count(face_);
	FT_Error error;
	FT_SfntName name;
	std::wstring nameValue;
	for (size_t index = 0; index < nameCount; ++index) {
		if ((error = FT_Get_Sfnt_Name(face_, index, &name)) != FT_Err_Ok) {
			continue;
		}
		nameValue.clear();
		switch (name.platform_id) {
			case TT_PLATFORM_APPLE_UNICODE:
				switch (name.encoding_id) {
					case TT_APPLE_ID_DEFAULT:
					case TT_APPLE_ID_UNICODE_1_1:
					case TT_APPLE_ID_ISO_10646:
					case TT_APPLE_ID_UNICODE_2_0:
						nameValue = convertNameValue(name.string, name.string_len, true);
						break;
					default:
						nameValue = L"{unsupported encoding}";
						break;
				}
				break;
			case TT_PLATFORM_MACINTOSH:
				switch (name.encoding_id) {
					case TT_MAC_ID_ROMAN:
						nameValue = convertNameValue(name.string, name.string_len, false);
						break;
					default:
						nameValue = L"{unsupported encoding}";
				}
				break;
			case TT_PLATFORM_ISO:
				switch (name.encoding_id) {
					case TT_ISO_ID_7BIT_ASCII:
					case TT_ISO_ID_8859_1:
						nameValue = convertNameValue(name.string, name.string_len, false);
						break;
					case TT_ISO_ID_10646:
						nameValue = convertNameValue(name.string, name.string_len, true);
						break;
					default:
						nameValue = L"{unsupported encoding}";
						break;
				}
				break;
			case TT_PLATFORM_MICROSOFT:
				switch (name.encoding_id) {
					case TT_MS_ID_SYMBOL_CS:
					case TT_MS_ID_UNICODE_CS:
						nameValue = convertNameValue(name.string, name.string_len, true);
						break;
					default:
						nameValue = L"{unsupported encoding}";
						break;
				}
				break;
			default:
				nameValue = L"{unsupported platform}";
				break;
		}

		std::pair<std::wstring, std::wstring> splineName(splineNameFromId(name.name_id), nameValue);
		info.faceInfo_.splineNames_.push_back(splineName);
	}

	FT_ULong charcode;
	FT_UInt glyphIndex;
	charcode = FT_Get_First_Char(face_, &glyphIndex);
	char glyphBuffer[255];

	while (glyphIndex != 0) {
		info.faceInfo_.charmap_.push_back(charcode);

		if (info.faceInfo_.haveGlyphNames_) {
			if (FT_Get_Glyph_Name(face_, glyphIndex, glyphBuffer, 255) != 0) {
				throw new FontAwesomeException("Error getting glyph name.");
			}
			info.faceInfo_.glyphNames_.push_back(glyphBuffer);
		}

		charcode = FT_Get_Next_Char(face_, charcode, &glyphIndex);
	}


	return info;
}

std::wstring Font::splineNameFromId(int id) const {
	// mappings taken from ftdemos ftdump.c
	switch (id) {
		case TT_NAME_ID_COPYRIGHT:
			return L"copyright";
		case TT_NAME_ID_FONT_FAMILY:
			return L"font family";
		case TT_NAME_ID_FONT_SUBFAMILY:
			return L"font subfamily";
		case TT_NAME_ID_UNIQUE_ID:
			return L"unique ID";
		case TT_NAME_ID_FULL_NAME:
			return L"full name";
		case TT_NAME_ID_VERSION_STRING:
			return L"version string";
		case TT_NAME_ID_PS_NAME:
			return L"PostScript name";
		case TT_NAME_ID_TRADEMARK:
			return L"trademark";

	   /* the following values are from the OpenType spec */
		case TT_NAME_ID_MANUFACTURER:
			return L"manufacturer";
		case TT_NAME_ID_DESIGNER:
			return L"designer";
		case TT_NAME_ID_DESCRIPTION:
			return L"description";
		case TT_NAME_ID_VENDOR_URL:
			return L"vendor URL";
		case TT_NAME_ID_DESIGNER_URL:
			return L"designer URL";
		case TT_NAME_ID_LICENSE:
			return L"license";
		case TT_NAME_ID_LICENSE_URL:
			return L"license URL";
		/* number 15 is reserved */
		case TT_NAME_ID_PREFERRED_FAMILY:
			return L"preferred family";
		case TT_NAME_ID_PREFERRED_SUBFAMILY:
			return L"preferred subfamily";
		case TT_NAME_ID_MAC_FULL_NAME:
			return L"Mac full name";

	   /* The following code is new as of 2000-01-21 */
		case TT_NAME_ID_SAMPLE_TEXT:
			return L"sample text";

	   /* This is new in OpenType 1.3 */
		case TT_NAME_ID_CID_FINDFONT_NAME:
			return L"CID `findfont' name";

		default:
			return L"UNKNOWN";
	}
}


std::wstring Font::splinePlatformFromId(int id) const {
	switch (id) {
		case TT_PLATFORM_APPLE_UNICODE:
			return L"Apple (Unicode)";
		case TT_PLATFORM_MACINTOSH:
			return L"Macintosh";
		case TT_PLATFORM_ISO:
			return L"ISO (deprecated)";
		case TT_PLATFORM_MICROSOFT:
			return L"Microsoft";
		case TT_PLATFORM_CUSTOM:
			return L"custom";
		case TT_PLATFORM_ADOBE:
			return L"Adobe";
		default:
			return L"UNKNOWN";
	}
}


Font::Glyph Font::glyph(wchar_t character, Font::Vector pen) const {
	size_t index = FT_Get_Char_Index(face_, character);
	FT_Vector ftPen;
	FT_GlyphSlot slot;
	Glyph glyph;

	ftPen.x = pen.first;
	ftPen.y = pen.second;

	FT_Set_Transform(face_, NULL, &ftPen);
	FT_Load_Char(face_, character, FT_LOAD_RENDER);

	if (index == 0) {
		glyph.empty_ = true;
	}
	else {
		glyph.empty_ = false;
	}

	slot = face_->glyph;

	if (slot->advance.x == 0) {
		if (FT_IS_SCALABLE(face_)) {
			glyph.advance_.first = face_->max_advance_width;
		}
		else {
			glyph.advance_.first = face_->size->metrics.max_advance;
		}
	}
	else {
		glyph.advance_.first 	= slot->advance.x;
	}
	glyph.advance_.second 	= slot->advance.y;
	glyph.index_ 			= index;
	glyph.bitmap_ 			= slot->bitmap.buffer;
	glyph.position_.first 	= slot->bitmap_left;
	glyph.position_.second 	= slot->bitmap_top;
	glyph.size_.first 		= slot->bitmap.width;
	glyph.size_.second 		= slot->bitmap.rows;

	return glyph;
}

Font::Glyph Font::emptyGlyph() const {
	FT_GlyphSlot slot;
	Glyph glyph;

	FT_Load_Glyph(face_, 0, FT_LOAD_RENDER);

	slot = face_->glyph;

	glyph.index_ 			= 0;
	glyph.empty_ 			= true;
	glyph.advance_.first 	= slot->advance.x;
	glyph.advance_.second 	= slot->advance.y;
	glyph.bitmap_ 			= slot->bitmap.buffer;
	glyph.position_.first 	= slot->bitmap_left;
	glyph.position_.second 	= slot->bitmap_top;
	glyph.size_.first 		= slot->bitmap.width;
	glyph.size_.second 		= slot->bitmap.rows;

	return glyph;
}


bool Font::missingExists() const {
	size_t top;
	FT_Load_Glyph(face_, 0, FT_LOAD_RENDER);
	// bitmap coords are in cartesian space & not image space
	top = 0 + face_->glyph->bitmap_top;
	if (top == 0) {
		return false;
	}
	// treat an empty glyph with no size as a missing empty glyph
	if (face_->glyph->bitmap.width == 0 && face_->glyph->bitmap.rows == 0) {
		return false;
	}
	return true;
}

