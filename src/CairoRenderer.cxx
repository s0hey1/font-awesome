/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "CairoRenderer.h"
#include "Image.h"
#include "FontAwesomeException.h"

// HarfBuzz
#include <hb.h>
#include <hb-ft.h>
#include <hb-icu.h>

// Cairo
#include <cairo.h>
#include <cairo-ft.h>

// ICU
#include <unicode/unistr.h>

#include <vector>
#include <iostream>
#include <cmath>


CairoRenderer::CairoRenderer(bool debug, bool gracefulEmpty, bool missing) : 
	Renderer(debug, gracefulEmpty, missing) 
{
}


boost::shared_ptr<Image> CairoRenderer::render(const Font & font, const Color & color, const std::wstring & text, const Color & emptyColor) {
	boost::shared_ptr<Image> image;
	cairo_font_face_t * 	cairoFace;
	cairo_matrix_t 			ctm;
	cairo_matrix_t 			fontMatrix;
	cairo_font_options_t *	fontOptions;
	cairo_scaled_font_t * 	scaledFont;
	FT_Face 				scaledFTFace;

	// wrap freetype font face in a cairo face object
	cairoFace = cairo_ft_font_face_create_for_ft_face(font.face(), 0);

	// setup default transformation matrices
	cairo_matrix_init_identity(&ctm);
	cairo_matrix_init_scale(&fontMatrix, font.pointSize(), font.pointSize());

	fontOptions = cairo_font_options_create();
	cairo_font_options_set_hint_style(fontOptions, CAIRO_HINT_STYLE_FULL);
	cairo_font_options_set_hint_metrics(fontOptions, CAIRO_HINT_METRICS_ON);
	cairo_font_options_set_antialias(fontOptions, CAIRO_ANTIALIAS_SUBPIXEL);

	// scale the face
	scaledFont = cairo_scaled_font_create(cairoFace, &fontMatrix, &ctm, fontOptions);
	scaledFTFace = cairo_ft_scaled_font_lock_face(scaledFont);

	hb_font_t * 	harfFont;
	hb_buffer_t * 	harfBuffer;
	const char * 	language = "en";

	harfFont = hb_ft_font_create(scaledFTFace, NULL);
	//hb_face_t * harfFace;
	//harfFace = hb_ft_face_create(scaledFTFace, NULL);
	harfBuffer = hb_buffer_create();
	hb_buffer_set_unicode_funcs(harfBuffer, hb_icu_get_unicode_funcs());
	// options are: HB_DIRECTION_LTR, HB_DIRECTION_RTL, HB_DIRECTION_TTB
	hb_buffer_set_direction(harfBuffer, HB_DIRECTION_LTR);
	// options: HB_SCRIPT_LATIN, HB_SCRIPT_ARABIC, HB_SCRIPT_HAN - see hb-unicode.h
	hb_buffer_set_script(harfBuffer, HB_SCRIPT_LATIN);
	hb_buffer_set_language(harfBuffer, hb_language_from_string(language, strlen(language)));

	// Layout the text
	//hb_buffer_add_utf8(harfBuffer, reinterpret_cast<const char *>(text.c_str()), text.size(), 0, text.size());
	icu::UnicodeString  ucs;
	std::string 		txt;
	ucs = icu::UnicodeString::fromUTF32(reinterpret_cast<const UChar32*>(text.c_str()), text.size());
	txt = ucs.toUTF8String(txt);
	hb_buffer_add_utf8(harfBuffer, txt.c_str(), txt.size(), 0, txt.size());
	hb_shape(harfFont, harfBuffer, NULL, 0);

	unsigned int 						glyphCount 	= 0;
	hb_glyph_info_t * 					glyphInfo;
	hb_glyph_position_t * 				glyphPosition;
	std::vector<cairo_glyph_t> 			cairoGlyphs;
	size_t 				 				pixelWidth 	= 2; // 1px border on each side
	size_t 				 				pixelHeight = 0;
	size_t 								x 			= 1; // 1st glyph positioned after left border
	cairo_text_extents_t 				cairoGlyphExtent;
	double								baseline;

	glyphInfo 		= hb_buffer_get_glyph_infos(harfBuffer, &glyphCount);
	glyphPosition 	= hb_buffer_get_glyph_positions(harfBuffer, &glyphCount);
	// preallocate space in the vector for all of the glyphs
	cairoGlyphs.reserve(glyphCount);

	// layout glyphs
	for (size_t index = 0; index < glyphCount; ++index) {
		cairo_glyph_t 		 cairoGlyph;

		cairoGlyph.index 	 = glyphInfo[index].codepoint;
		cairoGlyph.x 		 = x + (glyphPosition[index].x_offset / 64);
		cairoGlyph.y 		 = 0;
		x 					+= glyphPosition[index].x_advance / 64;
		cairoGlyphs[index] 	 = cairoGlyph;
	}

	cairo_scaled_font_glyph_extents(scaledFont, &cairoGlyphs[0], glyphCount, &cairoGlyphExtent);

	pixelWidth 	= cairoGlyphExtent.width + 2;
	pixelHeight = cairoGlyphExtent.height + 2;
	baseline 	= fabs(cairoGlyphExtent.y_bearing) + 1.0;

	// set vertical positioning & shift x bearing 
	for (size_t index = 0; index < glyphCount; ++index) {
		cairoGlyphs[index].y = baseline;
		cairoGlyphs[index].x -= cairoGlyphExtent.x_bearing;
	}	

	int 				 stride = 0;
	cairo_t * 			 cairoContext;
	cairo_surface_t * 	 cairoSurface;

	image.reset(new Image(pixelWidth, pixelHeight, 32));
	stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, pixelWidth);
	cairoSurface = cairo_image_surface_create_for_data(image->data(), CAIRO_FORMAT_ARGB32, pixelWidth, pixelHeight, stride);
	cairoContext = cairo_create(cairoSurface);
	cairo_set_source_rgba(
		cairoContext, 
		color.floatVal(Color::CHANNEL_BLUE), 
		color.floatVal(Color::CHANNEL_GREEN), 
		color.floatVal(Color::CHANNEL_RED), 
		color.floatVal(Color::CHANNEL_ALPHA)
	);
	//cairo_set_font_face(cairoContext, cairoFace);
	cairo_set_scaled_font(cairoContext, scaledFont);
	cairo_set_font_size(cairoContext, font.pointSize());
	//cairo_move_to(cairoContext, 0, baseline);
	cairo_show_glyphs(cairoContext, &cairoGlyphs[0], glyphCount);

	// all done! clean up after ourselves.
	cairo_font_options_destroy (fontOptions);
	cairo_font_face_destroy (cairoFace);

	hb_buffer_destroy(harfBuffer);

	cairo_destroy(cairoContext);
	cairo_surface_destroy(cairoSurface);

	return image;
}


