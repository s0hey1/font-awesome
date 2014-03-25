#include <pango/pangocairo.h>


boost::shared_ptr<Image> render(const Font & font, const Color & color, const std::wstring & text, const Color & emptyColor) {

	cairo_t * cairoContext;
	cairo_surface_t * cairoSurface;

	// TODO - how do we figure out surface size we need?
	size_t width;
	size_t height;
	cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cairoContext = cairo_create(cairoSurface);

	// set color to use for drawing
	cairo_set_source_rgb(cairoContext, 1.0, 1.0, 1.0);
	// flood fill with the color we just set
	cairo_paint(cairoContext);

	PangoLayout *layout;
	layout = pango_cairo_create_layout(cr);

	PangoFontDescription * desc;
	//desc = pango_font_description_new();
	FcPattern * pattern;
	desc = pango_fc_font_description_from_pattern(pattern, true);
	pango_font_description_free(desc);

	// set text to layout - -1 indicates text is null terminated & length will be auto calculated
	pango_layout_set_text(layout, str.c_str(), -1);

	// save to file
	cairo_status_t cairoStatus;
	char * filename
	cairoStatus = cairo_surface_write_to_png(cairoSurface, filename);
	if (cairoStatus != CAIRO_STATUS_SUCCESS) {
		// error saving
	}

	cairo_destroy(cairoContext);
	cairo_surface_destroy(cairoSurface);
}