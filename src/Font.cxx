/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Font.h"
#include "FontAwesomeException.h"

#include <algorithm>

// Our FreeType magic numbers
#define FREETYPE_DPI 100
#define FREETYPE_PEN_DPI 64

Font::Font(const std::string & filename, int size) {
	FT_Error error;
	if ((error = FT_Init_FreeType(&library_)) != 0) {
		throw FontAwesomeException("Error initializing freetype library!");
	}

	// load font file
	if ((error = FT_New_Face(library_, filename.c_str(), 0, &face_)) != 0) {
		throw FontAwesomeException("Error creating new freetype face!");
		FT_Done_FreeType(library_);
	}

	// set char size
	if ((error = FT_Set_Char_Size(face_, size * FREETYPE_PEN_DPI, 0, FREETYPE_DPI, 0)) != 0) {
		throw FontAwesomeException("Error setting freetype char size!");
		release();
	}
}

Font::~Font() {
	release();
}

void Font::release() {
	FT_Done_Face(face_);
	FT_Done_FreeType(library_);
}

Font::Pen Font::size(const std::string & text) {
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
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}
	maxx = std::max(maxx, static_cast<int>(pen.x / FREETYPE_PEN_DPI));
	maxy = std::max(maxy, static_cast<int>(pen.y / FREETYPE_PEN_DPI));

	Pen dimensions(maxx - minx, maxy - miny);
	return dimensions;
}

/*
boost::shared_ptr<Image> Font::render(const std::string & text) {
	FT_Vector pen;
	pen.x = 0;
	pen.y = 0;
	size_t length = text.length();
	for (size_t i = 0; i < length; i++) {
		index = FT_Get_Char_Index(face_, text[i]);
		FT_Set_Transform(face_, NULL, &pen);
		FT_Load_Char(face_, text[i], FT_LOAD_RENDER);
		if ((index == 0) && (flags & FONTAWESOME_FLAG_FIX_MISSING_MISSING)) {
			draw_empty_glyph(rows, (int)pen.x/FREETYPE_PEN_DPI, 0, (int)slot->advance.x/FREETYPE_PEN_DPI, height, text_color);
		} 
		else {
			blit_bitmap(rows, &slot->bitmap, slot->bitmap_left, height - slot->bitmap_top, text_color);
		}
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}
}
*/