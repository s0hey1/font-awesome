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

// HarfBuzz
#include <hb.h>
#include <hb-ft.h>
#include <hb-ot.h>

#include <iostream>


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


std::wstring FontInfo::featureTagName(const std::string & s) const {
	std::wstring temp(4, L' ');
	std::copy(s.begin(), s.begin() + 4, temp.begin());
	return temp; 
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
	// Some TTF fonts cause a segfault in FT when querying for the postscript name so we can't do this :(
	//name_ 				= FT_Get_Postscript_Name(face);

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

	// access OpenType features
	hb_face_t * 							harfFace;
	unsigned int 							featureCount;
	std::wstring 							featureName;
	std::vector<hb_tag_t> 					scripts;
	std::vector<hb_tag_t> 					features;
	std::vector<hb_tag_t>::const_iterator 	tagIterator;
	char 									tagName[5];

	harfFace = hb_ft_face_create(face, NULL);

	// scripts
	featureCount = hb_ot_layout_table_get_script_tags(harfFace, HB_OT_TAG_GSUB, 0, NULL, NULL);
	scripts.reserve(featureCount);
	for (size_t index = 0; index < featureCount; index++) {
		hb_tag_t tag;
		scripts.push_back(tag);
	}
	hb_ot_layout_table_get_script_tags(harfFace, HB_OT_TAG_GSUB, 0, &featureCount, &scripts[0]);
	tagIterator = scripts.begin();
	for (; tagIterator != scripts.end(); ++tagIterator) {
		hb_tag_to_string(*tagIterator, tagName);
		featureName = featureTagName(tagName);
		std::pair<std::wstring, std::wstring> feature(
			featureName, 
			scriptDescriptionFromName(featureName)
		);
		scripts_.push_back(feature);
	}

	// languages
	// need to do this for each script:
	//featureCount = hb_ot_layout_script_get_language_tags(harfFace, HB_OT_TAG_GSUB, scriptIndex, 0, NULL, NULL);

	// features
	featureCount = hb_ot_layout_table_get_feature_tags(harfFace, HB_OT_TAG_GSUB, 0, NULL, NULL);
	features.reserve(featureCount);
	for (size_t index = 0; index < featureCount; index++) {
		hb_tag_t tag;
		features.push_back(tag);
	}
	featureCount = hb_ot_layout_table_get_feature_tags(harfFace, HB_OT_TAG_GSUB, 0, &featureCount, &features[0]);
	tagIterator = features.begin();
	for (; tagIterator != features.end(); ++tagIterator) {
		hb_tag_to_string(*tagIterator, tagName);
		featureName = featureTagName(tagName);
		std::pair<std::wstring, std::wstring> feature(
			featureName, 
			featureDescriptionFromName(featureName)
		);
		features_.push_back(feature);
	}
}

std::wstring FontInfo::scriptDescriptionFromName(const std::wstring & tag) const {
	if (tag == L"arab") {
		return L"Arabic";
	}
	if (tag == L"armn") {
		return L"Armenian";
	}
	if (tag == L"beng") {
		return L"Bengali";
	}
	if (tag == L"bopo") {
		return L"Bopomofo";
	}
	if (tag == L"brai") {
		return L"Braille";
	}
	if (tag == L"byzm") {
		return L"Byzantine Music";
	}
	if (tag == L"cans") {
		return L"Canadian Syllabics";
	}
	if (tag == L"cher") {
		return L"Cherokee";
	}
	if (tag == L"hani") {
		return L"CJK Ideographic";
	}
	if (tag == L"cyrl") {
		return L"Cyrillic";
	}
	if (tag == L"DFLT") {
		return L"Default";
	}
	if (tag == L"deva") {
		return L"Devanagari";
	}
	if (tag == L"ethi") {
		return L"Ethiopic";
	}
	if (tag == L"geor") {
		return L"Georgian";
	}
	if (tag == L"grek") {
		return L"Greek";
	}
	if (tag == L"gujr") {
		return L"Gujarati";
	}
	if (tag == L"guru") {
		return L"Gurmukhi";
	}
	if (tag == L"jamo") {
		return L"Hangul Jamo";
	}
	if (tag == L"hang") {
		return L"Hangul";
	}
	if (tag == L"hebr") {
		return L"Hebrew";
	}
	if (tag == L"kana") {
		return L"Hiragana";
	}
	if (tag == L"knda") {
		return L"Kannada";
	}
	if (tag == L"kana") {
		return L"Katakana";
	}
	if (tag == L"khmr") {
		return L"Khmer";
	}
	if (tag == L"lao") {
		return L"Lao";
	}
	if (tag == L"latn") {
		return L"Latin";
	}
	if (tag == L"mlym") {
		return L"Malayalam";
	}
	if (tag == L"mong") {
		return L"Mongolian";
	}
	if (tag == L"mymr") {
		return L"Myanmar";
	}
	if (tag == L"ogam") {
		return L"Ogham";
	}
	if (tag == L"orya") {
		return L"Oriya";
	}
	if (tag == L"runr") {
		return L"Runic";
	}
	if (tag == L"sinh") {
		return L"Sinhala";
	}
	if (tag == L"syrc") {
		return L"Syriac";
	}
	if (tag == L"taml") {
		return L"Tamil";
	}
	if (tag == L"telu") {
		return L"Telugu";
	}
	if (tag == L"thaa") {
		return L"Thaana";
	}
	if (tag == L"thai") {
		return L"Thai";
	}
	if (tag == L"tibt") {
		return L"Tibetan";
	}
	if (tag == L"yi") {
		return L"Yi";
	}
	return L"{unknown tag}";
}

std::wstring FontInfo::featureDescriptionFromName(const std::wstring & tag) const {
	if (tag == L"aalt") {
		return L"Access All Alternates";
	}
	if (tag == L"abvf") {
		return L"Above-base Forms";
	}
	if (tag == L"abvm") {
		return L"Above-base Mark Positioning";
	}
	if (tag == L"abvs") {
		return L"Above-base Substitutions";
	}
	if (tag == L"afrc") {
		return L"Alternative Fractions";
	}
	if (tag == L"akhn") {
		return L"Akhands";
	}
	if (tag == L"blwf") {
		return L"Below-base Forms";
	}
	if (tag == L"blwm") {
		return L"Below-base Mark Positioning";
	}
	if (tag == L"blws") {
		return L"Below-base Substitutions";
	}
	if (tag == L"calt") {
		return L"Contextual Alternates";
	}
	if (tag == L"case") {
		return L"Case-Sensitive Forms";
	}
	if (tag == L"ccmp") {
		return L"Glyph Composition / Decomposition";
	}
	if (tag == L"clig") {
		return L"Contextual Ligatures";
	}
	if (tag == L"cpsp") {
		return L"Capital Spacing";
	}
	if (tag == L"cswh") {
		return L"Contextual Swash";
	}
	if (tag == L"curs") {
		return L"Cursive Positioning";
	}
	if (tag == L"c2sc") {
		return L"Small Capitals From Capitals";
	}
	if (tag == L"c2pc") {
		return L"Petite Capitals From Capitals";
	}
	if (tag == L"dist") {
		return L"Distances";
	}
	if (tag == L"dlig") {
		return L"Discretionary Ligatures";
	}
	if (tag == L"dnom") {
		return L"Denominators";
	}
	if (tag == L"expt") {
		return L"Expert Forms";
	}
	if (tag == L"falt") {
		return L"Final Glyph on Line Alternates";
	}
	if (tag == L"fin2") {
		return L"Terminal Forms #2";
	}
	if (tag == L"fin3") {
		return L"Terminal Forms #3";
	}
	if (tag == L"fina") {
		return L"Terminal Forms";
	}
	if (tag == L"frac") {
		return L"Fractions";
	}
	if (tag == L"fwid") {
		return L"Full Widths";
	}
	if (tag == L"half") {
		return L"Half Forms";
	}
	if (tag == L"haln") {
		return L"Halant Forms";
	}
	if (tag == L"halt") {
		return L"Alternate Half Widths";
	}
	if (tag == L"hist") {
		return L"Historical Forms";
	}
	if (tag == L"hkna") {
		return L"Horizontal Kana Alternates";
	}
	if (tag == L"hlig") {
		return L"Historical Ligatures";
	}
	if (tag == L"hngl") {
		return L"Hangul";
	}
	if (tag == L"hojo") {
		return L"Hojo Kanji Forms";
	}
	if (tag == L"hwid") {
		return L"Half Widths";
	}
	if (tag == L"init") {
		return L"Initial Forms";
	}
	if (tag == L"isol") {
		return L"Isolated Forms";
	}
	if (tag == L"ital") {
		return L"Italics";
	}
	if (tag == L"jalt") {
		return L"Justification Alternates";
	}
	if (tag == L"jp04") {
		return L"JIS2004 Forms";
	}
	if (tag == L"jp78") {
		return L"JIS78 Forms";
	}
	if (tag == L"jp83") {
		return L"JIS83 Forms";
	}
	if (tag == L"jp90") {
		return L"JIS90 Forms";
	}
	if (tag == L"kern") {
		return L"Kerning";
	}
	if (tag == L"lfbd") {
		return L"Left Bounds";
	}
	if (tag == L"liga") {
		return L"Standard Ligatures";
	}
	if (tag == L"ljmo") {
		return L"Leading Jamo Forms";
	}
	if (tag == L"lnum") {
		return L"Lining Figures";
	}
	if (tag == L"locl") {
		return L"Localized Forms";
	}
	if (tag == L"mark") {
		return L"Mark Positioning";
	}
	if (tag == L"med2") {
		return L"Medial Forms #2";
	}
	if (tag == L"medi") {
		return L"Medial Forms";
	}
	if (tag == L"mgrk") {
		return L"Mathematical Greek";
	}
	if (tag == L"mkmk") {
		return L"Mark to Mark Positioning";
	}
	if (tag == L"mset") {
		return L"Mark Positioning via Substitution";
	}
	if (tag == L"nalt") {
		return L"Alternate Annotation Forms";
	}
	if (tag == L"nlck") {
		return L"NLC Kanji Forms";
	}
	if (tag == L"nukt") {
		return L"Nukta Forms";
	}
	if (tag == L"numr") {
		return L"Numerators";
	}
	if (tag == L"onum") {
		return L"Oldstyle Figures";
	}
	if (tag == L"opbd") {
		return L"Optical Bounds";
	}
	if (tag == L"ordn") {
		return L"Ordinals";
	}
	if (tag == L"ornm") {
		return L"Ornaments";
	}
	if (tag == L"palt") {
		return L"Proportional Alternate Widths";
	}
	if (tag == L"pcap") {
		return L"Petite Capitals";
	}
	if (tag == L"pnum") {
		return L"Proportional Figures";
	}
	if (tag == L"pres") {
		return L"Pre-Base Substitutions";
	}
	if (tag == L"pstf") {
		return L"Post-base Forms";
	}
	if (tag == L"psts") {
		return L"Post-base Substitutions";
	}
	if (tag == L"pwid") {
		return L"Proportional Widths";
	}
	if (tag == L"qwid") {
		return L"Quarter Widths";
	}
	if (tag == L"rand") {
		return L"Randomize";
	}
	if (tag == L"rlig") {
		return L"Required Ligatures";
	}
	if (tag == L"rphf") {
		return L"Reph Forms";
	}
	if (tag == L"rtbd") {
		return L"Right Bounds";
	}
	if (tag == L"rtla") {
		return L"Right-to-left Alternates";
	}
	if (tag == L"ruby") {
		return L"Ruby Notation Forms";
	}
	if (tag == L"salt") {
		return L"Stylistic Alternates";
	}
	if (tag == L"sinf") {
		return L"Scientific Inferiors";
	}
	if (tag == L"size") {
		return L"Optical size";
	}
	if (tag == L"smcp") {
		return L"Small Capitals";
	}
	if (tag == L"smpl") {
		return L"Simplified Forms";
	}
	if (tag == L"ss01") {
		return L"Stylistic Set 1";
	}
	if (tag == L"ss02") {
		return L"Stylistic Set 2";
	}
	if (tag == L"ss03") {
		return L"Stylistic Set 3";
	}
	if (tag == L"ss04") {
		return L"Stylistic Set 4";
	}
	if (tag == L"ss05") {
		return L"Stylistic Set 5";
	}
	if (tag == L"ss06") {
		return L"Stylistic Set 6";
	}
	if (tag == L"ss07") {
		return L"Stylistic Set 7";
	}
	if (tag == L"ss08") {
		return L"Stylistic Set 8";
	}
	if (tag == L"ss09") {
		return L"Stylistic Set 9";
	}
	if (tag == L"ss10") {
		return L"Stylistic Set 10";
	}
	if (tag == L"ss11") {
		return L"Stylistic Set 11";
	}
	if (tag == L"ss12") {
		return L"Stylistic Set 12";
	}
	if (tag == L"ss13") {
		return L"Stylistic Set 13";
	}
	if (tag == L"ss14") {
		return L"Stylistic Set 14";
	}
	if (tag == L"ss15") {
		return L"Stylistic Set 15";
	}
	if (tag == L"ss16") {
		return L"Stylistic Set 16";
	}
	if (tag == L"ss17") {
		return L"Stylistic Set 17";
	}
	if (tag == L"ss18") {
		return L"Stylistic Set 18";
	}
	if (tag == L"ss19") {
		return L"Stylistic Set 19";
	}
	if (tag == L"ss20") {
		return L"Stylistic Set 20";
	}
	if (tag == L"subs") {
		return L"Subscript";
	}
	if (tag == L"sups") {
		return L"Superscript";
	}
	if (tag == L"swsh") {
		return L"Swash";
	}
	if (tag == L"titl") {
		return L"Titling";
	}
	if (tag == L"tjmo") {
		return L"Trailing Jamo Forms";
	}
	if (tag == L"tnam") {
		return L"Traditional Name Forms";
	}
	if (tag == L"tnum") {
		return L"Tabular Figures";
	}
	if (tag == L"trad") {
		return L"Traditional Forms";
	}
	if (tag == L"twid") {
		return L"Third Widths";
	}
	if (tag == L"unic") {
		return L"Unicase";
	}
	if (tag == L"valt") {
		return L"Alternate Vertical Metrics";
	}
	if (tag == L"vatu") {
		return L"Vattu Variants";
	}
	if (tag == L"vert") {
		return L"Vertical Writing";
	}
	if (tag == L"vhal") {
		return L"Alternate Vertical Half Metrics";
	}
	if (tag == L"vjmo") {
		return L"Vowel Jamo Forms";
	}
	if (tag == L"vkna") {
		return L"Vertical Kana Alternates";
	}
	if (tag == L"vkrn") {
		return L"Vertical Kerning";
	}
	if (tag == L"vpal") {
		return L"Proportional Alternate Vertical Metrics";
	}
	if (tag == L"vrt2") {
		return L"Vertical Alternates and Rotation";
	}
	if (tag == L"zero") {
		return L"Slashed Zero";
	}
	return L"{unknown tag}";
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

