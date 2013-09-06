/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "FontInfo.h"
#include "FontAwesomeException.h"

#include FT_XFREE86_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TABLES_H


FontInfo::FontInfo() :
	bold_(false),
	italic_(false)
{
	textInfo_.emptyCount_ 	= 0;
	textInfo_.hitCount_ 	= 0;
}

std::wstring FontInfo::convertNameValue(unsigned char * str, size_t len, bool unicode) {
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


std::wstring FontInfo::splineNameFromId(int id) const {
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


std::wstring FontInfo::splinePlatformFromId(int id) const {
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


void FontInfo::parse(const Font & font, const std::wstring & text) {
	FT_Face face 				= font.face();

	textInfo_.length_ 			= text.length();
	textInfo_.missingEmpty_ 	= !font.missingExists();
	textInfo_.size_ 			= font.size(text);

	Glyph::Vector pen(0, 0);
	for (size_t index = 0; index < textInfo_.length_; ++index) {
		Glyph charGlyph = font.glyph(text[index], pen);

		if (charGlyph.empty_) {
			textInfo_.emptyCount_++;
		}
		else {
			textInfo_.hitCount_++;
		}
	}

	if (!face->charmap) {
		encoding_ = "missing";
	}
	else {
		switch (face->charmap->encoding) {
			case FT_ENCODING_NONE:
				encoding_ = "none";
				break;
			case FT_ENCODING_UNICODE:
				encoding_ = "unicode";
				break;
			case FT_ENCODING_ADOBE_STANDARD:
				encoding_ = "adobe standard";
				break;
			case FT_ENCODING_ADOBE_EXPERT:
				encoding_ = "adobe expert";
				break;
			case FT_ENCODING_MS_SYMBOL:
				encoding_ = "ms symbol";
				break;
			case FT_ENCODING_ADOBE_LATIN_1:
				encoding_ = "adobe latin 1";
				break;
			case FT_ENCODING_SJIS:
				encoding_ = "sjis";
				break;
			case FT_ENCODING_GB2312:
				encoding_ = "gb 2312";
				break;
			case FT_ENCODING_BIG5:
				encoding_ = "big 5";
				break;
			case FT_ENCODING_WANSUNG:
				encoding_ = "wansung";
				break;
			case FT_ENCODING_JOHAB:
				encoding_ = "johab";
				break;
			case FT_ENCODING_ADOBE_CUSTOM:
				encoding_ = "adobe custom";
				break;
			case FT_ENCODING_OLD_LATIN_2:
				encoding_ = "old latin 2";
				break;
			case FT_ENCODING_APPLE_ROMAN:
				encoding_ = "apple roman";
				break;
			default:
				encoding_ = "{unknown}";
				break;
		}
	}

	// populate info about the face itself
	name_ 				= FT_Get_Postscript_Name(face);
	haveGlyphNames_ 	= FT_HAS_GLYPH_NAMES(face);
	multipleMasters_ 	= FT_HAS_MULTIPLE_MASTERS(face);
	glyphCount_ 		= face->num_glyphs;
	family_ 			= face->family_name;
	style_ 				= face->style_name;
	sizes_ 				= face->num_fixed_sizes;
	format_ 			= FT_Get_X11_Font_Format(face);
	if (face->style_flags & FT_STYLE_FLAG_BOLD) {
		bold_ = true;
	}
	if (face->style_flags & FT_STYLE_FLAG_ITALIC) {
		italic_ = true;
	}
	vertical_ 	= FT_HAS_HORIZONTAL(face);
	horizontal_ = FT_HAS_VERTICAL(face);
	kerning_ 	= FT_HAS_KERNING(face);
	scalable_ 	= FT_IS_SCALABLE(face);
	if (scalable_) {
		maxAdvance_ = face->max_advance_width;
	}
	else {
		maxAdvance_ = face->size->metrics.max_advance;
	}

	FT_ULong charcode;
	FT_UInt glyphIndex;
	charcode = FT_Get_First_Char(face, &glyphIndex);
	char glyphBuffer[255];
	glyphNames_.reserve(glyphCount_);
	charmap_.reserve(glyphCount_);
	while (glyphIndex != 0) {
		charmap_.push_back(charcode);

		if (haveGlyphNames_) {
			if (FT_Get_Glyph_Name(face, glyphIndex, glyphBuffer, 255) != 0) {
				throw new FontAwesomeException("Error getting glyph name.");
			}
			glyphNames_.push_back(glyphBuffer);
		}

		charcode = FT_Get_Next_Char(face, charcode, &glyphIndex);
	}
}


void FontInfo::parseNames(Font & font) {
	size_t nameCount;
	FT_Face face = font.face();
	spline_ 	 = FT_IS_SFNT(face);
	if (!spline_) {
		return;
	}
	nameCount 	 = FT_Get_Sfnt_Name_Count(face);
	if (nameCount == 0) {
		return;
	}
	splineNames_.reserve(nameCount);
	FT_Error error;
	FT_SfntName name;
	std::wstring nameValue;
	for (size_t index = 0; index < nameCount; ++index) {
		if ((error = FT_Get_Sfnt_Name(face, index, &name)) != FT_Err_Ok) {
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
		splineNames_.push_back(splineName);
	}
}

